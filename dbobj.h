/**************************************************************************
 * Source Id :
 *
 * $Id: dbobj.h,v 1.24 1993/11/04 14:59:41 kevinl Exp $
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
 *		dbObject     : This is an object which handles low-level requests
 *                     from the diamondBase.
 *
 *  Original Author :
 *		Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dbobj.h,v $
 * Revision 1.24  1993/11/04  14:59:41  kevinl
 * Fixed record locking
 *
 * Revision 1.23  1993/10/28  07:49:01  kevinl
 * Added diaGRel dbString/dbData support
 *
 * Revision 1.22  1993/10/18  11:18:40  kevinl
 * Added fieldName array
 *
 * Revision 1.21  1993/10/05  07:29:57  kevinl
 * Added in dbObjData (previously in diaGRel)
 *
 * Revision 1.20  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.19  1993/06/20  13:39:23  kevinl
 * theMemServer etc for String support
 *
 * Revision 1.18  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.17  1993/05/03  01:34:25  kevinl
 * No more extern consts
 *
 * Revision 1.16  1993/05/01  14:38:08  kevinl
 * Got rid of ints
 * Reowrked inlie funcs to have 1 return
 *
 * Revision 1.15  1993/04/27  07:15:24  kevinl
 * Added qWrite
 * Fixed qId bounds checks on all query functions
 *
 * Revision 1.14  1993/04/25  13:19:57  kevinl
 * Comments
 *
 * Revision 1.13  1993/04/15  07:58:55  kevinl
 * Added del
 *
 * Revision 1.12  1993/04/15  04:28:17  kevinl
 * Move malloc.h
 *
 * Revision 1.11  1993/04/11  05:49:02  kevinl
 * Rationalised find/seek/peek methods etc
 *
 * Revision 1.10  1993/04/09  13:00:29  kevinl
 * Stats can be called from diaRel now.
 *
 * Revision 1.9  1993/04/07  02:29:21  kevinl
 * Changed qEnd to reset qId to -1
 *
 * Revision 1.8  1993/04/01  04:23:18  kevinl
 * Fixed locking
 * Added qFind, fixed the rest
 *
 * Revision 1.7  1993/03/30  07:15:19  davison
 * Added the usage count stuff.
 *
 * Revision 1.6  1993/03/29  08:06:01  darrenp
 * Nuked pathinfo
 *
 * Revision 1.5  1993/03/29  06:12:49  darrenp
 * Nuked pathinfo structure
 *
 * Revision 1.4  1993/03/28  10:32:05  davison
 * Modified for dbErr.
 *
 * Revision 1.3  1993/03/28  04:53:59  root
 * more error code standardization.
 *
 * Revision 1.2  1993/03/26  06:16:38  darrenp
 * standardized error codes.
 *
 * Revision 1.1  1993/03/25  22:29:25  davison
 * Initial revision
 *
 **************************************************************************/

#ifndef __dbObject_h__
#define __dbObject_h__

#include <idxinfo.h>
#include <rserv.h>
#include <btree.h>
#include <object.h>
#include <mserv.h>
#include <iostream.h>
#include <fstream.h>

const int NO_REC = -1;

class dbObject;
class dbRegInfo;
class diamondBase;

class dbQueryInfo
{
	long	queryId;
	long	queryIdx;
	long	queryRec;
	bool	queryLock;
public:
//-----
//	dbError status;

//	inline dberror(dbError err) { return status = err; };

	char* verStr(void) { return "$Id: dbobj.h,v 1.24 1993/11/04 14:59:41 kevinl Exp $"; }
	dbQueryInfo(long id,long idx)
	{
		queryId = id;
		queryIdx = idx;
		queryRec = NO_REC;
		queryLock = false;
	}

#if 0
	void pError(char *s)
	{
		cerr << ": ";
		switch (status)
		{	
			case db_ok: cerr << "OK"; break;
			case db_locked: cerr << "Query locked"; break;
			case db_unlocked: cerr << "Query not locked"; break;
			default: cerr << "GURGLE GURGLE SPLAT"; break;
		}
		cerr << endl;
	}
#endif

	// Lock the query. If it's already locked, tell them
	dbError lock()
	{
		dbError err = db_locked;
		if (queryLock == false)
		{
			queryLock = true;
			err = db_ok;
		}
		return dbErr(err);
	}

	// Unlock the query. If it's already unlocked, tell them
	dbError unlock(void)
	{
		dbError err = db_unlocked;
		if (queryLock == true)
		{
			queryLock = false;
			err = db_ok;
		}
		return dbErr(err);
	}

	// If the query is not locked, set the record this query used.
	dbError setRec(long recIdx)
	{
		dbError err = db_locked;
		if (queryLock == false)
		{
			queryRec = recIdx;
			err = db_ok;
		}
		return dbErr(err);
	}

	// We locked?
	bool locked(void)
	{
		return queryLock;
	}

friend dbObject;
		
};

#define MFII MAX_FIELDS_IN_INDEX
class dbObjData
{
public:
	// ----------------------------
	// Data which is used to carry out the requested
	// operations on the relation.
	// ----------------------------
	long		uniqOff;		// unique offset - -1 if not present 
	long		numIndices;		// How many indices are there ?
	long		numFields;		// How many fields are there ?
	long		dataLength;		// What is the total length of the rec ?
	long		stringCount;	// How many of the buggers we got?
	long		*stringField;	// What are the numbers of the strings;
	long		*theLongs;		// Array of offsets;
	long		*containsUniq;	// Index contains a unique field.
	long		*keyLength;		// Key lengths.
	long		*fieldLength;	// How long each field is.
	long		*fieldOffset;	// Where each field is.
	char		*fieldType;		// What type each field is.
	char		**fieldName;	// The field names.

	// These next two arrays are a real nightmare.
	// They list which fields the indexes are compared
	// on. There are two arrays, b/c they are compared in a
	// different order to their packing order. C=comparison order
	// P = packing order (during storage into btree)

	long		(*indexedOnC)[MFII];// Which fields are in each index.
	long		(*indexedOnP)[MFII];// Which fields are in each index.
};

class dbObject : public dbObjData
{
public:
//-----

//	dbError status;
//	inline dbError dboerror(dbError err) { return status = err; };
//	void pError(char *s);

private:
//------
	char*		path;
	char*		prefix;
	fieldList	fldList;
	indexList	idxList;
	recServer*	theRecServer;
	bTree**		theBTreeList;
	memServer*	theMemServer;

	dbQueryInfo*	queryInfo[MAX_QUERY];

	long		usageCount;

public:
//-----
	dbObject(void)
	{
		idxList.numIndicies=0;
		theBTreeList=0;
		theRecServer=0;
		theMemServer=0;
		path=0;
		prefix=0;
	} // Use with care.
	dbObject(char* path, char* name);
	~dbObject();
	dbObjData* getObjData(void) { return (dbObjData*)this; }
	char* verStr(void);
	dbError checkRec(long qId, long recIdx, bool lockit = false);
	dbError qBegin(long idxId, long& qId);
	dbError qEnd(long& qId);
	dbError qSeekFirst(long qId, object& theObject);
	dbError qSeekLast(long qId, object& theObject);
	dbError qNext(long qId, object& theObject, bool lock = false);
	dbError qPeekNext(long qId, object& theObject);
	dbError qPrev(long qId, object& theObject, bool lock = false);
	dbError qPeekPrev(long qId, object& theObject);
	dbError qSeek(long qId, object& theObject);
	dbError qWrite(long qId, object& theObject);
	dbError add(object& theObject);
	dbError del(object& theObject);
	void	stats(void);
	dbError	getStrings(object& theObject);

friend class dbRegInfo;
friend class diamondBase;

};

#endif
