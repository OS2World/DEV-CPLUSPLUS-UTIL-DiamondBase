/**************************************************************************
 * Source Id :
 *
 * $Id: nserver.cc,v 1.18 1993/11/04 14:57:06 kevinl Exp $
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
 * $Log: nserver.cc,v $
// Revision 1.18  1993/11/04  14:57:06  kevinl
// Added allowAll and isOpen and took care of unnamed relations "" being
// added to the array
//
// Revision 1.17  1993/10/18  11:21:08  kevinl
// Allow use of "" as a database config file
//
// Revision 1.16  1993/07/11  08:46:18  kevinl
// Minor compile time thingos
//
// Revision 1.15  1993/07/11  08:19:14  kevinl
// Stopped NameServer exiting when config file is empty/missing
//
// Revision 1.14  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.13  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.12  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.11  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.10  1993/05/03  01:33:03  kevinl
// Cosmetic (mainly) changes for CC
//
// Revision 1.9  1993/04/27  07:03:31  kevinl
// Comments
//
// Revision 1.8  1993/04/15  04:21:52  kevinl
// Moved malloc.h
//
// Revision 1.7  1993/04/07  08:29:46  kevinl
// NameServer now displays correct dbc filename when it's not found
//
// Revision 1.6  1993/03/29  13:09:23  kevinl
// Now outputs requested relation name not array pointer
//
// Revision 1.5  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.4  1993/03/25  22:21:12  davison
// queryName now takes a const char*.
//
// Revision 1.3  1993/03/20  15:11:47  davison
// Now returns a name ID, so that only one dbobject for each relation is
// available at any time.
//
// Revision 1.2  1993/02/22  05:39:54  davison
// Fixed RCS Header.
//
 **************************************************************************/

#include <stdlib.h>
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif
#include <nserver.h>
#include <iostream.h>
#include <fstream.h>

char* TNameServer::verStr(void)
{
	return "$Id: nserver.cc,v 1.18 1993/11/04 14:57:06 kevinl Exp $";
}

TNameServer::TNameServer(char* name)
{
	nsOpen = false;
	char dbcName[200];
	if (name)
		strcpy(dbcName,name);
	else
		strcpy(dbcName,"ns.dbc"); // Default: (n)ame(s)erver.(d)ata(b)ase (c)onfig

	allowAll=false;
	if (!strlen(dbcName)) {
		NumNames=0;
		allowAll = true;
		nsOpen = true;
		return;
	}

	ifstream is(dbcName);

	if (!is.good()) {
		dbErr(db_nfound, "database configuration file");
		NumNames = 0;
		return;
	}

	// File is of the form:
	// relation=path
	// .
	// .
	for(int i=0;is.good()&&i<MAX_RELATIONS;i++) {
		is.getline(RelName[i],40,'=');
		is.getline(PathName[i],200,'\n');
	}
	NumNames = i;
	nsOpen = true;
}

// Fetch a path for a particular relation database.

char* TNameServer::queryName(const char *name, long& nameId)
{
	int i;
	char* data=0;

	for(i=0;i<NumNames;i++) 
	{
		if (!strcmp(RelName[i],name)) 
		{
			data = new char[strlen(PathName[i])+1];
			strcpy(data,PathName[i]);
			nameId = (long)i;
			break;
		}
	}
	if (!data) {
		if (allowAll)
		{
			strcpy(RelName[i=NumNames],name);
			strcpy(PathName[NumNames],".");
			NumNames++;
			data = new char[strlen(PathName[i])+1];
			strcpy(data,PathName[i]);
			nameId = (long)i;
		}
		else
		{
			cerr << "Relation name server lookup failed for '" << name <<"'" << endl;
			exit(1);
		}
	}
	return data;
}
