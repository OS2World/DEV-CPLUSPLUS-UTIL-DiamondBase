/**************************************************************************
 * Source Id :
 *
 * $Id: utils.cc,v 1.2 1993/11/03 12:31:59 davison Exp $
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
 *
 *
 *
 *  Original Author :
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: utils.cc,v $
// Revision 1.2  1993/11/03  12:31:59  davison
// Removed the "extern "c""
//
// Revision 1.1  1993/11/03  10:05:33  kevinl
// Initial revision
//
 **************************************************************************/

#include <string.h>

char* strNcpy(char* d, char* s, int n) 
{
	char* tmp = strncpy(d, s, n);
	d[n-1] = 0;
	return tmp;
}
