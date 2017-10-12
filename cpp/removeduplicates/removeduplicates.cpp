// removeduplicates.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int remove_duplicates(int * p, int size);

int main()
{
	const int MAXSIZE = 14;
	int nArray[] = { 1,2,2,3,4,5,5,6,7,8,8,8,9,0 };
	int * p = nArray;
	int lastElement = remove_duplicates(p, MAXSIZE);

	int newArray[MAXSIZE];
	memcpy(newArray, nArray, lastElement * sizeof(int));
	
	return 0;
}

int remove_duplicates(int * p, int size)
{
	int current = 1, insert = 1;

	while (current < size)
	{
		if (p[current] != p[insert - 1])
		{
			p[insert] = p[current];
			current++;
			insert++;
		}
		else
		{
			current++;
		}
	}

	return insert;
}

