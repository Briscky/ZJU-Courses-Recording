#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define Length 800
char ArrPrime[Length];						//Prime;
char ArrPrimitive[20];					//Primitive;
char ArrPriA[Length], ArrPriB[Length];		//Private Key of A and B;
char ArrPubA[Length], ArrPubB[Length];		//Public Key of A and B;
char ArrSesA[Length], ArrSesB[Length];		//Session Key of A and B;


//********************
//Large Integers Part
//********************
char A[Length], B[Length], Result[Length * 2];
char MulA[10][Length + 1];
char Quotient[Length], Remainder[Length];

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

	while (Result[0] == '0')			//the first bit have a borrow bit;
	{
		for (i = 0; i < LenA; i++)
		{
			Result[i] = Result[i + 1];
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
	else if (LenA > LenB || LenA == LenB && strcmp(A, B)>0)		//A > B;
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
		while (Quo != 10 && !compare(MulA[Quo], Remainder))	//try the possible quotient until MulA[Quo] > Remainder;
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


//***************
// DH Algorithm
//****************

//Readin Function: read in data from file and store them in array;
void Readin(char *FileName, char *ArrName)
{
	FILE *fp;

	strcpy(ArrName, "");		//initialize array;

	fp = fopen(FileName, "r");
	if (!fp)
	{
		printf("ERROR!");
		return;
	}
	fscanf(fp, "%s", ArrName);	//read in data;
}


//Generate_PrivateKey Function: generate private key for A and B randomly;
void Generate_PrivateKey(char *FileName)
{
	FILE *fp;
	char PrivateKey[300];
	int i;

	srand((unsigned)time(NULL));		//get random based on time;

	fp = fopen(FileName, "w");
	fprintf(fp, "%c", rand() % 9 + '1');	//the first bit of private key can not be 0;
	if (fp != NULL)
	{
		for (i = 0; i < 100; i++)
		{
			fprintf(fp, "%c", rand() % 10 + '0');	//mod 10 to get the random number for each bit;
		}
		fclose(fp);
	}
	else
	{
		printf("ERROR!");
	}

}

//Generate_Key Function: the algorithm of generate public key and session key is same;
//the specific proof of this algorithm is shown in the folder;
void Generate_Key(char *Pri, char *Dividend, char *Key, char *FileName)
{
	char PriBinary[800];	//to store exponent in binary form;
	int i = 0;
	char Binary[2] = "2";	
	FILE *fp;

	//initialize;
	strcpy(PriBinary, "");	
	strcpy(Remainder, "");
	strcpy(Key, "1");

	//get the result in binary form;
	while (Pri[0] != '0')
	{
		div(Pri, Binary);
		PriBinary[i++] = Remainder[0];
		strcpy(Pri, Quotient);
	}
	PriBinary[i] = '\0';

	//get the result of key;
	i--;
	for (i; i >= 0; i--)
	{
		mul(Key, Key);
		strcpy(Key, Result);
		if (PriBinary[i] == '1')
		{
			mul(Key, Dividend);
			strcpy(Key, Result);
		}
		div(Key, ArrPrime);
		strcpy(Key, Remainder);
	}

	//store the result in file;
	fp = fopen(FileName, "w");
	if (fp == NULL)
	{
		printf("ERROR!");
		return;
	}
	fprintf(fp, "%s", Key);

}


int main()
{
	char flag[2] = "1" ;
	char FilePrime[20] = "Prime.txt";
	char FilePrimitive[20] = "Primitive.txt";
	char FilePrivateKey_A[20] = "PrivateKey_A.txt";
	char FilePrivateKey_B[20] = "PrivateKey_B.txt";
	char FilePublicKey_A[20] = "PublicKey_A.txt";
	char FilePublicKey_B[20] = "PublicKey_B.txt";
	char FilSessionKey_A[20] = "SessionKey_A.txt";
	char FilSessionKey_B[20] = "SessionKey_B.txt";

	//generate private key for A and B;
	Generate_PrivateKey(FilePrivateKey_A);
	Generate_PrivateKey(FilePrivateKey_B);

	//read in prime, primitive and private numbers from file to array;
	Readin(FilePrime, ArrPrime);
	Readin(FilePrimitive, ArrPrimitive);

	//generate public key for A and B;
	Readin(FilePrivateKey_A, ArrPriA);
	Readin(FilePrivateKey_B, ArrPriB);
	Generate_Key(ArrPriA, ArrPrimitive, ArrPubA, FilePublicKey_A);
	Generate_Key(ArrPriB, ArrPrimitive, ArrPubB, FilePublicKey_B);

	//generate session key for A and B;
	Readin(FilePrivateKey_A, ArrPriA);
	Readin(FilePrivateKey_B, ArrPriB);
	Generate_Key(ArrPriA, ArrPubB, ArrSesA, FilSessionKey_A);
	Generate_Key(ArrPriB, ArrPubA, ArrSesB, FilSessionKey_B);

	//proof the correctness;
	sub(ArrSesA, ArrSesB);
	strcpy(flag, Result);
	if (strcmp(flag, "0") == 0)
	{
		printf("ArrSesA == ArrSesA\nCorrect!\n");
	}
	else 
	{
		printf("ArrSesA £¡= ArrSesB\nWrong£¡\n");
	}

	system("pause");
	return 0;
}