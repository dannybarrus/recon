// fibonacci.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 40

int fib(int num);


int main(void)
{
	int i = 0;

	for (i = 0; i <= 10; i++)
	{
		printf("\n Iteration: %d  Fib = %d\n", i, fib(i));
	}

	return 0;
}

int fib(int num)
/* Fibonacci value of a number */
{
	switch (num) {
	case 0:
		return(0);
		break;
	case 1:
		return(1);
		break;
	default:  /* Including recursive calls */
		return(fib(num - 1) + fib(num - 2));
		break;
	}
}

