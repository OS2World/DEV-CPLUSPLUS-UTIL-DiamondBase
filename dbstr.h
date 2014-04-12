/**************************************************************************
 * Source Id :
 *
 * $Id: dbstr.h,v 1.4 1993/09/26 06:40:32 kevinl Exp $
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
 *  dbstr.h: common types etc needed for variable length string support
 *
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dbstr.h,v $
 * Revision 1.4  1993/09/26  06:40:32  kevinl
 * Added dbData support
 *
 * Revision 1.3  1993/07/11  09:42:05  kevinl
 * Changed String to dbString
 *
 * Revision 1.2  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.1  1993/06/20  13:49:57  kevinl
 * Initial revision
 *
 **************************************************************************/

#ifndef _DBSTR_H
#define _DBSTR_H

#include <mystring.h>

// This struct is used in a relation and dbobj to find the relevant
// Strings and longs. 

struct _db_strStruct
{
	long*		theLong;
	dbData*		theString;
	_db_strStruct(void) {}
	_db_strStruct(long* tL, dbData* tS) { theLong = tL; theString = tS; }
};

#endif
