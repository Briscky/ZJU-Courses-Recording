#include "clock.h"
#include "riscv.h"
#include "sched.h"
#include "init.h"

#define SSTATUS_SPIE (1UL << 4)
void intr_enable(void) {
  // TODO: 设置 sstatus[spie] = 1
  set_csr(sstatus, SSTATUS_SPIE);
}

void intr_disable(void) {
  // TODO: 设置 sstatus[spie] = 0
  clear_csr(sstatus, SSTATUS_SPIE);
}

void idt_init(void) {
  extern void trap_s(void);
  // 向 stvec 寄存器中写入中断处理后跳转函数的地址
  write_csr(stvec, trap_s);
}

void init(void) {
  idt_init();
  intr_enable();
  clock_init();
  task_init();
}
