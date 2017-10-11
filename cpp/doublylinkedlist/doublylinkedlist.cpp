// doublylinkedlist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXLEN 40

#pragma warning(disable:4996)

struct Phone
{
	char *pName;
	char *pNumber;
	struct Phone *pNext;
	struct Phone *pPrev;
};


struct Phone *CreateList(void);
void DisplayList(struct Phone *pHead);
void DeleteList(struct Phone *pHead);
struct Phone *GetPhone(void);

int main(void)
{
	struct Phone *pHead;

	printf("\n Enter the names and telephone numbers as prompted, each less \n"
		"  than 40 characters.  Enter an empty line to end input.");

	pHead = CreateList();
	DisplayList(pHead);
	DeleteList(pHead);
	return 0;

}


struct Phone *CreateList(void)
{
	struct Phone *pHead = NULL;
	struct Phone *pCurrent = NULL;
	struct Phone *pInsert = NULL;
	struct Phone *pPrev = NULL;

	// Get first record
	pHead = pCurrent = GetPhone();

	// get the rest
	while ((pCurrent = GetPhone()) != NULL)
	{
		// start at top
		pInsert = pHead;
		while (pInsert != NULL)
		{
			if (strcmp(pCurrent->pName, pInsert->pName) < 0)
			{

				// change the head, if beginning
				if (pInsert->pPrev == NULL)
				{
					pHead = pCurrent;
				}
				else
				{
					// else reset pNext for previous
					pPrev->pNext = pCurrent;
				}

				pInsert->pPrev = pCurrent;
				pCurrent->pNext = pInsert;
				pCurrent->pPrev = pPrev;

				break;
			}
			else
			{
				// increase pointer to next object
				pPrev = pInsert;
				pInsert = pInsert->pNext;
			}
		}

		if (pCurrent->pNext == NULL)
		{
			// current was not inserted, so add it to the end
			pCurrent->pPrev = pPrev;
			pPrev->pNext = pCurrent;
		}

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

	strcpy(pPhone->pName, Buffer);


	printf("\nEnter a number for %s: \n", Buffer);
	gets_s(Buffer,sizeof(Buffer));

	pPhone->pNumber = (char *)malloc(strlen(Buffer) + 1);

	if (!pPhone->pNumber)
	{
		exit(2);
	}

	strcpy(pPhone->pNumber, Buffer);

	pPhone->pNext = NULL;
	pPhone->pPrev = NULL;


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
