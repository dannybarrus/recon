
/*******************************************************************
*
*    DESCRIPTION: A simple program to demonstrate reversing pairs
*                 in a singly linked list
*
*    AUTHOR:	   Danny E.C. Barrus (dbarrus@msn.com)
*
*    HISTORY:     Who        When             What
*               -----------------------------------------
*                 db         08 Apr 2005      Created
*
*******************************************************************/

/** include files **/
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#pragma warning( disable : 4996) 

/** data **/
struct Phone
{
	char *pName;
	char *pNumber;
	struct Phone *pNext;
};

/** functions **/
void DisplayList(struct Phone *pHead);
void DeleteList(struct Phone *pHead);
struct Phone * InsertPhone(char* pszName, char* pszNumber);
struct Phone * AutoCreateList(int nEntries);
struct Phone * ReversePairs(struct Phone ** pList);

/*
** main
*
*  FILENAME: D:\projects\interviewalgorithims\linkedlist\main.c
*
*  PARAMETERS: None used.
*
*  DESCRIPTION: Main entry point for application, standard main function
*
*  RETURNS: int -- Not used, always returns 0.
*
*/
int main(void)
{
	struct Phone *pHead;
	int nEntries = 7;

	printf("\n Auto Creating list with %d entries \n", nEntries);

	pHead = AutoCreateList(nEntries);

	printf("\n\n\n--Display Original List--\n\n");
	DisplayList(pHead);

	ReversePairs(&pHead);

	printf("\n\n\n--Display Reversed Pairs--\n\n");
	DisplayList(pHead);
	printf("\n\n");

	DeleteList(pHead);

	return 0;

}

/*
** DisplayList
*
* struct Phone * CreateList(void);
*
*  PARAMETERS: struct Phone * -- pointer to head of linked list to display
*
*  DESCRIPTION: Prints formatted output for linked liste
*
*  RETURNS: none
*
*/
void DisplayList(struct Phone *pList)
{
	while (pList != NULL)
	{
		printf("\n%-40s\t%s", pList->pName, pList->pNumber);
		pList = pList->pNext;
	}
	printf("\n\n");
}

/*
** DeleteList
*
*  FILENAME: \main.c
*
*  PARAMETERS: struct Phone * pHead -- pointer to head of list to be deleted
*
*  DESCRIPTION: Frees memory used by a linked list by deleting itx members and finally
*  the structure from memory
*
*  RETURNS:
*
*/
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

/*
** InsertPhone
*
*  struct Phone * CreateList(void);
*
*  PARAMETERS:	char* pszName, char* pszNumber -- name and number to be
*              used in the Phone data structure
*
*  DESCRIPTION: Creates a node in a linked list
*
*  RETURNS: struct * Phone -- a pointer to the head of the linked list
*
*/
struct Phone * InsertPhone(char* pszName, char* pszNumber)
{
	struct Phone *pPhone = NULL;
	int nLength;

	nLength = strlen(pszName);

	if (!nLength)
	{
		return (NULL);
	}

	pPhone = (struct Phone *) malloc(sizeof(struct Phone));

	if (!pPhone)
	{
		exit(2);
	}

	pPhone->pName = (char *)malloc(nLength + 1);

	if (!pPhone->pName)
	{
		exit(2);
	}

	strcpy(pPhone->pName, pszName);

	pPhone->pNumber = (char *)malloc(strlen(pszNumber) + 1);

	if (!pPhone->pNumber)
	{
		exit(2);
	}

	strcpy(pPhone->pNumber, pszNumber);

	pPhone->pNext = NULL;


	return pPhone;
}

/*
** AutoCreateList
*
*  FILENAME: D:\projects\interviewalgorithims\linkedlist2\main.c
*
*  PARAMETERS: int nEntries -- number of entries to create
*
*  DESCRIPTION: Automatically creates a linked list
*
*  RETURNS: struct Phone * -- Pointer to head of newly created linked list
*
*/
struct Phone *AutoCreateList(int nEntries)
{
	struct Phone *pHead = NULL;
	struct Phone *pCurrent = NULL;
	char szNameBuff[128];
	char szNumBuff[128];
	int i = 0;

	if (nEntries > 0)
	{
		pHead = pCurrent = InsertPhone("1 Test", "1238-92103-1");

		for (i = 2; i <= nEntries; i++)
		{
			sprintf(szNameBuff, "%d Test", i);
			sprintf(szNumBuff, "1238-92103-%d", i);

			pCurrent->pNext = InsertPhone(szNameBuff, szNumBuff);
			pCurrent = pCurrent->pNext;
		}

		pCurrent->pNext = NULL;
	}

	return pHead;
}

/*
** ReversePairs
*
*  PARAMETERS: struct Phone ** pList -- Pointer to a pointer to list head
*              NOTE:  In the coding problem, you asked for a pointer to a linked list.
*              However, I think we need a pointer to a pointer to a linked list.
*              We will be modifying the list when we reverse the
*              pairs, so we should modify the head.  In order to do this,
*              we need a pointer to a pointer.  If we do not do this, we will
*              lose the head of the list.  I.e. Given a list with items 1,2,3,4,
*              our function will display our function will return a head that points to
*              items 2,1,4,3 as expected.  But other functions will track the original head
*              and display 1,4,3 -- dropping #2.  Unless, of course, you modify the head.
*              This will be of particular interest when deleting the list
*
*  DESCRIPTION: Takes a list with items 1,2,3,4 and reverses the pairs, 2,1,4,3
*
*  RETURNS: struct Phone * -- pointer to head of newly modified structure
*
*/
struct Phone * ReversePairs(struct Phone ** pList)
{
	struct Phone * pCurr = NULL;
	struct Phone * pPrev = NULL;
	struct Phone * pNext = NULL;
	struct Phone * pNextIter = NULL;

	// set current postion at head
	pCurr = *pList;

	// This is only worth doing if we have a current and next pointers
	if ((pCurr) && (pCurr->pNext))
	{
		// init pNext
		pNext = pCurr->pNext;

		// set new head position
		*pList = pCurr->pNext;

		// since we are reversing pairs, we need to watch
		//  both pCurr and pNext
		while ((pCurr != NULL) && (pNext != NULL))
		{
			// set our next start point before modifying data
			pNextIter = pCurr->pNext->pNext;

			// start the reversal process
			//  set our pNext pointer back to the start of the list
			pNext->pNext = pCurr;

			//  set our pCurr pointer to point to the new next node
			pCurr->pNext = pNextIter;


			// if pPrev has been set, then we need to create that link to the newly
			// reversed pointer
			if (pPrev)
			{
				pPrev->pNext = pNext;
			}

			// advance our pointers for next iteration
			pPrev = pCurr;
			pCurr = pNextIter;
			// pCurr could be NULL and accessing the pNext elem.
			// could cause an exception
			if (pCurr)
			{
				pNext = pCurr->pNext;
			}
		}
	}

	return *pList;

}


