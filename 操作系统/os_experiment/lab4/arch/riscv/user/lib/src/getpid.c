#include "getpid.h"
#include "syscall.h"
#include "types.h"

uint64_t current_sp() {
  register void *current_sp __asm__("sp");
  return (uint64_t)current_sp;
}
long getpid() {
  // TODO: 完成系统调用，返回当前进程的 pid
  struct ret_info ret=u_syscall(SYS_GETPID, 0,0,0, 0, 0, 0);
  long syscall_ret;
  syscall_ret=ret.a0;
  return syscall_ret;
}
