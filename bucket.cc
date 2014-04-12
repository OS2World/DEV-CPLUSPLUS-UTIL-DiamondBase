/**************************************************************************
 * Source Id :
 *
 * $Id: bucket.cc,v 1.31 1993/10/05 07:29:18 kevinl Exp $
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
 *  Bucket Module
 *
 *  Original Author : Andy & Kev (with inspiration from Daz)
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: bucket.cc,v $
// Revision 1.31  1993/10/05  07:29:18  kevinl
// Fixed negative offset problem in bucket::del
//
// Revision 1.30  1993/07/20  13:05:14  kevinl
// No longer complains about unused 'in' in sploosh.
//
// Revision 1.29  1993/07/20  12:59:30  kevinl
// Some unisgned int vs long problems
//
// Revision 1.28  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.27  1993/06/20  13:43:05  kevinl
// Fixed multiple mallocs
//
// Revision 1.26  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.25  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.24  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.23  1993/05/03  23:33:37  kevinl
// long aligned data
//
// Revision 1.22  1993/05/03  01:33:03  kevinl
// Cosmetic (mainly) changes for CC
//
// Revision 1.21  1993/05/01  14:29:12  kevinl
// Replaced linear searchForKey with a binary search
// Got rid of ints
//
// Revision 1.20  1993/04/30  07:45:45  kevinl
// Some optimizing
//
// Revision 1.19  1993/04/28  11:30:15  kevinl
// Fixed up inlines
//
// Revision 1.18  1993/04/25  09:52:39  kevinl
// Fixed some dberror calls
//
// Revision 1.17  1993/04/24  07:15:27  kevinl
// Comments!
//
// Revision 1.16  1993/04/19  15:49:37  kevinl
// empty() now handles internal vs leaf nodes correctly
//
// Revision 1.15  1993/04/15  07:57:57  kevinl
// Added findIndex, del and empty
// Shortened full()
//
// Revision 1.14  1993/04/08  10:57:27  kevinl
// played with bcopy prototype to placate linux.
//
// Revision 1.13  1993/04/04  23:56:25  kevinl
// Rounded off keyLengths
// searchForKey returns orrect indicies
//
// Revision 1.12  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.11  1993/03/18  08:33:58  kevinl
// Fixed up reinitiaised default argument
//
// Revision 1.10  1993/03/17  14:27:42  kevinl
// Added index number to bucket constructor
// Added keyNum entry to all object key accesses
//
// Revision 1.9  1993/03/15  13:58:35  kevinl
// Added setParent
// changed some int's to long
//
// Revision 1.8  1993/03/08  00:30:50  kevinl
// Replaced BORLANDC with __BORLANDC__
//
// Revision 1.7  1993/02/17  12:15:53  kevinl
// Added getLink and setLink to solve alignment problems
// some tidying
//
// Revision 1.6  1993/02/13  00:55:33  kevinl
// replaced some long's with bucketId
//
// Revision 1.5  1993/02/11  03:53:38  kevinl
// a few fixes.
//
// Revision 1.4  1993/02/10  07:56:34  darrenp
// commented now.
//
// Revision 1.3  1993/02/08  14:33:24  davison
// Andy: wrote sploosh
// Kev: minor fixes to sploosh, comments, some output routines for debugging
//
// Revision 1.2  1993/02/08  00:12:28  kevinl
// Kevin adds a constructor...
//
// Revision 1.1  1993/02/06  16:50:36  davison
// Initial revision
//
 **************************************************************************/

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <defs.h>
#include <object.h>
#include <iostream.h>

// Borland needs mem.h for memmove.

#ifdef __BORLANDC__
#include <mem.h>
#endif

// Ultrix hides it's bcopy prototype in here

#ifdef __ultrix
#include <memory.h>
#endif

char* bucket::verStr(void)
{	
	return "$Id: bucket.cc,v 1.31 1993/10/05 07:29:18 kevinl Exp $";
}

// The constructor.

bucket::bucket(bucketId newId, long newKeyLength, long newSize, e_leaf newLeaf, bucketId newParentId, bucketId newNextBucket, bucketId newPrevBucket, long newKeyNum)
{

// Allocate the data for the bucket and grab a pointer to the header data.
	
	// Size is rounded up to the size of a long for allocation purposes
	// only. This is array of longs to satisfy potential alignment problems
	// with casting this to a headerData*

	data = new long[((size = newSize)+sizeof(long)-1) / sizeof(long)];

	headerData *h = (headerData*)data;

// Store the characteristics of the bucket

	h->leaf = newLeaf;
	h->parentId = newParentId;
	h->nextBucket = newNextBucket;
	h->prevBucket = newPrevBucket;
	h->activeKeys = 0;

	id = newId;

	keyLength = newKeyLength;
	keyNum = newKeyNum;

	// Each tuple contains a key, the length of which is defined by
	// the object and stored by the constructor, and a long link.

	// Round keyLength up to a multiple of 4

	myTupleSize = ((keyLength+3)&(~3)) + sizeof(long);

// The number of keys is as many tuples as will fit in the bucket, taking
// the header and initial pointer into account.

	numKeys = (size - headerSize() - sizeof(long)) / tupleSize();
}

bucket::~bucket()
{
	delete data;
}

// A little info for the programmer if s/he desires
void
bucket::info(void)
{
	cout << "HeaderSize = " << dec << headerSize() << endl
		<< "TupleSize = " << tupleSize() << endl
		<< "ActiveKeys = " << header()->activeKeys << endl
		<< "Previous = " << header()->prevBucket << "    Next = " << header()->nextBucket << endl;
}

// Used during debugging. NB Key is assumed to start with a long

void 
bucket::dump(void)
{
	cout << "Bucket Dump" << endl;

	cout << "Link: " << getLink(0) << endl;
	for (long i = 1; i <= header()->activeKeys; i++)
		cout << "Data: " << hex << *((long *)getKeyAddr(i)) << endl
		     << "Link: " << dec << getLink(i) << endl;
}

// Actually return the data in the link itself

bucketId
bucket::getLink(long pos)
{
	bucketId link;
	bcopy(getLinkAddr(pos), (char*)&link, sizeof(bucketId));
	return link;
}

// Store a new link

void
bucket::setLink(long pos, bucketId link)
{
	bcopy((char*)&link, getLinkAddr(pos), sizeof(bucketId));
}

// The key in theObject and the link are inserted into the bucket in the
// appropriate spot.

void
bucket::insert(object& theObject,long link)
{
	long pos = 0;

// This returns the position where theObject should be inserted in the bucket.

	searchForKey(theObject,pos);

// Shift the existing data along one.

	pKeyType tempKey = getKeyAddr(pos+1); // to stop it being called twice
	bcopy((char *)tempKey,(char *)getKeyAddr(pos+2), (header()->activeKeys-pos)*tupleSize());

// Now put the key in.

	pKeyType key = theObject.getKey(keyNum);
	bcopy((char *)key,(char *)tempKey,keyLength);
	delete key;

// Now the Link.
	setLink(pos+1, link);

// Now we have one more...

	header()->activeKeys++;
}

// Delete a key from a bucket

dbError
bucket::del(long pos)
{
	// Can't delete an entry that is not there.
	// Are there pos in the bucket?

	if (pos > header()->activeKeys || pos < 0)
		return dbErr(db_range);

	// All after pos roll over and one falls out...
	// NB For Inner nodes, we move starting at a link, not a key. This is
	// so that the less than link at the start of the bucket is maintained
	// accurate. The number to move is therefore 1 tuple less plus the extra
	// link.

	if (header()->leaf)
		bcopy((char*)getKeyAddr(pos+1), (char*)getKeyAddr(pos), (header()->activeKeys - pos) * tupleSize());
	else
		if (header()->activeKeys > pos)
			bcopy((char*)getLinkAddr(pos+1), (char*)getLinkAddr(pos), (header()->activeKeys - pos - 1) * tupleSize() + sizeof(long));

	// And there was one less entry in the bucket.
	header()->activeKeys--;

	return dbErr(db_ok);
}

// Find a key in the bucket or return where it should go.
// If the key is not found then return the entry before where it would be
// If the key is found, return the spot where it is

// This function has been replaced by the binary chop version below.

#if 0
bool
bucket::searchForKey(object& theObject, long& ptr)
{
	long i=1;

	while (i <= header()->activeKeys) // NB pairs are numbered 1 to n
	{
		pKeyType key = getKeyAddr(i);
		if (theObject.isLessThan(key, keyNum) )
		{
			ptr = i-1; // Needs to be inserted before i
			return FALSE;
		}
		else if (theObject.isEqual(key, keyNum) )
		{
			ptr = i;
			return TRUE;
		}
		i++;
	}
	ptr=i-1;
	return FALSE;
}
#endif

bool
bucket::searchForKey(object& theObject, long& ptr)
{
	long high = header()->activeKeys;
	long low = 1; // NB pairs are numbered 1 to n

	while (low <= high)
	{
		long mid = (low + high) / 2;
		pKeyType key = getKeyAddr(mid);
		if (theObject.isEqual(key, keyNum) )
		{
			ptr = mid;
			return TRUE;
		}
		if (theObject.isLessThan(key, keyNum) )
			high = mid - 1;
		else
			low = mid + 1;
	}
	// OK, now high is less than low. The key must therefore go after
	// position high

	ptr = high;
	return FALSE;
}

// Search a bucket for a particular link and return it's position. This is
// used during delete, when trying to find where, in it's parent, a deleted
// bucket was positioned.

void
bucket::findIndex(long& ind, bool& found)
{
	found = false;
	for (long i=0; i <= header()->activeKeys; i++)
		if (getLink(i) == ind)
		{
			found = true;
			ind = i;
			break;
		}
}

// When a bucket is full, it contents need to be split into two buckets.
// The calling function creates the new bucket and fixes the forward and
// backward links.
// In the case of a leaf bucket, the first key in the new bucket is returned
// in 'out'. In the case of an inner node, the key deleted is returned.

void
bucket::sploosh(bucket* newBucket, pKeyType in, pKeyType out, object& theObject,
				long link)
{
	in = in;

	long newKeyPos=0;
	long delFromPos=header()->activeKeys/2+1;
//	pKeyType medianKey = getKeyAddr(header()->activeKeys / 2);

// This loop is a little ugly! It determines where the key must be inserted.

	for(long i=1;(!newKeyPos) && i<=header()->activeKeys;i++)
	{
		// is in < current key ?
		if (theObject.isLessThan(getKeyAddr(i), keyNum)) // && (newKeyPos!=0))
			newKeyPos = i;
	}
	if (!newKeyPos)
		newKeyPos = i;

// Now put the excess values into the new bucket.
	
	bcopy((char *)getKeyAddr(delFromPos),(char *)newBucket->getKeyAddr(1),
			(header()->activeKeys-delFromPos+1)*tupleSize());
	newBucket->setActiveKeys(header()->activeKeys-delFromPos+1);

	setActiveKeys(delFromPos-1);

// Now add the new key where appropriate...
	
	if (newKeyPos <= delFromPos)
		insert(theObject,link);
	else
		newBucket->insert(theObject,link);

#ifdef __CC
	memcpy(out,newBucket->getKeyAddr(1),(int)keyLength);
#else
	memcpy(out,newBucket->getKeyAddr(1),keyLength);
#endif

// If this is an inner node then move everything in the new bucket down one.
// Note that it is not the first tuple that is deleted, but the link number
// 0 and key number 1. This deletes the key that will be sent back in 'out'
// and leaves the correct link in the extra spot.

	if (header()->leaf == INNER)
	{
		bcopy( newBucket->getLinkAddr(1),(char *)newBucket->firstTuple(),
				(newBucket->header()->activeKeys-1)*tupleSize()+sizeof(long));
		newBucket->setActiveKeys(newBucket->header()->activeKeys-1);
	}
}
