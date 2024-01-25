1.
在本次作业中，最开始我尝试使用string的数据结构来
完成大数运算，但发现在大数除法中由于需要多次调用
减法函数并计算余数，因此采用动态存储以及string的
结构不易实现。在查阅参考网上思路之后采用了：
·静态数组
·通过对输入的数字A和B的第一个char是否为'-'对
flag赋值表示大数的正负号，从而完成负数的运算

2.
Large_Number_String_Bug.cpp 
	完成+ - * 的正负整数运算
Large_Number_Array_Final.cpp 
	完成 + - * / % 的正负整数运算
验收时请TAJJ使用后者代码

3.
Large_Number_Array_Final.cpp 
Input 用户可以自由选择：
·Mode 1： 手动输入大数
·Mode 2： 手动输入txt文件地址与文件名，读入大数
·Operator： 用户通过1-5选择需要进行的运算操作
Output:
·输出最终的运算结果

