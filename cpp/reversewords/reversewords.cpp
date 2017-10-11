// reversewords.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include <string>


void ReverseWords(char s1[]);
void ReverseString(char str[], int start, int end);

int main()
{
	char szTest[] = "This is a test";

	ReverseWords(szTest);

	return 0;
}

void ReverseWords(char str[])
{

	int start = 0, end = 0, length;

	length = strlen(str);

	// reverse string
	ReverseString(str, start, length - 1);

	while (end < length)
	{
		if (str[end] != ' ')
		{
			start = end;

			while ((end < length) && (str[end] != ' '))
			{
				end++;
			}
			end--;

			ReverseString(str, start, end);
		}
		end++;

	}
	return;
}

void ReverseString(char str[], int start, int end)
{
	char temp;
	while (end > start)
	{
		temp = str[start];
		str[start] = str[end];
		str[end] = temp;

		start++;
		end--;
	}
	return;
}