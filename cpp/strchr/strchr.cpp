// strchr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

char * mystrchr(const char *s, int c);

int main()
{
	char szTest[] = "This is a test";

	int findMe = 't';
	
	char found = *mystrchr(szTest, findMe);
		
	return 0;
}

char * mystrchr(const char *s, int c)
{

	while (*s++)
	{
		if (*s == c)
		{
			return (char*)s;
		}
	}

	return (NULL);
}
