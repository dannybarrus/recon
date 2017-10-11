// expressionmatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#pragma warning(disable : 4996)

bool ExpressionMatches(char * szText, char * szPattern);

int main()
{

	char szTest[] = "This is a test string";

	ExpressionMatches(szTest, "T*test*string"); // true
	ExpressionMatches(szTest, "*test*"); // true
	ExpressionMatches(szTest, "*"); // true
	ExpressionMatches(szTest, "*This is *"); // true
	ExpressionMatches(szTest, "A*string"); // false
	ExpressionMatches(szTest, "This is a test string"); // true
	ExpressionMatches(szTest, "This is a test"); // false
	ExpressionMatches(szTest, "This is***"); // true


	return 0;
}

// need default return value
// spec for handling error condition
// handle szText == NULL


bool ExpressionMatches(char * szText, char * szPattern)
{
	bool bRetVal = false;
	char wildcard = '*';
	int nPos = 0;
	char szBuffer[256];

	char * p1 = NULL;
	char * p2 = NULL;

	// Set the search starting point
	p1 = szPattern;

	while (p1)
	{
		szBuffer[0] = '\0';

		p2 = strchr(p1, wildcard);

		// check for wild cards
		// we need to know if a search is in progress
		// if we have found something, bRetVal will be 
		// true.  Otherwise, p2 will not be null and allow the search
		// to continue
		if (p2 || bRetVal)
		{
			// get position of first wildcard
			nPos = p2 - p1 + 1;

			strcpy(szBuffer, p1);
			szBuffer[nPos - 1] = '\0';

			if (strstr(szText, szBuffer))
			{
				bRetVal = true;
				// increase our pointer past the text we have found
				p1 += strlen(szBuffer) + 1;

				// ensure we are in range
				if (p1 >= p2)
				{
					p1 = NULL;
				}

			}
			else
			{
				// we are missing a portion of our string
				// set bRetVal to falue and p1 to exit loop
				bRetVal = false;
				p1 = NULL;
			}

		}
		else
		{
			// no wild cards, simple compare
			if (strcmp(szText, szPattern) == 0)
			{
				bRetVal = true;
			}
			// set condtion to break loop
			p1 = NULL;
		}
	}

	if (bRetVal)
	{
		cout << szPattern << "    found in     " << szText << endl;
	}
	else
	{
		cout << szPattern << "    NOT found in     " << szText << endl;
	}

	return bRetVal;
}