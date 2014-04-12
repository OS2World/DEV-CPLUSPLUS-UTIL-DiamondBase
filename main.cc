/**************************************************************************
 * Source Id :
 *
 * $Id: main.cc,v 1.3 1993/05/26 01:01:39 kevinl Exp $
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
 *  An initial main to do some testing with.
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: main.cc,v $
// Revision 1.3  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.2  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.1  1993/02/10  07:57:31  darrenp
// Initial revision
//
 **************************************************************************/

#include "myobj.h"
#include <iostream.h>
#ifdef BORLANDC
#include <stdlib.h>
#include <time.h>
#endif

#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

int
main()
{
#ifdef BORLANDC
	randomize();
#else
	srand(getpid() * getppid());
#endif

	myObject obj;
	bucket b(1, obj.getKeyLength(), 512, INNER, 0, 0, 0);
	b.info();
	for (int i = 0; i < 20; i++)
	{
		myObject insObj;
		b.insert(insObj, i);
	}
	b.info();
	b.dump();

	bucket b1(2, obj.getKeyLength(), 512, INNER, 0, b.getId(), 0);
	b.header()->nextBucket = b1.getId();

	pKeyType outKey = b.allocTmpKey();
	myObject insObj;
	b.sploosh(&b1, 0, outKey, insObj, 99);

	cout << "Promote " << hex << *(long*)outKey << endl;

	cout << "Original Bucket:" << endl;

	b.info();
	b.dump();

	cout << endl << "Splooshed Bucket:" << endl;
	b1.info();
	b1.dump();
	return 0;
}
