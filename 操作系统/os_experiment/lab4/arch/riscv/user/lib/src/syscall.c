#include "syscall.h"


struct ret_info u_syscall(uint64_t syscall_num, uint64_t arg0, uint64_t arg1, uint64_t arg2, \
                uint64_t arg3, uint64_t arg4, uint64_t arg5){
    struct ret_info ret;
    // TODO: 完成系统调用，将syscall_num放在a7中，将参数放在a0-a5中，触发ecall，将返回值放在ret中
    asm volatile("mv a0, %0" : : "r"(arg0));
    asm volatile("mv a1, %0" : : "r"(arg1));
    asm volatile("mv a2, %0" : : "r"(arg2));
    asm volatile("mv a3, %0" : : "r"(arg3));
    asm volatile("mv a4, %0" : : "r"(arg4));
    asm volatile("mv a5, %0" : : "r"(arg5));
    asm volatile("mv a7, %0" : : "r"(syscall_num));
    asm volatile("ecall");
    asm volatile("mv %0, a0" : "=r"(ret.a0));
    asm volatile("mv %0, a1" : "=r"(ret.a1));
                  
    return ret;
}