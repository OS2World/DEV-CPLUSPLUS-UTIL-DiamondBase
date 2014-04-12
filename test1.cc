/**************************************************************************
 * Source Id :
 *
 * $Id: test1.cc,v 1.4 1993/05/26 01:01:39 kevinl Exp $
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
 *  Testing rserv and bucket reading
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: test1.cc,v $
// Revision 1.4  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.3  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.2  1993/03/26  06:16:38  darrenp
// standardized error codes.
//
// Revision 1.1  1993/02/11  07:45:59  darrenp
// Initial revision
//
 **************************************************************************/

#include "myobj.h"
#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef BORLANDC
#include <time.h>
#endif
#include "rserv.h"
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
	recServer rs("testdb");
	cout << "Server opened" << endl;
/*	if (rs.createDb("testdb", 512) != recServer::db_ok)
		exit(0);

	bucket b(1, obj.getKeyLength(), 512, INNER, 0, 0, 0);
	b.info();
	for (int i = 0; i < 20; i++)
	{
		myObject insObj;
		b.insert(insObj, i);
	}
	b.info();
	b.dump();

	long idx;
	cout << "newRec" << endl;
	if (rs.newRec(idx) != recServer::db_ok)
		exit (0);

	cout << "idx=" << idx << endl;
	cout << "putRec" << endl;
*/
	bucket b(1, obj.getKeyLength(), 512, INNER, 0, 0, 0);
	cout << "getRec" << endl;
	if (rs.getRec(0, b.getData()) != recServer::db_ok)
		exit (0);

	cout << "Success" << endl;

	b.info();
	b.dump();
	return 0;
}
