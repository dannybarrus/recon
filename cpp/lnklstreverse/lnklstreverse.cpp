// lnklstreverse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 40

struct Phone
{
	char *pName;
	char *pNumber;
	struct Phone *pNext;
};


struct Phone *CreateList(void);
void DisplayList(struct Phone *pHead);
void DeleteList(struct Phone *pHead);
struct Phone *GetPhone(void);
void ReverseAndDisplayList(struct Phone ** pList);

int main(void)
{
	struct Phone *pHead;

	printf("\n Enter the names and telephone numbers as prompted, each less \n"
		"  than 40 characters.  Enter an empty line to end input.");

	pHead = CreateList();
	DisplayList(pHead);
	ReverseAndDisplayList(&pHead);
	DeleteList(pHead);
	return 0;

}


struct Phone *CreateList(void)
{
	struct Phone *pHead = NULL;
	struct Phone *pCurrent = NULL;

	pHead = pCurrent = GetPhone();

	while (pCurrent != NULL)
	{
		pCurrent->pNext = GetPhone();
		pCurrent = pCurrent->pNext;
	}

	return pHead;
}

void DisplayList(struct Phone *pList)
{
	while (pList != NULL)
	{
		printf("\n%-40s\t%s", pList->pName, pList->pNumber);
		pList = pList->pNext;
	}
}


void ReverseAndDisplayList(struct Phone ** pHead)
{
	struct Phone * pCurr = *pHead;
	struct Phone * pNext = NULL;
	struct Phone * pPrev = NULL;

	while (pCurr)
	{
		// set the head to the current node
		*pHead = pCurr;
		// save the next ptr in pNext
		pNext = pCurr->pNext;
		// change the next to previous
		pCurr->pNext = pPrev;
		pPrev = pCurr;
		pCurr = pNext;
	}

	printf("\n\n\n--Display Reverse Order--\n\n");
	DisplayList(*pHead);
	printf("\n\n");

}

struct Phone *GetPhone(void)
{
	struct Phone *pPhone = NULL;
	char Buffer[MAXLEN];
	size_t Length = 0;

	printf("\nEnter a name: \n");
	gets_s(Buffer,sizeof(Buffer));
	Length = strlen(Buffer);
	if (!Length)
	{
		return (NULL);
	}

	pPhone = (struct Phone *) malloc(sizeof(struct Phone));

	if (!pPhone)
	{
		exit(2);
	}

	pPhone->pName = (char *)malloc(Length + 1);

	if (!pPhone->pName)
	{
		exit(2);
	}

	strcpy_s(pPhone->pName, strlen(pPhone->pName), Buffer);


	printf("\nEnter a number for %s: \n", Buffer);
	gets_s(Buffer,sizeof(Buffer));

	pPhone->pNumber = (char *)malloc(strlen(Buffer) + 1);

	if (!pPhone->pNumber)
	{
		exit(2);
	}

	strcpy_s(pPhone->pNumber, strlen(pPhone->pNumber), Buffer);

	pPhone->pNext = NULL;


	return pPhone;
}


void DeleteList(struct Phone *pHead)
{
	struct Phone *pTemp = NULL;


	while (pHead != NULL)
	{
		free(pHead->pName);
		free(pHead->pNumber);
		pTemp = pHead;
		pHead = pHead->pNext;
		free(pTemp);
	}

	return;
}



