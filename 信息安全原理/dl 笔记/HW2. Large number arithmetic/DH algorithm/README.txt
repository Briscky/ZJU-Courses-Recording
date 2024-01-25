Test 2 中prime number与primitive root为手动输入来
自于rfc, Private Key采用大致确定长度后随机生成，
Public Key与Session Key均运用文件夹图片中所证明
的算法进行计算生成，并输出至对应的txt文件中。

此外，由于最后生成的Session Key位数较长，为了保
证最后A与B生成Key完全一样，采用了将两个结果再次
进行一次减法后与0进行比较的方法，如果两者相减为0，
则证明运算正确。因此可能增加了程序运行的时间。

exe需要在有prime.txt与primitive.txt的情况下才可
以正常运行。