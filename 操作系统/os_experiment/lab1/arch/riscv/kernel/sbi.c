#include "defs.h"

struct sbiret sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0, uint64_t arg1,
                       uint64_t arg2, uint64_t arg3, uint64_t arg4,
                       uint64_t arg5) {
struct sbiret ret;
/*将 ext (Extension ID) 放入寄存器 a7 中，fid (Function ID) 放入寄存器 a6 中，将 arg0 ~ arg5 放入寄存器 a0 ~ a5 中。
使用 ecall 指令。ecall 之后系统会进入 M 模式，之后 OpenSBI 会完成相关操作。
OpenSBI 的返回结果会存放在寄存器 a0 ， a1 中，其中 a0 为 error code， a1 为返回值， 我们用 sbiret  结构来接受这两个返回值。*/
__asm__ volatile(
    // 汇编指令
    "mv a0, %[arg0]\n"
    "mv a1, %[arg1]\n"
    "mv a2, %[arg2]\n"
    "mv a3, %[arg3]\n"
    "mv a4, %[arg4]\n"
    "mv a5, %[arg5]\n"
    "mv a6, %[fid]\n"
    "mv a7, %[ext]\n"
    "ecall\n"
    "mv %[error], a0\n"
    "mv %[value], a1\n"
    // 输出操作数
    : [error]"=r"(ret.error), [value]"=r"(ret.value)
    // 输入操作数
    : [arg0]"r"(arg0), [arg1]"r"(arg1), [arg2]"r"(arg2), [arg3]"r"(arg3), [arg4]"r"(arg4), [arg5]"r"(arg5),
      [fid]"r"(fid), [ext]"r"(ext)
    // 可能影响的寄存器或存储器
    : "memory", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7");

return ret;
}
