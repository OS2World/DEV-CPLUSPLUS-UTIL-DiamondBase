/**************************************************************************
 * Source Id :
 *
 * $Id: dbase.h,v 1.15 1993/10/05 07:30:22 kevinl Exp $
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
 *      DiamondBase Object : This object is the main database which will
 *  interact with client code via a transaction protocol
 *
 *  Original Author :
 *		Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dbase.h,v $
 * Revision 1.15  1993/10/05  07:30:22  kevinl
 * AAdded dbObjData to attach
 *
 * Revision 1.14  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.13  1993/05/11  15:14:41  kevinl
 * Made constructor for diamondBase have an optional argument since
 *      TNameServer can handle the null.
 *
 * Revision 1.12  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.11  1993/05/01  14:37:13  kevinl
 * Got rid of ints
 *
 * Revision 1.10  1993/04/27  07:14:13  kevinl
 * Added put and write
 *
 * Revision 1.9  1993/04/11  05:49:02  kevinl
 * Rationalised find/seek/peek methods etc
 *
 * Revision 1.8  1993/04/09  13:00:29  kevinl
 * Stats can be called from diaRel now.
 *
 * Revision 1.7  1993/04/04  23:57:47  kevinl
 * Cache dbObj's
 *
 * Revision 1.6  1993/04/01  04:23:46  kevinl
 * Modified/added transactions
 * Now have get, extract, and find
 *
 * Revision 1.5  1993/03/30  07:13:52  davison
 * Usage counter for dbObject implemented, thus burying the delete bug (this
 * time Rocky, fur shur !)
 *
 * Revision 1.4  1993/03/29  08:05:26  darrenp
 * Removed deletion of other peoples' data
 *
 * Revision 1.3  1993/03/26  06:16:38  darrenp
 * standardized error codes.
 *
 * Revision 1.2  1993/03/25  23:25:36  davison
 * Added the detach method.
 *
 * Revision 1.1  1993/03/25  22:28:50  davison
 * Initial revision
 *
 **************************************************************************/

#ifndef __DiamondBaseObject_H__
#define __DiamondBaseObject_H__

#define MAX_REG 20
#define MAX_DB_INFO 20

#if (MAX_REG > MAX_DB_INFO)
#error There must be at least as many dbObj slots as regInfo slots
#endif

#include <nserver.h>
#include <dbobj.h>
#include <time.h>

class diamondBase;
class dbObject;

// Who put this here?
// extern diamondBase theDBase;

//------------------------------------------------------------------
// dbInfo struct. This holds this dbObj. We use this so that dbObj's
// can remain open, thus effecting good cacheing.

struct dbInfo {
	dbObject	*theDbObject;
	long		nameId;
	time_t		detachTime;
};

//------------------------------------------------
// User Class Registration Information Structure.
//
//	Keeps track of the user classes currently attatched
// to the database server. This information includes
// a pointer to the buffer area, a pointer to the 
// dbObject the user has requested access to, the
// nameId for the relation and the current
// query context.

class dbRegInfo
{
	object* 	buffer;
	long		dbListId;
	long		queryId;
public:
	dbRegInfo(object* theObj, long listId)
	{
		buffer = theObj;
		dbListId = listId;
		queryId = -1;
	}

	~dbRegInfo()
	{
		// Repeat a thousand times, I shalt not delete other peoples data!
		// delete buffer;
//		if(!(--theDbObject->usageCount))
//			delete theDbObject;
//		nameId = -1;
	}
	char* verStr(void) { return "$Id: dbase.h,v 1.15 1993/10/05 07:30:22 kevinl Exp $"; }
#if 0
	void saveDbObject(void)
	{
		theDbObject = 0;
	}
#endif
	friend class diamondBase;
};

//---------------------------------------------
//  The staticly linked database server.

class diamondBase  : public TNameServer
{
	private:
		dbRegInfo*		regInfo[MAX_REG];	// The registration info.
		dbInfo			dbList[MAX_DB_INFO]; // The open relations
//		long			nameIds[MAX_RELATIONS][2];
						// The first element of this is the nameId for
						// some relation which has been registered. The second
						// is the number of user classes currently accessing
						// it. When User classes are detached, this array is
						// checked to determine whether or not to delete the
						// dbObject associated with the user class.
	public:
		enum transId {ti_add,ti_del,ti_begin,ti_end,ti_get,ti_put,ti_extract,
			ti_seekFirst,ti_first,ti_next,ti_prev,ti_seekLast,ti_last,
			ti_peekNext,ti_peekPrev,ti_seek,ti_find,ti_stats,ti_write};

		char* verStr(void);
		char* version(bool full = false);
		diamondBase(char* name = 0);
		dbError attach(const char* name, object* resBuffer, long& refId, dbObjData* & objData);
		dbError detach(long refId);
		dbError trans(long refId,transId id,long idx=0);
		~diamondBase();
};
#endif
