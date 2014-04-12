/**************************************************************************
 * Source Id :
 *
 * $Id: btree.h,v 1.20 1993/06/23 05:21:22 kevinl Exp $
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
 *  The bTree code. Implements a B+Tree (sort of)
 *
 *
 *  Original Author : Daz & Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: btree.h,v $
 * Revision 1.20  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.19  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.18  1993/05/01  14:27:04  kevinl
 * Stripped ;'s off inlines
 * Got rid of ints
 * Made inBTree two functions
 *
 * Revision 1.17  1993/04/29  07:00:03  kevinl
 * Added lsoeBucket
 *
 * Revision 1.16  1993/04/24  14:31:27  kevinl
 * Comments
 * Some small fixups and error code changes.
 *
 * Revision 1.15  1993/04/19  15:50:03  kevinl
 * fixParent is no longer boolean. now int
 *
 * Revision 1.14  1993/04/15  07:55:24  kevinl
 * Traverse & dump now have intonly mode
 * inBTree takes optional reference to return spot where found.
 *
 * Revision 1.13  1993/04/08  07:23:40  kevinl
 * insertInternal now takes refernce to bucket* so delete on outside works
 *
 * Revision 1.12  1993/03/28  13:58:23  kevinl
 * Added inBTree
 *
 * Revision 1.11  1993/03/28  10:35:26  kevinl
 * Modifed for dbErr
 *
 * Revision 1.10  1993/03/28  19:07:10  davison
 * Deleted pError member.
 *
 * Revision 1.9  1993/03/26  06:16:38  darrenp
 * standardized error codes.
 *
 * Revision 1.8  1993/03/24  06:15:58  kevinl
 * Added pError and bterror members
 *
 * Revision 1.7  1993/03/17  14:26:31  kevinl
 * Fixed up include files and mutiple include protection.
 *
 * Revision 1.6  1993/03/15  13:56:27  kevinl
 * Put query stuff (back) in and finished all of it.
 *
 * Revision 1.5  1993/03/08  00:30:15  kevinl
 * Added (and ifdef'd out) beginnings of query code
 * Added integrity checks.
 *
 * Revision 1.4  1993/02/18  05:40:16  kevinl
 * Added query transaction methods
 *
 * Revision 1.3  1993/02/17  12:14:02  darrenp
 * Fixed definition of findInternal
 *
 * Revision 1.2  1993/02/13  00:54:07  kevinl
 * new members added for insertInternal, createNewBucket, writeBucket
 * added dbError enum for error codes
 *
 **************************************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include <defs.h>
#include <rserv.h>
#include <object.h>

// The header data that the btree stores at the start of its revServer

struct bTreeHead {
	long keyLen;
	bucketId rootBucket;
};

// The query structure for bTree. The constructor is so that the class
// that uses this can compile. All its structures/subclasses must have
// constructors.

struct bTreeQuery {
	pKeyType	key;	// Current key, used if unsafe
	bucket*		bptr;	// Current bucket
	long		idx;	// The index into bptr, used if !unsafe
	bool		unsafe;	// Has someone made a change to influence this query
	bool		used;	// Query used?
	bTreeQuery() {}		// Keep the compiler happy
};

// The bTree class. It inherits recServer to store its data.
// Each recServer record is a bucket in the bTree.

class bTree : public recServer
{
	bTreeHead head;
	long indNum; // Which index to quote when asking for comparisons.
	bTreeQuery query[MAX_QUERY];

	void findInternal(
		object 		&searchObject,
		bucketId 	rootBucket,
		bool		&Found,
		bucket		**bptr,
		long		&ptrIdx,
		int			fixParent = 0
	);
	bucket* fetchBucket(bucketId idx);
	bucket* createNewBucket(e_leaf leaf, bucketId parentId, bucketId nextBucket, bucketId prevBucket);
	void writeBucket(bucket* bptr);
	void loseBucket(bucketId id);
	void traverse(bucket* b, bool intonly);
	void initQueries(void);
	bool internalCheckIntegrity(bucketId buck);

public:
#if 0
	dbError status;
	inline dbError bterror(dbError err) { return (status = err); };
#endif

	char* verStr(void);
	bTree(char *name, long bucketSize, long keyLen, long newIndNum=0); // create constructor
	bTree(char *name, long newIndNum=0);  // open constructor
	bTree(void) {} // Blank constructor. Use with care.
	dbError add(object &newObject, long extPtr);
	dbError del(object &delObject);
//	dbError find(object &findObject, long &extPtr); // see qFind
	bool checkIntegrity(void);
	bool inBTree(object &theObject, long& idx);
	inline bool inBTree(object &theObject)
		{
			long temp;
			return inBTree(theObject, temp);
		}
	dbError qBegin(long& qNum);
	dbError qEnd(long qNum);
	dbError qFirst(long qNum, object &theObject, long &extPtr); // Act as peek
	dbError qLast(long qNum, object &theObject, long &extPtr); // Act as peek
	dbError qFind(long qNum, object& findObject, long& extPtr);
	dbError qNext(long qNum, object &theObject, long &extPtr);
	dbError qPrev(long qNum, object &theObject, long &extPtr);
	dbError qPeekNext(long qNum, object &theObject, long &extPtr);
	dbError qPeekPrev(long qNum, object &theObject, long &extPtr);

	void dump(bool intonly = false);

private:
	dbError insertInternal(bucket*& bptr,object &newObj,long extPtr);
};

#endif
