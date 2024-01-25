#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <algorithm>
#include <stdio.h>
using namespace std;

#define Length 800		//the length of large integers;

//turn string to int and with right alignment;
void str_to_arr(string Str, int Arr[Length], int Start)
{
	int i, j;
	for (j = 0; j < Start; j++)
	{
		Arr[j] = 0;
	}
	for (i = 0, j = Start; j < Length; i++, j++)		//right alignment;
	{
		Arr[j] = Str[i] - '0';			//turn char to int;
	}
}

string arr_to_str(int Arr[Length], string Str, int Start)
{
	int i;
	for (i = Start; i < Length; i++)
	{
		(Str).push_back(Arr[i] + '0');
	}
	(Str).push_back('\0');

	if (Str[0] == '0') {
		Str.erase(0, 1);
	}

	return Str;
}

string add(string StrA, string StrB)
{
	int LenA, LenB;
	LenA = StrA.length();
	LenB = StrB.length();

	int StartA, StartB;
	StartA = Length - LenA;
	StartB = Length - LenB;

	int ArrA[Length], ArrB[Length], ArrC[Length];
	str_to_arr(StrA, ArrA, StartA);
	str_to_arr(StrB, ArrB, StartB);

	int StartMin, StartMax;
	StartMin = StartA < StartB ? StartA : StartB;

	int i;
	int Temp;
	int Carry = 0;
	for (i = Length - 1; i >= StartMin; i--)
	{
		Temp = ArrA[i] + ArrB[i] + Carry;
		ArrC[i] = Temp % 10;
		if (Temp > 9) { Carry = 1; }
		else { Carry = 0; }
	}
	if (Carry > 0)
	{
		ArrC[--StartMin] = Carry;
	}

	string StrC;
	StrC = arr_to_str(ArrC, StrC, StartMin);

	return StrC;
}

string sub(string StrA, string StrB)
{
	int LenA, LenB;
	LenA = StrA.length();
	LenB = StrB.length();

	int StartA, StartB;
	StartA = Length - LenA;
	StartB = Length - LenB;

	int ArrA[Length], ArrB[Length], ArrC[Length];
	str_to_arr(StrA, ArrA, StartA);
	str_to_arr(StrB, ArrB, StartB);

	int i;
	int Temp;
	int Borrow = 0;
	for (i = Length - 1; i >= StartA; i--)
	{
		Temp = ArrB[i] + Borrow;
		if (ArrA[i] < Temp)
		{
			ArrC[i] = ArrA[i] - Temp + 10;
			Borrow = 1;
		}
		else
		{
			ArrC[i] = ArrA[i] - Temp;
			Borrow = 0;
		}
	}

	string StrC;
	StrC = arr_to_str(ArrC, StrC, StartA);

	return StrC;
}

string mul(string StrA, string StrB)
{
	int LenA, LenB;
	LenA = StrA.length();
	LenB = StrB.length();

	if (StrA == "0" || StrB == "0")
	{
		std::cout << 0 << endl;
		return NULL;
	}

	int StartA, StartB;
	StartA = Length - LenA;
	StartB = Length - LenB;

	int ArrA[Length], ArrB[Length], ArrC[Length], Sum[Length];
	memset(Sum, 0, sizeof(Sum));
	str_to_arr(StrA, ArrA, StartA);
	str_to_arr(StrB, ArrB, StartB);

	int i, j, k;
	int Temp;
	int CarryMul = 0, CarrySum = 0;
	for (j = Length - 1; j >= StartB; j--)
	{
		k = j + 1;
		CarryMul = 0;
		for (i = Length - 1; i >= StartA; i--)
		{
			Temp = ArrB[j] * ArrA[i] + CarryMul;
			ArrC[--k] = Temp % 10;
			CarryMul = Temp / 10;
		}
		if (CarryMul > 0)
		{
			ArrC[--k] = CarryMul;
		}

		for (i = j; i >= k; i--)
		{
			Temp = ArrC[i] + Sum[i] + CarrySum;
			Sum[i] = Temp % 10;
			CarrySum = Temp / 10;
		}
		if (CarrySum > 0)
		{
			Sum[i] = CarrySum;
		}
	}

	string StrC;
	StrC = arr_to_str(Sum, StrC, k);

	return StrC;
}

void c_to_d(char str[Length], char d[Length], int &start)
{//把str的各位转变成对应整数，右对齐存入d, start为最高位下标 
	int len; //strlen(str)=5 N=10 start =10-5=5 
	int i, j;
	for (i = 0; i<Length; i++)
		d[i] = 0;
	len = strlen(str);
	start = Length - len;
	for (i = 0, j = start; i<len; i++, j++)
		d[j] = str[i] - '0';
}

void d_to_c(char d[Length], char str[Length], int start)
{
	//把d中数字d[start...N-1]转变为字符左对齐存入str 
	int i, j;
	int len = Length - start;
	for (i = 0, j = start; i<len; i++, j++)
		str[i] = d[j] + '0';
	str[len] = '\0';
}

void sub_for_div(char str_a[Length], char str_b[Length], char str_c[Length])
{//已假设a>b 
	char a[Length], b[Length], c[Length]; //以右对齐的方式存储各位数 
	int i;
	int start_a, start_b;//分别存储a,b,c中最高位的下标 
	int borrow;//借位 

			   //把str_a,str_b的各位转变成对应整数，右对齐存入a,b 
	c_to_d(str_a, a, start_a);
	c_to_d(str_b, b, start_b);

	//从低位到高位，对应位相减，注意借位 
	memset(c, 0, sizeof(c));//将c中有效的个字节替换为0并返回c； 
	i = Length - 1;
	borrow = 0;
	while (i >= start_a)
	{
		c[i] = a[i] - b[i] - borrow;
		if (c[i]<0) //借位 
		{
			borrow = 1;
			c[i] += 10;
		}
		else //注意不可缺省 
			borrow = 0;

		i--;
	}

	//去掉结果前面的0 
	while (i<Length && c[i] == 0)i++;

	//把c转化为字符串str_c 
	d_to_c(c, str_c, i);//i是c中最高位下标 
}

int compare(char str1[Length], char str2[Length]) //str1,str2内存的是要进行比较的大整数 
{ //如果前者大于后者，返回正数，若相等返回0，否则返回负数 
	int len1, len2;
	len1 = strlen(str1);
	len2 = strlen(str2);
	if (len1<len2)
		return -1;
	else if (len1>len2)return 1;
	else
		return strcmp(str1, str2);
}

void div(char str_a[Length], char str_b[Length], char str_c[Length])
{//已假设a>b 
	char c[Length]; //以左对齐的方式存储各位数 
	char rmd[Length];//存储余数 
	char temp[Length];
	int i, k, len_rmd, len1, len2, cur;
	memset(c, 0, sizeof(c));

	//准备工作,先在被除数中取和除数同样多的位数 
	len1 = strlen(str_a);
	len2 = strlen(str_b);
	strcpy(rmd, str_a);
	rmd[len2] = '\0';
	cur = len2 - 1; //cur记下当前商对应的a中的下标 

					//如果不够除，再向右取1位 
	if (compare(rmd, str_b)<0)
	{
		cur++;
		rmd[cur] = str_a[cur];
		rmd[cur + 1] = '\0';
	}

	//求商的过程是从高位到低位进行，商的每一位计算：需要多次执行减法,减法次数即商的一位。 
	k = 0;
	while (str_a[cur] != '\0')
	{
		while (compare(rmd, str_b) >= 0) //本循环确定str_a[cur]对应位置的商 
		{
			sub_for_div(rmd, str_b, temp);
			c[k]++;
			strcpy(rmd, temp);
		}

		//右移一位后继续上述过程 
		cur++;
		len_rmd = strlen(rmd);
		rmd[len_rmd] = str_a[cur];//照抄被除数的一位 
		rmd[len_rmd + 1] = '\0';
		k++;
	}

	//将c[0...k-1]转换成字符存入str_c 
	for (i = 0; i<k; i++)
		str_c[i] = c[i] + '0';
	str_c[i] = '\0';

	cout << str_c;
}

int main()
{
	////read large integers;
	//ifstream inFile;
	//inFile.open("C:\\Large_Num.txt");

	//if (!inFile)
	//{
	//	cout << "Error" << std::endl;
	//}

	//cout << "**** Large Integers ****" << endl;
	//cout << "**** Chose the mode of input ****" << endl;
	//cout << "**** To read large numbers from file from C:\\Large_Num.txt : Input 1 ****" << endl;
	//cout << "**** To input large numbers directly : Input 2 ****" << endl;

	//string StrA, StrB, StrC;
	//int ModeInput;
	//cin >> ModeInput;
	//if (ModeInput == 1)
	//{
	//	getline(inFile, StrA);
	//	cout << "Num A is: " << StrA << endl;
	//	getline(inFile, StrB);
	//	cout << "Num B is: " << StrB << endl;
	//}
	//else if (ModeInput == 2)
	//{
	//	cout << "**** Input the first large number: ****" << endl;
	//	cin >> StrA;
	//	cout << "**** Input the second large number: ****" << endl;
	//	cin >> StrB;
	//}

	char str_a[Length], str_b[Length], str_c[Length];
	cin >> str_a;
	cin >> str_b;
	div(str_a, str_b, str_c);

	////choose operations;
	//cout << "**** Please choose the operation ****" << endl;
	//cout << "**** + : input 1 ****" << endl;
	//cout << "**** - : input 2 ****" << endl;
	//cout << "**** * : input 3 ****" << endl;
	//cout << "**** / : input 4 ****" << endl;


	//int ModeOperation;
	//cin >> ModeOperation;

	//switch (ModeOperation)
	//{
	//case 1: StrC = add(StrA, StrB); break;
	//case 2: StrC = sub(StrA, StrB); break;
	//case 3: StrC = mul(StrA, StrB); break;
	//case 4: div(str_a, str_b, str_c); break;
	//}

	//cout << StrC;
	

	system("pause");
	return 0;
}
