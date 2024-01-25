#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

int main()
{
	string str;
	cin >> str;
	
	char ch[100];
	strcpy_s(ch, str.c_str());

	int str_len;
	str_len = str.length();

	int i,j;
	for (i = 0; i < 26; i++) {
		cout << i << "\t";
		for (j = 0; j < str_len; j++)
		{
			cout << (char)((ch[j] - 'A' + i) % 26 + 'A');
		}
		cout << endl;
	}

	string str2;
	cin >> str2;

	int str_len2;
	str_len2 = str2.length();

	char ch2[100];
	strcpy_s(ch2, str2.c_str());

	for (j = 0; j < str_len2; j++)
	{
		cout << (char((ch2[j] - 'A' + 15) % 26 + 'A'));
	}
	cout << endl;

	system("pause");
}