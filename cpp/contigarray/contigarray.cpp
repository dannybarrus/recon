// contigarray.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

int CountElemStep(int * p, int size, int step);
int CountContigElem(int * p, int size);

int main()
{

	int array1[] = { -3,-2,-1,0,1,2,3,4,5,6,7,8 };
	int array2[] = { 3,2,1,0,-1,-2,-3,-4,-5,-6,-7,-8 };
	int array3[] = { 1,2,3,4,5,-2,-3,-4,-5,-6,-7,-8 };

	int * p = array1;
	CountContigElem(p, 12);
	printf("Ascending ReturnValue = %d\n\n", CountContigElem(p, 12));

	p = array2;
	CountContigElem(p, 12);
	printf("Descending ReturnValue = %d\n\n", CountContigElem(p, 12));

	p = array3;
	CountContigElem(p, 12);
	printf("Descending/Ascending mix ReturnValue = %d\n\n", CountContigElem(p, 12));


	return 0;

}


int CountContigElem(int * p, int size)
{
	int temp;
	int nCount;

	// check for stepping up
	nCount = CountElemStep(p, size, 1);
	// check for stepping down
	temp = CountElemStep(p, size, -1);

	// if we found more descending,
	// return that number
	if (nCount < temp)
	{
		nCount = temp;
	}

	return(nCount);
}


int CountElemStep(int * p, int size, int step)
{
	int nCount = 0;
	int nIndex = 0;
	int temp = 1; // init at 1 as we will need to count the first element

	while (nIndex < size)
	{
		if (p[nIndex] + step == p[nIndex + 1])
		{
			temp++;
		}
		else
		{
			if (temp > nCount)
			{
				nCount = temp;
			}
			temp = 1;
		}
		nIndex++;
	}

	return(nCount);
}

