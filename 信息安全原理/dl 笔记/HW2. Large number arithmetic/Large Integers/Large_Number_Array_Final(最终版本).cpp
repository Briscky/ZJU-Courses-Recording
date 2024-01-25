#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Length 800			//the length of large numbers;
char A[Length], B[Length], Result[Length*2];		//large numbers needed to be operated and the result; 
char MulA[10][Length + 1];	//the result of A * 0~9;
char Quotient[Length],Remainder[Length];		//the quotient and remainder of division;

//***ADD***
//Function: to output the result of add;
//Algorithm: from the end to the start to add each pair of bit from the large numbers and use carry-on;
//Input requirement: the length of A is not shorter than the length of B;
void add_main(char *A, char *B)
{
	int LenA, LenB;
	int Carry = 0;
	int Temp = 0;
	int ka, kb;
	int i;

	LenA = strlen(A);
	LenB = strlen(B);

	Result[LenA] = '\0';
	kb = LenB - 1;
	for (ka = LenA - 1; ka >= 0; ka--)
	{
		Temp = A[ka] - '0' + Carry;
		if (kb >= 0)		//if B[kb] is still exist;
		{
			Temp += B[kb] - '0';
		}

		if (Temp > 9)		//carry-on which means last bit should be added one;
		{
			Result[ka] = Temp - 10 + '0';
			Carry = 1;
		}
		else				//no carry-on;
		{
			Result[ka] = Temp + '0';
			Carry = 0;
		}
		kb--;
	}

	if (Carry)				//the first bit have a carry-on bit;
	{
		for (i = LenA; i >= 0; i--)
		{
			Result[i + 1] = Result[i];
		}
		Result[0] = '1';
	}

}

//Funcion: adjust the order of A and B to ensure the length of A is not shorter than that of B;
void add(char *A, char *B)
{
	int LenA, LenB;

	LenA = strlen(A);
	LenB = strlen(B);

	if (LenA >= LenB)
	{
		add_main(A, B);
	}
	else
	{
		add_main(B, A);
	}
}


//***SUBSTRACTION***
//Function: to output the result of sub;
//Algorithm: from the end to the start to add each pair of bit from the large numbers and use borrow;
//Input requirement: the length of A is not shorter than the length of B;
void sub_main(char *A, char *B)
{
	int LenA, LenB;
	int Borrow = 0;
	int Temp = 0;
	int ka, kb;
	int i;

	LenA = strlen(A);
	LenB = strlen(B);

	Result[LenA] = '\0';
	kb = LenB - 1;
	for (ka = LenA - 1; ka >= 0; ka--)
	{
		Temp = A[ka] - '0' - Borrow;
		if (kb >= 0)				//if B[kb] is still exist;
		{
			Temp -= B[kb] - '0';
		}

		if (Temp < 0)				//borrow which means last bit should be subtracted one;
		{
			Result[ka] = Temp + 10 + '0';
			Borrow = 1;
		}
		else						//no borrow;
		{
			Result[ka] = Temp + '0';
			Borrow = 0;
		}
		kb--;
	}

	while(Result[0] == '0')			//the first bit have a borrow bit;
	{
		for (i = 0; i < LenA; i++)
		{
			Result[i] = Result[i+1];
		}
	}
}

//Funcion: adjust the order of A and B to ensure A is not smaller than B;
void sub(char *A, char *B)
{
	int LenA, LenB;

	LenA = strlen(A);
	LenB = strlen(B);

	if (strcmp(A, B) == 0)		//A == B;
	{
		strcpy(Result, "0");
	}
	else if (LenA > LenB || LenA == LenB && strcmp(A,B)>0)		//A > B;
	{
		sub_main(A, B);
	}
	else						//A < B so output "-" first and change the order of A and B;
	{
		printf("-");
		sub_main(B, A);
	}
}


//***MULTIPLICATION***
//Function: to output the result of mul;
//Algorithm: in order to accelerate, store the result of A * 0~9 in an array;
void mul(char *A, char *B)
{
	int LenA, LenB;
	char Temp[Length * 2];
	int i, ka, kb;

	LenA = strlen(A);
	LenB = strlen(B);

	strcpy(MulA[0], "0");			//calculate and store the result of A * 0~9;
	for (i = 1; i < 10; i++)
	{
		add(MulA[i - 1], A);
		strcpy(MulA[i], Result);
	}

	strcpy(Result, "0");			//initialize the result array;

	for (kb = 0; kb < LenB; kb++)
	{
		if (B[kb] != '0')			//if B[kb] is not the last bit;
		{
			strcpy(Temp, MulA[B[kb] - '0']);		//Temp = MulA[B[kb]-'0'] * 10^x, x is based on the position of B[kb];
			for (i = 0; i < LenB - kb - 1; i++)
			{
				strcat(Temp, "0");	//add "0" to Temp which is equal to * 10^x;
			}
			add(Result, Temp);		//add the result of each bit of B mul A to the final result array;
		}
	}

}


//***DIVISION***
//Function: to campare A and B and return 1 when A > B, otherwise return 0;
int compare(char *A, char *B)
{
	int LenA, LenB;

	LenA = strlen(A);
	LenB = strlen(B);

	if (LenA > LenB)
	{
		return 1;
	}
	else if (LenA == LenB && strcmp(A, B) > 0)
	{
		return 1;
	}
	else return 0;
}

//Function: to output the result of div;
//Algorithm: from the start to the end to divide large numbers and store the remainder;
//Input requirement: B is not equal to 0;
void div(char *A, char *B)
{
	int LenA, LenB;
	char Temp[2];				//to store the result of each division;
	int Quo;					//the number of possible quotient which used to compare and remainder and B;
	int i;

	LenA = strlen(A);
	LenB = strlen(B);

	strcpy(MulA[0], "0");
	for (i = 1; i < 10; i++)
	{
		add(MulA[i - 1], B);
		strcpy(MulA[i], Result);
	}

	strcpy(Quotient, "");		//initialize quotient;
	strcpy(Remainder, "");		//initialize remainder;
	strcpy(Temp, "0");			//initialize temp;

	for (i = 0; i < LenA; i++)
	{
		Temp[0] = A[i];			//add one bit to the end of remainder;
		strcat(Remainder, Temp);

		Quo = 1;
		while (Quo != 10 && !compare(MulA[Quo], Remainder) )	//try the possible quotient until MulA[Quo] > Remainder;
		{
			Quo++;
		}

		Temp[0] = Quo - 1 + '0';	//add one bit to the end of quotient;.
		if (!(Temp[0] == '0' && Quotient[0] == '\0'))     //if the start of quotient is 0, then ignore;
		{
			strcat(Quotient, Temp);
		}

		sub(Remainder, MulA[Quo - 1]);
		strcpy(Remainder, Result);	//the remainder after one time of division;
		if (Remainder[0] == '0')	//the condition to judge if remainder is 0;
		{
			Remainder[0] = '\0';
		}
	}

	if (Quotient[0] == '\0')		//the quotient is 0;
	{
		strcpy(Quotient, "0");
	}
	if (Remainder[0] == '\0')		//the remainder is 0;
	{
		strcpy(Remainder, "0");
	}
}


//Function: to input larger numbers A and B, choose mode and output results;
int main()
{
	int LenA = 0, LenB = 0;		//the length of A and B;
	int FlagA = 1, FlagB = 1;	//number > 0, flag = 1; number < 0, flag = -1;
	char ch;                    //input char;
	int i;
	int Mode,Operator;			//Mode: the way to input numbers; Operator: + - * /;
	FILE *fa,*fb;				//Read numbers from files;
	char FlagNum, FlagOpr;		//the Flag to decide whether continue;
	char FileNameA[30], FileNameB[30];

	//input;	
	printf("Please choose the way to input large numbers:\n");
	printf("Input directly: 1\n");		
	printf("Read from files: 2\n");
	scanf("%d", &Mode);
	getchar();

	if (Mode == 1) 
	{
		//input A;
		ch = getchar();
		if (ch == '-')				//if number is negative, change flag to -1;
		{
			FlagA = -1;
			ch = getchar();
		}
		while (ch != '\n')			//input large numbers and store them into arrays;
		{
			A[LenA++] = ch;
			ch = getchar();
		}

		//input B;
		ch = getchar();
		if (ch == '-')
		{
			FlagB = -1;
			ch = getchar();
		}
		while (ch != '\n')
		{
			B[LenB++] = ch;
			ch = getchar();
		}
	}
	else if (Mode == 2)
	{
		printf("Please input filename A: \n");
		scanf("%s", FileNameA);
		fa = fopen(FileNameA, "r");
		if (fa == NULL)
		{
			printf("ERROR!");
			return 0;
		}
		else
		{
			fscanf(fa, "%s", A);
			fclose(fa);
			printf("%s\n", A);
		}

		printf("Please input filename A: \n");
		scanf("%s", FileNameB);
		fb = fopen(FileNameB, "r");
		if (fb == NULL)
		{
			printf("ERROR!");
			return 0;
		}
		else
		{
			fscanf(fb, "%s", B);
			fclose(fb);
			printf("%s\n", B);
		}
	}

	printf("Please choose operator: \n");
	printf("+: input 1\n-: input 2\n*: input 3\n/: input 4\n%%: input 5\n");
	scanf("%d", &Operator);
	getchar();

	//ADD;
	if (Operator == 1)
	{
		printf("A + B = ");
		if (FlagA == 1 && FlagB == 1)
		{
			add(A, B); printf("%s\n", Result);
		}
		else if (FlagA == 1 && FlagB == -1)
		{
			sub(A, B); printf("%s\n", Result);
		}
		else if (FlagA == -1 && FlagB == 1)
		{
			sub(B, A); printf("%s\n", Result);
		}
		else if (FlagA == -1 && FlagB == -1)
		{
			printf("-");
			add(A, B); printf("%s\n", Result);
		}
	}
	//SUB;
	else if (Operator == 2)
	{
		printf("A - B = ");
		if (FlagA == 1 && FlagB == 1)
		{
			sub(A, B); printf("%s\n", Result);
		}
		else if (FlagA == 1 && FlagB == -1)
		{
			add(A, B); printf("%s\n", Result);
		}
		else if (FlagA == -1 && FlagB == 1)
		{
			printf("-");
			add(A, B); printf("%s\n", Result);
		}
		else if (FlagA == -1 && FlagB == -1)
		{
			sub(B, A); printf("%s\n", Result);
		}
	}
	//MUL;
	else if (Operator == 3)
	{
		printf("A * B = ");
		if (FlagA == 1 && FlagB == 1)
		{
			if (A[0] == '0' || B[0] == '0')
			{
				printf("0\n");
			}
			else
			{
				mul(A, B); printf("%s\n", Result);
			}
		}
		else if (FlagA == 1 && FlagB == -1)
		{
			if (A[0] == '0')
			{
				printf("0\n");
			}
			else
			{
				printf("-");
				mul(A, B); printf("%s\n", Result);
			}
		}
		else if (FlagA == -1 && FlagB == 1)
		{
			if (B[0] = '0')
			{
				printf("0\n");
			}
			else
			{
				printf("-");
				mul(A, B); printf("%s\n", Result);
			}
		}
		else if (FlagA == -1 && FlagB == -1)
		{
			mul(A, B); printf("%s\n", Result);
		}
	}
	//DIV;
	else if (Operator == 4)
	{
		printf("A / B = ");
		if (FlagA == 1 && FlagB == 1)
		{
			if (B[0] == '0')
			{
				printf("Dividend can not be 0!\n");
			}
			else if (A[0] == '0')
			{
				printf("0\n");
			}
			else
			{
				div(A, B); printf("%s\n", Quotient);
			}
		}
		else if (FlagA == 1 && FlagB == -1)
		{
			if (A[0] == '0')
			{
				printf("0\n");
			}
			else
			{
				printf("-");
				div(A, B); printf("%s\n", Quotient);
			}
		}
		else if (FlagA == -1 && FlagB == 1)
		{
			if (B[0] == '0')
			{
				printf("Dividend can not be 0!\n");
			}
			else
			{
				printf("-");
				div(A, B); printf("%s\n", Quotient);
			}
		}
		else if (FlagA == -1 && FlagB == -1)
		{
			div(A, B); printf("%s\n", Quotient);
		}
	}
	//Remainder;
	else if (Operator == 5)
	{
		printf("A %% B = ");
		if (FlagA == 1 && A[0] == '0' || Remainder[0] == '0')
		{
			printf("0\n");
		}
		else if (FlagB == 1 && B[0] == '0')
		{
			printf("Dividend can not be 0!\n");
		}
		else if (FlagA == 1)
		{
			printf("%s\n", Remainder);
		}
		else
		{
			printf("-%s\n", Remainder);
		}
	}
	else
	{
		printf("Please input 1-5 to choose the operators!");
	}
		
	system("pause");
	return 0;
}