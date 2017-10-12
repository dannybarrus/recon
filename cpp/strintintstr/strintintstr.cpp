// strintintstr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void IntToStr(int num, char str[]);
int StrToInt(char str[]);

#define MAX_DIGITS_INT 10

int main()
{
	int nValue = 14352;
	char szBuf[256];

	IntToStr(nValue, szBuf);
	int nVerify = StrToInt(szBuf);

	if (nVerify != nValue)
	{
		printf("Conversion failed!!!");
	}
	else
	{
		printf("Conversion success!!!");
	}
	
	return 0;
}

void IntToStr(int num, char str[])
{
	int i = 0, j = 0, isNeg = 0;

	char temp[MAX_DIGITS_INT + 2];

	if (num < 0)
	{
		num *= -1;
		isNeg = 1;
	}

	do
	{
		temp[i++] = (num % 10) + '0';
		num /= 10;
	} while (num);

	if (isNeg)
	{
		str[j++] = '-';
	}

	while (i > 0)
	{
		str[j++] = temp[--i];
	}

	str[j] = '\0';

	return;
}

int StrToInt(char str[])
{

	int i = 0, isNeg = 0, num = 0;

	if (str[0] == '-')
	{
		isNeg = 1;
		i++;
	}

	while (str[i])
	{
		num *= 10;
		num += ((char)str[i++] - '0');
	}

	if (isNeg)
	{
		num *= -1;
	}

	return num;
}