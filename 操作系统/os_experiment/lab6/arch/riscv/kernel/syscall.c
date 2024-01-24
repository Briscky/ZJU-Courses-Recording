#include "syscall.h"
#include "list.h"
#include "riscv.h"
#include "sched.h"
#include "task_manager.h"
#include "stdio.h"
#include "defs.h"
#include "slub.h"
#include "mm.h"
#include "vm.h"

extern uint64_t text_start;
extern uint64_t rodata_start;
extern uint64_t data_start;
extern uint64_t user_program_start;
extern void trap_s_bottom(void);

int strcmp(const char *a, const char *b) {
  while (*a && *b) {
    if (*a < *b)
      return -1;
    if (*a > *b)
      return 1;
    a++;
    b++;
  }
  if (*a && !*b)
    return 1;
  if (*b && !*a)
    return -1;
  return 0;
}

uint64_t get_program_address(const char * name) {
    uint64_t offset = 0;
    if (strcmp(name, "hello") == 0) offset = PAGE_SIZE;
    else if (strcmp(name, "malloc") == 0) offset = PAGE_SIZE * 2;
    else if (strcmp(name, "print") == 0) offset = PAGE_SIZE * 3;
    else if (strcmp(name, "guess") == 0) offset = PAGE_SIZE * 4;
    else {
        printf("Unknown user program %s\n", name);
        while (1);
    }
    return PHYSICAL_ADDR((uint64_t)(&user_program_start) + offset);
}

struct ret_info syscall(uint64_t syscall_num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t sp) {
    uint64_t* sp_ptr = (uint64_t*)(sp);

    struct ret_info ret;
    switch (syscall_num) {
    case SYS_GETPID: {
        ret.a0 = getpid();
        sp_ptr[4] = ret.a0;
        sp_ptr[16] += 4;
        break;
    }
    case SYS_READ: {
        ret.a0 = getchar();
        sp_ptr[4] = ret.a0;
        sp_ptr[16] += 4;
        break;
    }
    case SYS_FORK: {
        // TODO:
        // 1. create new task and set counter, priority and pid (use our task array)
        // 2. create root page table, set current process's satp
        //   2.1 copy current process's user program address, create mapping for user program
        //   2.2 create mapping for kernel address
        //   2.3 create mapping for UART address
        // 3. create user stack, copy current process's user stack and save user stack sp to new_task->sscratch
        // 4. copy mm struct and create mapping
        // 5. set current process a0 = new task pid, sepc += 4
        // 6. copy kernel stack (only need trap_s' stack)
        // 7. set new process a0 = 0, and ra = trap_s_bottom, sp = register number * 8
        //1
        int i;
        for (i = 0; i < NR_TASKS; i++) {
            if (task[i] == NULL) {  // 找到空闲的槽位
                task[i] = (struct task_struct*)(VIRTUAL_ADDR(alloc_page()));
                task[i]->counter = 5;  
                task[i]->priority = 1;
                task[i]->pid = i;           // 使用索引作为PID
                break;
            }
        }
        
        //2
        uint64_t root_page_table = alloc_page();
        task[i]->satp = root_page_table >> 12 | 0x8000000000000000 | (((uint64_t) (task[i]->pid))  << 44);
        create_mapping((uint64_t*)root_page_table, 0x1000000, current->mm.user_program_start, PAGE_SIZE, PTE_V | PTE_R | PTE_X | PTE_U | PTE_W);
        
        // 调用 create_mapping 函数将虚拟地址 0xffffffc000000000 开始的 16 MB 空间映射到起始物理地址为 0x80000000 的 16MB 空间
        create_mapping(root_page_table, 0xffffffc000000000, 0x80000000, 16 * 1024 * 1024, PTE_V | PTE_R | PTE_W | PTE_X);
        // 修改对内核空间不同 section 所在页属性的设置，完成对不同section的保护，其中text段的权限为 r-x, rodata 段为 r--, 其他段为 rw-。
        create_mapping(root_page_table, 0xffffffc000000000, 0x80000000, (uint64_t)&rodata_start - 0x80000000, PTE_V | PTE_R | PTE_X);
        create_mapping(root_page_table, (uint64_t)&rodata_start-0x80000000+0xffffffc000000000, (uint64_t)&rodata_start, (uint64_t)&data_start - (uint64_t)&rodata_start, PTE_V | PTE_R);
        create_mapping(root_page_table, (uint64_t)&data_start-0x80000000+0xffffffc000000000, (uint64_t)&data_start, (uint64_t)&_end - (uint64_t)&data_start, PTE_V | PTE_R | PTE_W);        
        
        // 改了（lab3遗留bug,用了lab4的==）
        // 对内核起始地址 0x80000000 的16MB空间做等值映射（将虚拟地址 0x80000000 开始的 16 MB 空间映射到起始物理地址为 0x80000000 的 16MB 空间）
        create_mapping((uint64_t*)root_page_table, 0x80000000, 0x80000000, 16 * 1024 * 1024, PTE_V | PTE_R | PTE_W | PTE_X);
        // 修改对内核空间不同 section 所在页属性的设置，完成对不同section的保护，其中text段的权限为 r-x, rodata 段为 r--, 其他段为 rw-。
        create_mapping((uint64_t*)root_page_table, 0x80000000, 0x80000000, PHYSICAL_ADDR((uint64_t)&rodata_start) - 0x80000000, PTE_V | PTE_R | PTE_X);
        create_mapping((uint64_t*)root_page_table, PHYSICAL_ADDR((uint64_t)&rodata_start), PHYSICAL_ADDR((uint64_t)&rodata_start), (uint64_t)&data_start - (uint64_t)&rodata_start, PTE_V | PTE_R);
        create_mapping((uint64_t*)root_page_table, PHYSICAL_ADDR((uint64_t)&data_start), PHYSICAL_ADDR((uint64_t)&data_start), (uint64_t)&_end - (uint64_t)&data_start, PTE_V | PTE_R | PTE_W);
        
        // 将必要的硬件地址（如 0x10000000 为起始地址的 UART ）进行等值映射 ( 可以映射连续 1MB 大小 )，无偏移，3 为映射的读写权限
        create_mapping((uint64_t*)root_page_table, 0x10000000, 0x10000000, 1 * 1024 * 1024, PTE_V | PTE_R | PTE_W | PTE_X);

        //3
        uint64_t physical_stack = alloc_page();
        task[i]->mm.user_stack = physical_stack;
        create_mapping((uint64_t*)root_page_table, 0x1001000, physical_stack, PAGE_SIZE, PTE_V | PTE_R | PTE_W | PTE_U);
        memcpy((uint64_t *)task[i]->mm.user_stack, (uint64_t *)current->mm.user_stack, PAGE_SIZE);
        task[i]->sscratch = read_csr(sscratch);


        //4
        task[i]->mm.vm = kmalloc(sizeof(struct vm_area_struct));
        INIT_LIST_HEAD(&(task[i]->mm.vm->vm_list));
        struct vm_area_struct* vma;
        struct vm_area_struct* new_vma;
        list_for_each_entry(vma, &current->mm.vm->vm_list, vm_list) {
            new_vma = (struct vm_area_struct*)kmalloc(sizeof(struct vm_area_struct));
            new_vma->vm_start = vma->vm_start;
            new_vma->vm_end = vma->vm_end;
            new_vma->vm_flags = vma->vm_flags;
            new_vma->mapped = vma->mapped;
            list_add(&(new_vma->vm_list), &(task[i]->mm.vm->vm_list));
            //改了
            if (vma->mapped == 1) {
                uint64_t pte = get_pte((current->satp&(0xFFFFFFFFFFF))<<12, vma->vm_start);
                uint64_t temp = alloc_pages((vma->vm_end - vma->vm_start) / PAGE_SIZE);
                create_mapping((uint64_t*)root_page_table, new_vma->vm_start, temp, new_vma->vm_end - new_vma->vm_start, vma->vm_flags);
                memcpy((uint64_t*)temp, (uint64_t*)((pte >> 10) << 12), (vma->vm_end - vma->vm_start));
            }  
        }
        
        //5
        sp_ptr[4] = task[i]->pid;// a0 = new task pid
        sp_ptr[16] += 4;// sepc += 4
        
        //改了（抄了lab7==）
        //6
        memcpy((uint64_t*)((uint64_t)task[i] + PAGE_SIZE - 31 * 8),(uint64_t*)((uint64_t)current + PAGE_SIZE - 31 * 8),31 * 8);// 复制内核栈

        //7
        ((uint64_t*)((uint64_t)task[i] + PAGE_SIZE - 31 * 8))[4] = 0;
        task[i]->thread.ra = (uint64_t)trap_s_bottom;
        task[i]->thread.sp = (uint64_t)task[i] + PAGE_SIZE - 8 * 31;
        break; 

    }
    case SYS_EXEC: {
        // TODO:
        // 1. free current process vm_area_struct and it's mapping area
        // 2. reset user stack, user_program_start
        // 3. create mapping for new user program address
        // 4. set sepc = 0x1000000
        // 5. refresh TLB
        //清空当前进程的动态内存分配区域 task->mm.vm ，包括映射和空间回收
        struct vm_area_struct* vma;
        uint64_t root_page_table = (current->satp & (0xFFFFFFFFFFF)) << 12;
        list_for_each_entry(vma, &current->mm.vm->vm_list, vm_list)
        {
            if (vma->mapped == 1) {
                uint64_t pte = get_pte(root_page_table, vma->vm_start);
                free_pages((pte >> 10) << 12);
            }
            create_mapping(root_page_table, vma->vm_start, 0, (vma->vm_end - vma->vm_start), 0);
            list_del(&(vma->vm_list));
            kfree(vma);
        }
        // 重置用户栈指针（ssratch）【我们复用用户栈】和 user_program_start【我们复用用户进程代码】
        write_csr(sscratch, 0x1001000 + PAGE_SIZE);// 重置用户栈指针
        current->mm.user_program_start = get_program_address((const char*)arg0);
        // 重新设置用户进程代码的虚拟地址映射
        create_mapping((uint64_t*)root_page_table, 0x1000000, current->mm.user_program_start, PAGE_SIZE, PTE_V | PTE_U | PTE_R | PTE_X | PTE_W);
        asm volatile ("sfence.vma");
        // 设置 sepc = 0x1000000
        //改了
        sp_ptr[16] = 0x1000000;
        break;
    }
    case SYS_EXIT: {
        // TODO:
        // 1. free current process vm_area_struct and it's mapping area
        // 2. free user stack
        // 3. clear current task, set current task->counter = 0
        // 4. call schedule
        // 清空当前进程的动态内存分配区域 task->mm.vm ，包括映射和空间回收
        struct vm_area_struct* vma;
        list_for_each_entry(vma, &current->mm.vm->vm_list, vm_list) {
            if (vma->mapped == 1) {
                uint64_t pte = get_pte((current->satp&(0xFFFFFFFFFFF))<<12, vma->vm_start);
                free_pages((pte >> 10) << 12);
            }
            create_mapping((current->satp&(0xFFFFFFFFFFF))<<12, vma->vm_start, 0, (vma->vm_end - vma->vm_start), 0);
            list_del(&(vma->vm_list));
            kfree(vma);
        }
        kfree(&(current->mm.vm));
        current->mm.vm = NULL;
        // 清空用户栈
        free_pages(current->mm.user_stack);
        current->mm.user_stack = 0;
        // 清空当前 task_struct 信息，设置 counter = 0，避免再次调度
        task[current->pid] = NULL;
        free_pages((current->satp&(0xFFFFFFFFFFF))<<12);
        current->counter = 0;
        // 调用 schedule，执行别的进程
        schedule();
        break;
    }
case SYS_WAIT: {
        // TODO:
        // 1. find the process which pid == arg0
        // 2. if not find
        //   2.1. sepc += 4, return
        // 3. if find and counter = 0
        //   3.1. free it's kernel stack and page table
        // 4. if find and counter != 0
        //   4.1. change current process's priority
        //   4.2. call schedule to run other process
        //   4.3. goto 1. check again
        int pid = arg0;
        int process_found = 0;
        //int waiting = 1;
        int i;
        for(i = 0; i < NR_TASKS; i++) {
            if(task[i]) {
                if(task[i]->pid == pid) {
                    process_found = 1;
                    break;
                }
            }
        }
        //改了
        if(process_found) {
            while(task[i]) {
                if(task[i]->counter == 0) {
                    free_pages(task[i]->thread.sp + 31 * 8 - PAGE_SIZE);// 释放内核栈
                    free_pages((task[i]->satp&(0xFFFFFFFFFFF))<<12);// 释放页表
                    task[i] = NULL;
                    break;
                } else {
                    current->priority = task[i]->priority + 1;
                    schedule();
                }
            }
        }

        if(!process_found) {
            sp_ptr[16] += 4;
        }

        break;
    }
    case SYS_WRITE: {
        int fd = arg0;
        char* buffer = (char*)arg1;
        int size = arg2;
        if(fd == 1) {
            for(int i = 0; i < size; i++) {
                putchar(buffer[i]);
            }
        }
        ret.a0 = size;
        sp_ptr[4] = ret.a0;
        sp_ptr[16] += 4;
        break;
    }
    case SYS_MMAP: {
        struct vm_area_struct* vma = (struct vm_area_struct*)kmalloc(sizeof(struct vm_area_struct));
        if (vma == NULL) {
            ret.a0 = -1;
            break;
        }
        vma->vm_start = arg0;
        vma->vm_end = arg0 + arg1;
        vma->vm_flags = arg2;
        vma->mapped = 0;
        list_add(&(vma->vm_list), &(current->mm.vm->vm_list));

        ret.a0 = vma->vm_start;
        sp_ptr[16] += 4;
        break;
    }
    case SYS_MUNMAP: {
        ret.a0 = -1;
        struct vm_area_struct* vma;
        list_for_each_entry(vma, &current->mm.vm->vm_list, vm_list) {
            if (vma->vm_start == arg0 && vma->vm_end == arg0 + arg1) { 
                if (vma->mapped == 1) {
                    uint64_t pte = get_pte((current->satp & ((1ULL << 44) - 1)) << 12, vma->vm_start);
                    free_pages((pte >> 10) << 12);
                }
                create_mapping((current->satp & ((1ULL << 44) - 1)) << 12, vma->vm_start, 0, (vma->vm_end - vma->vm_start), 0);
                list_del(&(vma->vm_list));
                kfree(vma);

                ret.a0 = 0;
                break;
            }
        }
        // flash the TLB
        asm volatile ("sfence.vma");
        sp_ptr[16] += 4;
        break;
    }
    default:
        printf("Unknown syscall! syscall_num = %d\n", syscall_num);
        while(1);
        break;
    }
    return ret;
}