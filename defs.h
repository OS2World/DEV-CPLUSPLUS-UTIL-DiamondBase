/**************************************************************************
 * Source Id :
 *
 * $Id: defs.h,v 1.29 1993/11/05 12:45:05 kevinl Exp $
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
 *  Common types and definitions
 *
 *
 *  Original Author : Dunno. Andy started it. Kev finished it.
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: defs.h,v $
 * Revision 1.29  1993/11/05  12:45:05	kevinl
 * Fixed problems that CC picked up (and some multi bugs)
 *
 * Revision 1.28  1993/11/04  22:24:32	darrenp
 * Avoided prototype for strcasesmp for linux as it already has one.
 *
 * Revision 1.27  1993/11/03  10:04:46	kevinl
 * Added ichar and utils.{h,cc}
 *
 * Revision 1.26  1993/10/24  15:35:08	kevinl
 * Fix for strcasecmp vs stricmp
 *
 * Revision 1.25  1993/10/18  11:19:12	kevinl
 * Added strNcpy
 *
 * Revision 1.24  1993/10/05  07:31:29	kevinl
 * Had problems with some sytems #define'ing TRUE and FALSE
 *
 * Revision 1.23  1993/06/23  05:21:22	kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.22  1993/05/11  14:44:50	kevinl
 * Added version number output
 *
 * Revision 1.21  1993/05/03  23:33:53	kevinl
 * long aligned data
 *
 * Revision 1.20  1993/05/03  01:33:03	kevinl
 * Cosmetic (mainly) changes for CC
 *
 * Revision 1.19  1993/05/01  14:39:19	kevinl
 * Got rid of ints
 *
 * Revision 1.18  1993/04/30  07:45:45	kevinl
 * Some optimizing
 *
 * Revision 1.17  1993/04/29  07:22:23	kevinl
 * Removed some bucket::'s
 *
 * Revision 1.16  1993/04/28  11:30:15	kevinl
 * Fixed up inlines
 *
 * Revision 1.15  1993/04/27  10:31:23	darrenp
 * Added BOOL_DEFINED flag.
 *
 * Revision 1.14  1993/04/26  00:35:30	kevinl
 * Comments
 *
 * Revision 1.13  1993/04/15  07:58:17	kevinl
 * Added findIndex, del and empty
 *
 * Revision 1.12  1993/04/04  23:58:41	kevinl
 * Added get & setParent
 *
 * Revision 1.11  1993/03/26  06:16:38	darrenp
 * standardized error codes.
 *
 * Revision 1.10  1993/03/21  23:35:44	kevinl
 * Added typedefs for unsigned types
 * Changd pKeyType to a char*
 *
 * Revision 1.9  1993/03/17  14:28:34  kevinl
 * newKeyNum argument in bucket constructors
 * changed some int data members to long
 *
 * Revision 1.8  1993/03/15  14:19:33  kevinl
 * Fixed up BORLANDC probs
 * Changed some int's to long
 * added empty bucket() constructor
 * added setParent
 *
 * Revision 1.7  1993/02/17  12:15:16  kevinl
 * Fixed pointer alignment problems by adding getLink and setLink
 *
 * Revision 1.6  1993/02/13  00:56:04  kevinl
 * long's replaced with bucketId
 * getShot nuked
 *
 * Revision 1.5  1993/02/11  03:53:38  kevinl
 * a few fixes.
 *
 * Revision 1.4  1993/02/10  07:59:03  darrenp
 * Global definitions of things like bool.
 *
 * Revision 1.3  1993/02/08  14:36:00  darrenp
 * sploosh added, some output routines in class bucket
 * enum for leaf and some Borlandc/Unix confrontation added
 *
 * Revision 1.2  1993/02/08  00:13:21  kevinl
 * Kevin adds a constructor for the bucket class...
 *
 * Revision 1.1  1993/02/06  16:33:53  kevinl
 * Initial revision
 *
 **************************************************************************/
#ifndef __DEFS_H__
#define __DEFS_H__

#include <dberr.h>

#ifdef __CC
extern "C" {
void bcopy(const void*, void*, long);
}
#else
#include <string.h>
#endif // __CC

// Some useful types...

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;

typedef long bucketId;

#ifndef BOOL_DEFINED
#define BOOL_DEFINED
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
enum bool {FALSE=0, TRUE=1,false=0,true=1};
#endif

enum e_leaf {INNER, LEAF};

// Borland and Unix don't share a memory move/copy routine that understands
// overlaps. Also the two have different argument orderings.

#if defined(__BORLANDC__) || defined(__EMX__)
#define bcopy(a,b,c) memmove(b,a,c)
#define strcasecmp(a,b) stricmp(a,b)
#define strncasecmp(a,b,c) strnicmp(a,b,c)
#else
extern "C" {
#if !defined(linux) && !defined(__CC)
int strncasecmp(char*, char*, int);
#endif
}
#endif

// maximum number of btree queries
#define MAX_QUERY 10

class object;
class bucket;

// All keys stored in buckets are of this type. Objects should cast these
// as appropriate.

typedef char* pKeyType;

// Data stored at the head of each bucket's data area.

struct headerData
{
	e_leaf		leaf;
	bucketId	parentId;
	bucketId	nextBucket;
	bucketId	prevBucket;
	long		activeKeys;
};

// Class definition for the bucket.

class bucket
{
	long*		data; // This is long* so that the headerData is aligned right
	bucketId	id;
	long		keyLength;
	long		numKeys;
	long		size;
	long		keyNum;
	long		myTupleSize;

public:
//-----
	bucket(bucketId newId, long newKeyLength, long newSize, e_leaf newLeaf, bucketId newParentId, bucketId newNextBucket, bucketId newPrevBucket, long NewkeyNum=0);
//	bucket(long newKeyNum=0 ) {};
	~bucket();
	inline bucketId getId(void)
		{
			return id;
		}
	void	info(void);
	inline	long headerSize(void)
		{
			return sizeof(headerData);
		}

	inline	long tupleSize(void)
		{
			return myTupleSize;
		}

	inline	headerData* header(void)
		{
		// Return a correctly typed pointer to the header data.
		// This gives access to private data and should probably be
		// replaced with access methods for all header data individually.

			return (headerData*)data;
		}

	inline	char* firstTuple(void)
		{
		// The keys and links in the bucket are numbered as follows:
		//    +-+-+-+-+-+-+-+
		//    |L|K|L|K|L|K|L|  L = Link,  K = Key
		//    +-+-+-+-+-+-+-+
		//     0 1 1 2 2 3 3
		//

		// Return the address of the
		// start of the Tuple space (i.e.
		// the address of Link 0).

			return (char*)data + headerSize();
		}

	inline	pKeyType getKeyAddr(long pos)
		{
		// Return a pointer to the pos'th
		// Key in the Tuple space.

			pKeyType key = (pKeyType)(firstTuple() + (pos-1) * tupleSize() +sizeof(long));
			return key;
		}

	inline	char* getLinkAddr(long pos)
		{
		// Return a pointer to the pos'th link
		// in the Tuple space.
		// char * so no unaligned long accesses with keys not a multiple
		// of 4 long. This problem now removed with keys rounded up to
		// multiples of 4 long

			char* link = (firstTuple()+pos*tupleSize());
			return link;
		}

	bucketId	getLink(long pos);
	void		setLink(long pos, bucketId link);
	bool	searchForKey(object& theObject, long& ptr);
	void	findIndex(long& ind, bool& found);
	void	insert(object& theObject, long link);
	dbError del(long pos);
	inline	bool	full(void)
		{
			return (bool)(numKeys == header()->activeKeys);
		}

	inline	bool	empty(void)
		{
		// Leaf buckets are considered empty when 0 keys are present.
		// Inner buckets still contain a link at this point so we test for -1.
		// A slight adaptation to the delete mechanism, with appropriate care
		// for balancing might not require this strange treatment.

			return (bool)(header()->activeKeys == ((header()->leaf==LEAF)?0:-1));
		}

	inline	pKeyType allocTmpKey(void)
		{
		// The bucket is in charge of allocating temporary key areas but
		// the deleting is left to the caller.

			return (pKeyType)new char[keyLength];
		}

	void	sploosh(bucket* newBucket, pKeyType in, pKeyType out, object& theObject, long link);

	// A couple of access to functions to keep our data safe...

	inline	void setActiveKeys(long keys)
		{
			header()->activeKeys = keys;
		}

	inline	bucketId getParent(void)
		{
			return header()->parentId;
		}

	inline	void setParent(bucketId parent)
		{
			header()->parentId = parent;
		}

	void	dump(void);
	inline	void*	getData(void)
		{
			return (void*)data;
		}
	char* verStr(void);

friend class bTree;

}; //---- class def for Buckets...

#endif

