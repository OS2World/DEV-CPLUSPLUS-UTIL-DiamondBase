/**************************************************************************
 * Source Id :
 *
 * $Id: btdump.cc,v 1.4 1993/05/26 01:01:39 kevinl Exp $
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
 *  Dump a btree
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: btdump.cc,v $
// Revision 1.4  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.3  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.2  1993/04/08  15:14:02  davison
// included stdlib.h
//
// Revision 1.1  1993/04/05  00:00:53  kevinl
// Initial revision
//
 **************************************************************************/

#include <iostream.h>
#include "btree.h"
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif
#include <stdlib.h>

int
main(int argc, char **argv)
{
	if (argc != 2)
	{
		cerr << "Syntax: " << argv[0] << " filename" << endl;
		exit (1);
	}

	bTree bt(argv[1], 0);
	bt.dump();
}
