// comparestr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int CompStr(char * s1, char * s2);

int main()
{
	int nRes;
    
	nRes = CompStr("Test1 this does not match", "Test1 tnope it sure does not");
	nRes = CompStr("Test1", "Test1");

	return 0;
}

int CompStr(char * s1, char * s2)
{
	while ((*s1 != '\0') && (*s1 == *s2))
	{
		*s1++;
		*s2++;
	}

	return (*s1 - *s2);
}