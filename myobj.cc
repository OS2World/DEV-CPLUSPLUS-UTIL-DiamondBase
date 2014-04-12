/**************************************************************************
 * Source Id :
 *
 * $Id: myobj.cc,v 1.9 1993/05/26 01:01:39 kevinl Exp $
 *-------------------------------------------------------------------------
 * Project Notes :
 *
 *  Diamond Base
 *  ============
 *      A solid database implementation, spurred on by the continuing
 *  Metal (Lead) Base saga.
 *
 *  Project Team :
 *        A. Davison
 *        K. Lentin
 *        D. Platt
 *
 *	Project Commenced : 05-02-1993
 *
 *-------------------------------------------------------------------------
 *  Module Notes :
 *
 *	My test object class.
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: myobj.cc,v $
// Revision 1.9  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.8  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.7  1993/03/20  14:12:16  kevinl
// Added a char* cast for sdcanf and some {} in switch
//
// Revision 1.6  1993/03/18  04:38:28  kevinl
// Simplified long comparison in compare()
//
// Revision 1.5  1993/03/17  14:33:10  kevinl
// Added in second key.
// Rewrote compare, getKey, getKeyLength
// Removed isEqual and isLessThan
//
// Revision 1.4  1993/02/18  22:28:30  davison
// Added numKey and default return values
//
// Revision 1.3  1993/02/17  12:16:43  kevinl
// Added isEqual
// fixed setKey
//
// Revision 1.2  1993/02/13  00:56:44  kevinl
// added setKey and new constructor
//
// Revision 1.1  1993/02/08  14:41:47  darrenp
// Initial revision
//
 **************************************************************************/

#include "myobj.h"
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

myObject::myObject() : object()
{
	theKey = rand();
	strcpy(theData, "HelloWorld");
}

myObject::myObject(pKeyType newKey, long numKey) : object()
{
	setKey(newKey, numKey);
}

void myObject::setKey(pKeyType newKey, long keyNum)
{
	switch (keyNum)
	{
		case longKey:
			bcopy(newKey, &theKey, sizeof(theKey));
			break;
		case charKey:
			sscanf((char*)newKey, "%x", &theKey);
			break;
	}
}

long myObject::getKeyLength(long keyNum)
{
	switch (keyNum)
	{
		case longKey:
			return sizeof(theKey);
			break;
		case charKey:
			return 10;
			break;
		default:
			cerr << "getKeyLength AAGGHH" << endl;
			exit (-1);
//			return -1;
	}
	return -1;
}

pKeyType myObject::getKey(long keyNum)
{

	switch (keyNum)
	{
		case longKey:
		{
			long* copy = new long(theKey);
			return (pKeyType)(copy);
		}
		case charKey:
		{
			char* key = new char[10];
			sprintf(key, "%lx", theKey);
			return (pKeyType)(key);
		}
		default:
			cerr << "getKey AAGGHH" << endl;
			exit (-1);
//			return (pKeyType)0;
	}
	return (pKeyType)0;
}

int myObject::compare(pKeyType key, long keyNum)
{
	char* myKey = (char*)getKey(keyNum);
	int res;

	switch (keyNum)
	{
		case longKey:
			res = (theKey - *(long*)key);
			break;
		case charKey:
			res = strcmp(myKey, (char*)key);
			break;
		default:
			cerr << "compare AAGGHH" << endl;
			exit (-1);
	}

	delete myKey;
	return res;
}
