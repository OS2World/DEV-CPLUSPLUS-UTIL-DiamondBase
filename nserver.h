/**************************************************************************
 * Source Id :
 *
 * $Id: nserver.h,v 1.9 1993/11/04 14:57:06 kevinl Exp $
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
 *    Name Server Class : This will return the path leading to the database
 *   files related a relation by its name.
 *
 *  Original Author :
 *		Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: nserver.h,v $
 * Revision 1.9  1993/11/04  14:57:06  kevinl
 * Added allowAll and isOpen and took care of unnamed relations "" being
 * added to the array
 *
 * Revision 1.8  1993/07/11  08:46:18  kevinl
 * Minor compile time thingos
 *
 * Revision 1.7  1993/07/11  08:19:19  kevinl
 * Stopped NameServer exiting when config file is empty/missing
 *
 * Revision 1.6  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.5  1993/04/27  07:03:31  kevinl
 * Comments
 *
 * Revision 1.4  1993/03/25  22:21:42  davison
 * queryName now takes a const char*
 *
 * Revision 1.3  1993/03/20  15:12:56  davison
 * Now returns a name ID, so that only one dbobject for each relation is
 * available at any time.
 *
 * Revision 1.2  1993/02/22  05:40:05  davison
 * Fixed RCS Header.
 *
 **************************************************************************/

#ifndef __Name_Server_h__
#define __Name_Server_h__

#include <ctype.h>
#include <string.h>
#include <defs.h>

#define MAX_RELATIONS 20

class TNameServer
{
	char	PathName[MAX_RELATIONS][200];
	char	RelName[MAX_RELATIONS][40];
	int		NumNames;
	bool	allowAll;
protected:
	bool	nsOpen;
public:
			TNameServer(char* name=0);
	char*	queryName(const char*, long& nameId);
	char*	verStr(void);
	bool	ok(void) { return nsOpen; }
};

#endif
