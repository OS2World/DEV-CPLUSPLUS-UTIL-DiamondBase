/*************************************************************************
 * Source Id :
 *
 * $Id: dberr.cc,v 1.14 1993/11/05 13:46:57 kevinl Exp $
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
 *	dbErr : A global error handler.
 *
 *  Original Author : Daz / Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dberr.cc,v $
// Revision 1.14  1993/11/05  13:46:57  kevinl
// Protocol and fixes
//
// Revision 1.13  1993/11/03  13:48:27  kevinl
// Fixed something little
//
// Revision 1.12  1993/11/03  10:45:26  kevinl
// Needs string.h
//
// Revision 1.11  1993/08/05  11:52:08  darrenp
// added db_refid error for MV.
//
// Revision 1.10  1993/07/05  05:36:19  darrenp
// Added db_nomem and db_comms for the multi version.
//
// Revision 1.9  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.8  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.7  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.6  1993/05/03  01:34:07  kevinl
// 2 cases left out of switch
//
// Revision 1.5  1993/04/25  11:21:28  kevinl
// Comments
//
// Revision 1.4  1993/03/30  14:37:48  kevinl
// Modified error handling again. now we can get a string back (if we want)
//
// Revision 1.3  1993/03/29  23:26:47  kevinl
// Support for diaRel's error handling
//
// Revision 1.2  1993/03/29  08:21:19  darrenp
// Added malloc library support
//
// Revision 1.1  1993/03/28  10:39:20  kevinl
// Initial revision
//
 **************************************************************************/

#include <iostream.h>
#include <stdio.h>
#include <string.h>

#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif
#include <dberr.h>

// This is the status returned by the last dbErr call
// extraErr can be optionally added and is cleared by subsequent calls

dbError dbErrorStatus = db_ok;
char extraErr[50] = "";

//----------------------------------------
// Register an error by remembering the
// error code. Then return the error 
// code.

dbError dbErr(dbError err, char* extra)
{
	if (extra)
		strcpy(extraErr, extra);
	else
		extraErr[0] = 0;
	dbErrorStatus = err;
	return err;
}

// Display a particular error message. temp is used to reinstate the
// the last error code.

void dbErr(char* msg, dbError err)
{
	dbError temp = dbErrorStatus;
	dbErrorStatus = err;
	dbErr(msg);
	dbErrorStatus = temp;
}

//--------------------------------------------
// Display the current error status, preceeded
// by an optional error message.

void dbErr(char* msg)
{
	if (msg)
		cerr << msg << " : " ;
	else
		cerr << "Error : " << endl;

	if (*extraErr)
		cerr << extraErr << " : ";

	cerr << dbErrStr(dbErrorStatus) << endl;
}

// Return the string representation of err

char* dbErrStr(dbError err)
{
	static char errStr[100];

	switch (err)
	{
		case db_ok : 
			strcpy(errStr,"No error.");
			break;
		case db_alreadyregistered:
			strcpy(errStr,"Relation already registered.");
			break;
		case db_unimp:
			strcpy(errStr,"Not yet implemented.");
			break;
		case db_nfound:
			strcpy(errStr,"Not Found.");
			break;
		case db_range:
			strcpy(errStr,"Range Error.");
			break;	
		case db_toomany:
			strcpy(errStr,"Too many.");
			break;
		case db_noquery:
			strcpy(errStr,"No (such) query currently in progress.");
			break;
		case db_querylocked:
			strcpy(errStr,"Query has a lock placed on it.");
			break;
		case db_reclocked:
			strcpy(errStr,"Record is locked.");
			break;
		case db_dup:
			strcpy(errStr,"Duplicate data found in database.");
			break;
		case db_badName:
			strcpy(errStr,"Bad Name. - who knows?");
			break;
		case db_notopen:
			strcpy(errStr,"Data file not open.");
			break;
		case db_err:
			strcpy(errStr,"An unnamed error occured.");
			break;
		case db_alreadyopen:
			strcpy(errStr,"Data file is already open.");
			break;
		case db_locked:
			strcpy(errStr,"Data base is locked.");
			break;
		case db_unlocked:
			strcpy(errStr,"Database unlocked.");
			break;
		case db_nobuckets:
			strcpy(errStr,"Cannot create a bucket.");
			break;
		case db_eof:
			strcpy(errStr,"End of file reached.");
			break;
		case db_nomem:
			strcpy(errStr,"Out of memory. MV");
			break;
		case db_comm:
			strcpy(errStr,"Comm. error. MV");
			break;
		case db_refid:
			strcpy(errStr,"Bad reference id. MV");
			break;
		case db_protocol:
			strcpy(errStr,"Old protocol. MV");
			break;
		default:
			sprintf(errStr,"Danger, Will Robinson ! Unspecified error %d approaching !", err);
			break;
	}
	return errStr;
}

