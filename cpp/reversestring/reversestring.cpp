// reversestring.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

char * ReverseString(char * s1);

int main()
{
	char szTest[] = "This is a test";
	
	char * szResult = ReverseString(szTest);

	printf(szResult);

	return 0;
}

char * ReverseString(char * s1)
{
	char temp = '\0';
	char * p1 = NULL;
	char * p2 = NULL;


	p1 = p2 = s1;

	while (*p2 != '\0')
	{
		p2++;
	}

	// remove null term
	p2--;


	while (p1 < p2)
	{
		temp = *p2;
		*p2 = *p1;
		*p1 = temp;
		p2--;
		p1++;
	}

	return s1;
}
