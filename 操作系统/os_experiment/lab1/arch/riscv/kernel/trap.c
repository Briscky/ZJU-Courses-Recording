#ifndef PRINT_ONLY
#include "defs.h"

extern main(), puts(), put_num(), ticks;
extern void clock_set_next_event(void);

void handler_s(uint64_t cause) {
  // TODO
  // interrupt
  if (cause & (1UL << 63)) {
    // supervisor timer interrupt
    if ((cause & 0x7FFFFFFF) == 5) {
      // 设置下一个时钟中断，打印当前的中断数目
      // TODO
      puts("Interrupt count: ");
      put_num(ticks);
      puts("\n");
      clock_set_next_event();
    }
  }
}
#endif