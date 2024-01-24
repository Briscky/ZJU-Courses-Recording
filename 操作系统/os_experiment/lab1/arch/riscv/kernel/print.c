#include "defs.h"
extern struct sbiret sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0,
                              uint64_t arg1, uint64_t arg2, uint64_t arg3,
                              uint64_t arg4, uint64_t arg5);

int puts(char *str) {
  // TODO
  while((*str) !='\0'){
    sbi_call(1,0,(uint64_t)*str,0,0,0,0,0);
    str++;
  }
  return 0;
}

int put_num(uint64_t n) {
  // TODO
  if(n == 0){
    sbi_call(1, 0, (uint64_t)('0'), 0, 0, 0, 0, 0);
    return 0;
  }

  char buffer[21]; 
  int pos = 20;
  buffer[pos] = '\0'; // 字符串尾部

  while (n > 0) {
      buffer[--pos] = '0' + (n % 10); // 将当前最低位数字转为字符，放入buffer
      n /= 10; // 去掉最低位数字
  }

  puts(&buffer[pos]); // 调用puts函数进行打印
  return 0;
}