//ktbueluegvitnthuexmonveggmrcgxptlyhhjaogchoemqchpdnetxupbqntietiabpsmaoncnwvoutiugtagmmqsxtvxaoniiogtagmbpsmtuvvihpstpdvcrxhokvhxotawswquunewcgxptlcrxtevtubvewcnwwsxfsnptswtagakvoyyak

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

using namespace std;

int main()
{
	string str;
	cin >> str;

	int strlen;
	strlen = str.length();
	cout << strlen << endl;

	char ch1[200];			//string of the original ciphertext;
	strcpy_s(ch1, str.c_str());		//turn the string to char[];
	
	//Kasiski examination;
	int i, j;
	char ch2[200];			//store the char[] after moving;
	int count[26];			//store the number of the same alphabets of the original char[] after moving;
	for (j = 0; j < 26; j++)
	{	
		count[j] = 0;
		for (i = 0; i < strlen; i++)
		{
			ch2[i] = ch1[(i + j) % strlen];
			if (ch1[i] == ch2[i])
			{
				count[j]++;
			}
		}
		cout << j << "\t" << count[j] << endl;
	}

	//Frequency Analysis;
	int count2[3][26];
	int temp;
	for (i = 0; i < 3; i++)			//Initialize;
	{
		for (j = 0; j < 26; j++)
		{
			count2[i][j] = 0;
		}
	}
	for (i = 0; i < strlen; i++)	//frequency sum;
	{
		count2[i%3][ch1[i]- 'a']++;
	}
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 26; j++)
		{
			cout << char('a'+j) << "\t" << count2[i][j] << "\t";
		}
		cout << endl;
	}

	cout << "Key: cat" << endl;

	for (i = 0; i < strlen; i+=3)		//decode;
	{
		ch1[i] = char((ch1[i] + 26 - 'c') % 26 + 'a');
		ch1[i+2] = char((ch1[i+2] + 26 - 't') % 26 + 'a');
	}

	for (i = 0; i < strlen; i++)	//output the plaintext;
	{
		cout << ch1[i];
	}

	system("pause");
}