#include "sched.h"
#include "test.h"
#include "stdio.h"

#define Kernel_Page 0x80210000
#define LOW_MEMORY 0x80211000
#define PAGE_SIZE 4096UL

extern void __init_sepc();

struct task_struct* current;
struct task_struct* task[NR_TASKS];

// If next==current,do nothing; else update current and call __switch_to.
void switch_to(struct task_struct* next) {
  if (current != next) {
    struct task_struct* prev = current;
    current = next;
    __switch_to(prev, next);
  }
}

int task_init_done = 0;
// initialize tasks, set member variables
void task_init(void) {
  puts("task init...\n");

  for(int i = 0; i < LAB_TEST_NUM; ++i) {
    // TODO
    // initialize task[i]
    // get the task_struct based on Kernel_Page and i
    // set state = TASK_RUNNING, counter = 0, priority = 5, 
    // blocked = 0, pid = i, thread.sp, thread.ra
    //  struct task_struct {
    // long state;                  // 进程状态 Lab3中进程初始化时置为TASK_RUNNING
    // long counter;                // 运行剩余时间
    // long priority;               // 运行优先级 1最高 5最低
    // long blocked;
    // long pid;                    // 进程标识符
    //                              // Above Size Cost: 40 bytes

    // struct thread_struct thread; // 该进程状态段
    task[i]= (struct task_struct*) (Kernel_Page + i * PAGE_SIZE);
    task[i]->state = TASK_RUNNING; 
    task[i]->counter = 0; 
    task[i]->priority = 5; 
    task[i]->blocked = 0; 
    task[i]->pid = i;
    task[i]->thread.sp = (unsigned long long)(LOW_MEMORY + i * PAGE_SIZE);
    task[i]->thread.ra=(unsigned long long)__init_sepc;
   
    printf("[PID = %d] Process Create Successfully!\n", task[i]->pid);
  }
  task_init_done = 1;
}

void call_first_process() {
  // set current to 0x0 and call schedule()
  current = (struct task_struct*)(Kernel_Page + LAB_TEST_NUM * PAGE_SIZE);
  current->pid = -1;
  current->counter = 0;
  current->priority = 0;

  schedule();
}


void show_schedule(unsigned char next) {
  // show the information of all task and mark out the next task to run
  for (int i = 0; i < LAB_TEST_NUM; ++i) {
    if (task[i]->pid == next) {
      printf("task[%d]: counter = %d, priority = %d <-- next\n", i,
             task[i]->counter, task[i]->priority);
    } else {
      printf("task[%d]: counter = %d, priority = %d\n", i, task[i]->counter,
           task[i]->priority);
    }
  }
}

#ifdef SJF
// simulate the cpu timeslice, which measn a short time frame that gets assigned
// to process for CPU execution
void do_timer(void) {
  if (!task_init_done) return;

  printf("[*PID = %d] Context Calculation: counter = %d,priority = %d\n",
         current->pid, current->counter, current->priority);
  
  // current process's counter -1, judge whether to schedule or go on.
  // TODO
  current->counter-=1;
  if(current->counter==0){
    schedule();
  }
}


// Select the next task to run. If all tasks are done(counter=0), reinitialize all tasks.
void schedule(void) {
  unsigned char next = 64;
  // TODO
  int all_zero = 1;
  int min_time = 10000000;
  while(all_zero){
    for(int i=NR_TASKS-1; i>=0; i--){
      if(task[i]){
        if (task[i]->counter<min_time && task[i]->counter!=0){
          all_zero = 0;
          next=i;
          min_time=task[i]->counter;
        }
      }
    }

    if(all_zero){
      init_test_case();
      //call_first_process();
    }
  }
  
  show_schedule(next);
  
  switch_to(task[next]);
}

#endif

#ifdef PRIORITY

// simulate the cpu timeslice, which measn a short time frame that gets assigned
// to process for CPU execution
void do_timer(void) {
  if (!task_init_done) return;
  
  printf("[*PID = %d] Context Calculation: counter = %d,priority = %d\n",
         current->pid, current->counter, current->priority);
  
  // current process's counter -1, judge whether to schedule or go on.
  // TODO
  current->counter-=1;
  schedule();
 
}

// Select the task with highest priority and lowest counter to run. If all tasks are done(counter=0), reinitialize all tasks.
void schedule(void) {
  unsigned char next = NR_TASKS - 1;
  // TODO
  int max_priority = 6;
  int all_zero = 1;
  while(all_zero){
    for(int i=NR_TASKS-1; i>=0; i--){
      if(task[i]){
        if ((!task[next]||
            (task[i]->priority<max_priority || task[i]->priority==max_priority && task[i]->counter<task[next]->counter))&& 
            task[i]->counter!=0){
          all_zero = 0;
          next = i;
          max_priority = task[i]->priority;
        }
      }
    }
    if(all_zero){
      init_test_case();
    }
  }
  

  show_schedule(next);
  
  switch_to(task[next]);
}

#endif