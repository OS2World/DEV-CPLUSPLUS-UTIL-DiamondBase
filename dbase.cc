/**************************************************************************
 * Source Id :
 *
 * $Id: dbase.cc,v 1.46 1993/11/04 14:57:43 kevinl Exp $
 *-------------------------------------------------------------------------
 * Project Notes :
 *
 *  Diamond Base
 *  ============
 *	A solid database implementation, spurred on by the continuing
 *  Metal (Lead) Base saga.
 *
 *  Project Team :
 *	  A. Davison
 *	  K. Lentin
 *	  D. Platt
 *
 *	Project Commenced : 05-02-1993
 *
 *-------------------------------------------------------------------------
 *  Module Notes :
 *
 *		Diamond Base Object : this object is the main database which will
 *	interact with client code via a transaction protocol
 *
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dbase.cc,v $
// Revision 1.46  1993/11/04  14:57:43	kevinl
// NumNames becomes nsOpen
//
// Revision 1.45  1993/10/13  13:14:36	kevinl
// Fixed get
//
// Revision 1.44  1993/10/05  07:29:41	kevinl
// Attach now handles dbObjData
//
// Revision 1.43  1993/07/19  11:43:14	kevinl
// Removed all the pesky ifdef'ed break's
//
// Revision 1.42  1993/07/11  10:49:21	kevinl
// dbString version
//
// Revision 1.41  1993/07/11  08:18:12	kevinl
// Stopped the NameServer exiting when the config file is empty/missing
//
// Revision 1.40  1993/06/23  05:21:22	kevinl
// Mallocs are now in angular brackets
//
// Revision 1.39  1993/06/20  13:43:05	kevinl
// Fixed multiple mallocs
//
// Revision 1.38  1993/05/26  01:01:39	kevinl
// MALLOC_H_MISSING
//
// Revision 1.37  1993/05/12  08:16:26	kevinl
// detaxch now kills a query
//
// Revision 1.36  1993/05/12  06:34:12	kevinl
// ti_put now checks for nfound not dup!
//
// Revision 1.35  1993/05/11  14:51:30	kevinl
// Oops, minor spelling error
//
// Revision 1.34  1993/05/11  14:42:54	kevinl
// Version numbers
// Extract does not return anything if not found
//
// Revision 1.33  1993/05/07  13:55:06	kevinl
// Fixed put
//
// Revision 1.32  1993/05/06  04:00:38	kevinl
// SASC define for malloc.h and some extra breaks
//
// Revision 1.31  1993/05/03  01:33:03	kevinl
// Cosmetic (mainly) changes for CC
//
// Revision 1.30  1993/05/01  14:37:13	kevinl
// Got rid of ints
//
// Revision 1.29  1993/04/27  07:14:13	kevinl
// Added put and write
//
// Revision 1.28  1993/04/25  11:06:40	kevinl
// Fixed up some error reporting that was a little out
// Comments
//
// Revision 1.27  1993/04/15  07:58:37	kevinl
// Added ti_del
//
// Revision 1.26  1993/04/15  04:21:52	kevinl
// Moved malloc.h
//
// Revision 1.25  1993/04/11  10:25:16	kevinl
// Fixed up return codes from find
//
// Revision 1.24  1993/04/11  05:49:02	kevinl
// Rationalised find/seek/peek methods etc
//
// Revision 1.23  1993/04/09  13:00:29	kevinl
// Stats can be called from diaRel now.
//
// Revision 1.22  1993/04/08  05:26:40	kevinl
// FGixed some memory leaks
//
// Revision 1.21  1993/04/08  00:28:41	kevinl
// Some range checking on queryId's
//
// Revision 1.20  1993/04/07  07:59:58	kevinl
// qBegin now qEnd's a query if it's active
//
// Revision 1.19  1993/04/07  02:29:21	kevinl
// Changed qEnd to reset qId to -1
//
// Revision 1.18  1993/04/06  04:42:19	kevinl
// Check queryId in trans before passing on...
//
// Revision 1.17  1993/04/06  04:27:44	kevinl
// Fixed trans so qBegin gets correct reference
//
// Revision 1.16  1993/04/06  01:01:55	kevinl
// Put a string in db_toomany
// fixed get transaction
//
// Revision 1.15  1993/04/05  01:06:12	kevinl
// Assignment of dbObj now to correct spot
//
// Revision 1.14  1993/04/05  01:02:49	kevinl
// Incorrect comparison for newDbNum fixed
//
// Revision 1.13  1993/04/05  00:52:39	kevinl
// Checking correct field in attach now for empty slot
//
// Revision 1.12  1993/04/05  00:42:17	kevinl
// replaced << with < in attach
// put in a break in attach
//
// Revision 1.11  1993/04/04  23:57:09	kevinl
// Now cache the dbObj's so they don't duisappear
// Made trans much neater by removing all the ugly indirection
//
// Revision 1.10  1993/04/01  04:23:46	kevinl
// Modified/added transactions
// Now have get, extract, and find
//
// Revision 1.9  1993/03/30  07:13:29  davison
// Um.
//
// Revision 1.8  1993/03/30  03:28:01  davison
// Fixed multiple delete problem.
//
// Revision 1.7  1993/03/29  08:04:43  darrenp
// Fixed multiple deletions of other peoples' data
// Removed bogus error reporting
// Debug malloc library
//
// Revision 1.6  1993/03/28  10:37:55  kevinl
// Modified for dbErr
//
// Revision 1.5  1993/03/28  04:53:59  root
// more error code standardization.
//
// Revision 1.4  1993/03/26  06:15:57  darrenp
// standardised error codes.
//
// Revision 1.3  1993/03/26  05:49:24  darrenp
// Small syntax errors.
//
// Revision 1.2  1993/03/25  23:25:16  davison
// Added a few more transactions
// Added the detach method
//
// Revision 1.1  1993/03/25  22:28:50  davison
// Initial revision
//
 **************************************************************************/

#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <dbase.h>
#include <nserver.h>
#include <dbobj.h>
#include <btree.h>
#include <diarel.h>
#include <version.h>

//const int MAX_REG = 100;

char*
diamondBase::verStr(void)
{
	return "$Id: dbase.cc,v 1.46 1993/11/04 14:57:43 kevinl Exp $";
}

//----------------------------------------------------
// Diamond Base constructor
//
//
//	Provide this with the name of the database description
// file.

diamondBase::diamondBase(char* name):TNameServer(name)
{
	// Startup the database.
	// Make all the arrays point to nothing.

	for(int i=0;i<MAX_REG;i++)
		regInfo[i] = 0;
	for (i=0; i<MAX_DB_INFO; i++)
		dbList[i].theDbObject = 0;
}


//---------------------------------------
//  Attach a user class to the
// database server.

dbError diamondBase::attach(const char* name, object* resBuffer, long& refId,
dbObjData* & objData)
{

	long	nameId; 		// The ID of the name, returned by the nameserver.
	char*	path;		    // The path for the relation.
	char*	prefix;
	int		cpyDbNum=-1;	// The dbList number of a user class which
				// already is the requested relation.
	int		newRegNum=-1;	// The registration number for the new user class.
	int		newDbNum=-1;	// New spot in the dbList
	int		lastDbNum=-1;	// Oldest unused dbObj

	// Is the database alive

	if (!nsOpen)
		return dbErr(db_notopen, "Database config file missing or empty");

	// First, is this relation already open ?

	path = queryName(name,nameId);
	prefix = new char[strlen(name)+1];
	strcpy(prefix,name);

	// Find a blank registration spot
	for (int i=0; i < MAX_REG; i++)
		if (!regInfo[i])
		{
			newRegNum = i;
			break;
		}

	// We couldn't find one. Invalidate refId and return an error.

	if (newRegNum == -1)
	{
		refId = newRegNum;
		return dbErr(db_toomany, "attachments");
	}

	// Search all the dbObj classes attached
	time_t lastTime = (time_t) -1;
	for(i=0;i<MAX_DB_INFO;i++)
	{
		// There is an object there
		if (dbList[i].theDbObject)
		{
			// Is it the one we want? cpyDbNum says which one to copy
			if (dbList[i].nameId == nameId) {
				cpyDbNum = i;	// Found a reference to this relation.
								// Remember it.
			}

			// See if this is the oldest detached dbObj
			if (dbList[i].detachTime && (lastTime == -1 || dbList[i].detachTime < lastTime))
			{
				// This is a detached dbObj which is older than any other
				lastTime = dbList[i].detachTime;
				lastDbNum = i;
			}
		}
		else
			if (newDbNum == -1)
				newDbNum = i; // Remember the location of the first empty slot.
	}

	// Now:
	// cpyDbNum holds the spot (if any) where this dbObj is already present
	// lastDbNum holds the spot of the oldest detached dbObj
	// newDbNum holds the spot of a free slot

	if (cpyDbNum == -1)	// Couldn't find an instance of this relation
	{
		// Ok. First we need to create a dbObject for this relation.
		dbObject* dbObj = new dbObject(path,prefix);

		if (newDbNum != -1) // Got a spare spot?
			cpyDbNum = newDbNum; // Put it here
		else
		{
			// No spare spot, nuke an old one.
			// NB refCount will be 0 otherwise no detach time
			// We delete the oldest detached one

			delete dbList[lastDbNum].theDbObject;
			cpyDbNum = lastDbNum; // And put the new one here
		}
		// Now cpyDbNum points where to put it, so store it
		dbList[cpyDbNum].theDbObject = dbObj;
	}

	// We create a new regInfo for each registration, whether the dbObj was
	// there or not.
	regInfo[newRegNum] = new dbRegInfo(resBuffer,
									   cpyDbNum);

	// Now there is one more reference
	dbList[cpyDbNum].theDbObject->usageCount++;
	objData = dbList[cpyDbNum].theDbObject->getObjData();

	// It hasn't been detached.
	dbList[cpyDbNum].detachTime = 0;

	// And who it is.
	dbList[cpyDbNum].nameId = nameId;

	// Clean up.
	delete path;
	delete prefix;
	refId = newRegNum;	// return the registration number for this user class.
	return dbErr(db_ok);
}

// The destructor. Makes sure all memory is cleaned up

diamondBase::~diamondBase()
{
	for(int i=0;i<MAX_REG;i++)
	{
		delete regInfo[i];
		regInfo[i]=0;
	}
	for (i=0; i<MAX_DB_INFO; i++)
		delete dbList[i].theDbObject;
}

// Return the current diamondBase version string

char* diamondBase::version(bool full)
{
	strstream s;
	s << "DiamondBase Version " << VERSION_MAJOR << "." << VERSION_MINOR
	  << "." << PATCH_LEVEL << PATCH_STATUS << "  Released: "
	  << RELEASE_DATE << endl;

	if (full)
	{
		s << "diamondBase: " << verStr() << endl;
		s << "TNameServer: " << TNameServer::verStr() << endl;
		bTree b;
		s << "bTree: " << b.verStr() << endl;
		s << "recServer: " << b.recServer::verStr() << endl;
		s << "cache: " << b.cache::verStr() << endl;
		// diaRel can't be instantiated.
		s << "diaRel: " << diaVerStr() << endl;
		bucket bu(0,0,0,LEAF,0,0,0);
		s << "bucket: " << bu.verStr() << endl;
		dbRegInfo ri(0,0);
		s << "dbRegInfo: " << ri.verStr() << endl;
		dbQueryInfo qi(0,0);
		s << "dbQueryInfo: " << qi.verStr() << endl;
		dbObject dbo;
		s << "dbObject: " << dbo.verStr() << endl;
		dbString str;
		s << "dbString: " << str.verStr() << endl;
	}
	return s.str();
}

//--------------------------------------------
// Perform some transaction, given by the
// id argument, with the dbObject defined
// in the regInfo structure.

#ifdef __CC
dbError diamondBase::trans(long refId, transId id, long idx)
#else
dbError diamondBase::trans(long refId, diamondBase::transId id, long idx)
#endif
{
	// Check the refId is valid
	if (refId < 0 || refId > MAX_REG)
		return dbErr(db_range);

	// Make sure it's in use. Don't use db_nfound 'cos the transaction
	// itself may. Don't want to confuse anybody.
	if (!regInfo[refId])
		return dbErr(db_range);

	// These 3 just make the code below look much better.
	// Note than in some cases below, this copy of queryId is not used,
	// but the original is. This is because the action concerned modifies
	// it.
	object* theObj = regInfo[refId]->buffer;
	dbObject* theDbObject = dbList[regInfo[refId]->dbListId].theDbObject;
	long queryId = regInfo[refId]->queryId;

	// For most situations, not having a query is an error.
	if (queryId == -1)
		switch(id)
		{
			case ti_first:
			case ti_seekFirst:
			case ti_end:
			case ti_next:
			case ti_peekNext:
			case ti_prev:
			case ti_peekPrev:
			case ti_last:
			case ti_seekLast:
			case ti_find:
			case ti_seek:
			case ti_extract:
			case ti_write:
				return dbErr(db_noquery);
			default:
				break;
		}

	// One for each transaction type. Most are self explanatory, see dbObj
	switch(id)
	{
		case ti_add:
			return theDbObject->add(*theObj);

		case ti_del:
			return theDbObject->del(*theObj);

		// A first is merely a seekFirst followed by a next
		case ti_seekFirst:
		case ti_first:
		{
			dbError err = theDbObject->qSeekFirst(queryId, *theObj);
			if (id == ti_seekFirst)
				return err;
			if (err == db_ok)
				err = theDbObject->qNext(queryId, *theObj);
			return err;
		}

		// Start a new query
		case ti_begin:
		{
			if (queryId != -1)
			{
				// If we already have a query, end it now
				dbError err;
				// This queryId not removed 'cos it's a reference
				err = theDbObject->qEnd(regInfo[refId]->queryId);
				if (err != db_ok || err != db_noquery)
					return err;
			}
			// This queryId not removed 'cos it's a reference
			return theDbObject->qBegin(idx, regInfo[refId]->queryId);
		}
		case ti_write:
			return theDbObject->qWrite(queryId, *theObj);

		case ti_end:
			// This queryId not removed 'cos it's a reference
			return theDbObject->qEnd(regInfo[refId]->queryId);

		case ti_peekNext:
			return theDbObject->qPeekNext(queryId, *theObj);

		case ti_next:
			return theDbObject->qNext(queryId, *theObj);

		case ti_peekPrev:
			return theDbObject->qPeekPrev(queryId, *theObj);

		case ti_prev:
			return theDbObject->qPrev(queryId, *theObj);

		case ti_seekLast:
		case ti_last:
		{
			dbError err = theDbObject->qSeekLast(queryId, *theObj);
			if (id == ti_seekLast)
				return err;
			if (err == db_ok)
				err = theDbObject->qPrev(queryId, *theObj);
			return err;
		}

		// All these are the same. A find/extract is a seek then a next
		// find/extract differ only in whether a lock is set
		case ti_seek:
		case ti_find:
		case ti_extract:
		{
			dbError err1 = theDbObject->qSeek(queryId, *theObj);
			if (id == ti_seek || (id==ti_extract && err1 == db_nfound))
				return dbErr(err1);
			dbError err2 = theDbObject->qNext(queryId, *theObj, (bool)(id==ti_extract));
			if (err1 == db_ok)
				return err2;
			return err1;
		}

		// get creates a query, does a find and then removes the query
		case ti_get:
		{
			long tempQid;
			dbError err;

			// Start a query
			err = theDbObject->qBegin(idx, tempQid);

			// If it failed bail out
			if (err != db_ok)
				return err;

			// Search for the object.
			err = theDbObject->qSeek(tempQid, *theObj);

			// If that fails (including not found), remove the query and go
			if (err != db_ok)
			{
				theDbObject->qEnd(tempQid);
				if (err == db_eof)
					return dbError(db_nfound);
				else
					return err;
			}

			// Otherwise take the one we want
			err = theDbObject->qNext(tempQid, *theObj, false);

			// If that fails, remove the query and go
			if (err != db_ok)
			{
				theDbObject->qEnd(tempQid);
				if (err == db_eof)
					return dbError(db_nfound);
				else
					return err;
			}

			// Otherwise end the query and return. NB this is not combined
			// with the above call to qEnd because of the different treatment
			// of the returned error. In the above call, it is ignored.
			return theDbObject->qEnd(tempQid);
		}

		// put creates a query, does a write and then removes the query
		case ti_put:
		{
			long tempQid;
			dbError err;

			// Start a query
			err = theDbObject->qBegin(idx, tempQid);

			// If it failed bail out
			if (err != db_ok)
				return err;

			// Otherwise write the data
			err = theDbObject->qWrite(tempQid, *theObj);

			if (err == db_nfound)
			{
				err = theDbObject->add(*theObj);
			}

			// If that fails, remove the query and go
			if (err != db_ok)
			{
				theDbObject->qEnd(tempQid);
				return err;
			}

			// Otherwise end the query and return. NB this is not combined
			// with the above call to qEnd because of the different treatment
			// of the returned error. In the above call, it is ignored.
			return theDbObject->qEnd(tempQid);
		}

		case ti_stats:
			theDbObject->stats();
			break;

		default:
			return dbErr(db_unimp);
	}

	return dbErr(db_ok);
}

//-----------------------------------------------------
// Detach a user class from the database server.
//
// When detaching a user class, care must be taken
// not to delete the dbObject associated with the
// user class if another user class has a handle on it.
//
// To prevent this the registration space is searched
// for references to the dbObject in question. If one
// is found, the regInfo::saveDbObject() method is called,
// preventing the dbObject from being deleted when the
// regInfo destructor is invoked.
// [KEV: See below]

dbError diamondBase::detach(long refId)
{
// Kev: I have deleted all this. It's saving the wrong reginfo
// plus since the refcount, we don't need it any more.
#if 0
    // Ok. First figure out whether or not the relation is in use
    // by another user class.

    for(int i=0;i<MAX_REG;i++)
    {
	if ((regInfo[i])&&(i!= refId))	// Don't look at nothing, nor
					// ourselves :-)
	    if (regInfo[i]->nameId == regInfo[refId]->nameId)
		regInfo[i]->saveDbObject();

    }
#endif

	// Make sure the reference is valid
	if (refId < 0 || refId > MAX_REG)
		return dbErr(db_range);

	if (!regInfo[refId])
		return dbErr(db_range);

	// First sort out the dbList
	// Decrease the reference count of the dbObject associated with the
	// regInfo in question
	if (!(--dbList[regInfo[refId]->dbListId].theDbObject->usageCount))
	{
		// Nobody else is using this. set a detach time.
		dbList[regInfo[refId]->dbListId].detachTime = time(0);

		// Don't delete anything, leave it hanging around.
		// It will be deleted in attach if its spot is needed.
		// We leave it here to save time when the next one is attached
		// since the usage pattern is often to delete an object then create a
		// new one.
	}
    // Now delete the regInfo structure for this user class.

	dbList[regInfo[refId]->dbListId].theDbObject->qEnd(regInfo[refId]->queryId);
    delete regInfo[refId];

    // And finally set it to zero to signify an empty slot.

    regInfo[refId] = 0;

    return dbErr(db_ok);
}
