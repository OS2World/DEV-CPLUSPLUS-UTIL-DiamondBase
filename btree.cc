/**************************************************************************
 * Source Id :
 *
 * $Id: btree.cc,v 1.38 1993/06/23 05:21:22 kevinl Exp $
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
 *  The main B-Tree implemetation
 *
 *
 *  Original Author : Daz & Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: btree.cc,v $
// Revision 1.38  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.37  1993/06/20  13:43:05  kevinl
// Fixed multiple mallocs
//
// Revision 1.36  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.35  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.34  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.33  1993/05/03  01:33:03  kevinl
// Cosmetic (mainly) changes for CC
//
// Revision 1.32  1993/05/01  14:27:42  kevinl
// Got rid of ints
//
// Revision 1.31  1993/04/30  04:07:28  kevinl
// Incorrect !used queries
//
// Revision 1.30  1993/04/29  15:10:01  kevinl
// Fixed the BIG BUG. idx = 1 was all that was needed!
//
// Revision 1.29  1993/04/29  12:55:16  kevinl
// Fixed up setting of query[]->key
//
// Revision 1.28  1993/04/29  07:00:03  kevinl
// Added lsoeBucket
//
// Revision 1.27  1993/04/28  14:29:31  kevinl
// inBTree returning incorrect idx
//
// Revision 1.26  1993/04/28  10:36:47  kevinl
// qPeekNext, qPeekPrev not expecting eof from bTree
//
// Revision 1.25  1993/04/27  07:02:47  kevinl
// Next and PeekNext return codes were broken
//
// Revision 1.24  1993/04/24  14:31:27  kevinl
// Comments
// Some small fixups and error code changes.
//
// Revision 1.23  1993/04/19  15:50:29  kevinl
// fixParent is now an int.
// Convert root node to LEAF when it becomes empty/
//
// Revision 1.22  1993/04/15  07:56:13  kevinl
// Traverse & dump have intonly mode
// inBTree takes optional reference to return spot where found
// Added del
//
// Revision 1.21  1993/04/11  10:24:56  kevinl
// Fixed up jumping from bucket to bucket in a find
//
// Revision 1.20  1993/04/08  15:39:38  kevinl
// put some output into DEBUG blocks
//
// Revision 1.19  1993/04/08  07:23:40  kevinl
// insertInternal now takes refernce to bucket* so delete on outside works
//
// Revision 1.18  1993/04/08  06:41:14  kevinl
// Memory leak related to findInternal()
//
// Revision 1.17  1993/04/06  05:47:26  kevinl
// Moved key reinstatement down
//
// Revision 1.16  1993/04/05  07:50:08  kevinl
// Fixed the rereading in add
//
// Revision 1.15  1993/04/05  01:13:11  kevinl
// Fixed incorrect idx entries from searchForKey
// Made traverse/dump more friendly
// made fixparent a little less disk intensive
//
// Revision 1.14  1993/03/29  08:18:39  darrenp
// Added query bptr validity check
// malloc lib
//
// Revision 1.13  1993/03/28  13:58:23  kevinl
// Added inBTree
//
// Revision 1.12  1993/03/28  10:37:11  kevinl
// Modified for dbErr
//
// Revision 1.11  1993/03/28  19:06:27  davison
// Changed references to pError to dbErr().
//
// Revision 1.10  1993/03/26  06:15:57  darrenp
// standardised error codes.
//
// Revision 1.9  1993/03/24  06:15:58  kevinl
// Added pError and bterror members
//
// Revision 1.8  1993/03/21  00:47:49  davison
// Included <stdio.h> to fix pError error.
//
// Revision 1.7  1993/03/20  14:08:45  kevinl
// Fixed up call to recServer constructor
// Check return code of recServer constructor
//
// Revision 1.6  1993/03/17  14:27:00  kevinl
// Added indNum to bucket creation calls
//
// Revision 1.5  1993/03/15  13:59:46  kevinl
// Added parent pointer fixup
// Added integrity check
// Finished query members
//
// Revision 1.4  1993/03/08  00:29:32  kevinl
// Added multiple index handling
// Added (and ifdef'ed out) query handling - partial
// Added integrity check for next/prev links
//
// Revision 1.3  1993/02/17  12:14:29  darrenp
// Insert, find and search work correctly
// still minor bug in insert routine to be found
//
// Revision 1.2  1993/02/13  00:53:01  kevinl
// fixed insertInternal to point where it compiles
// added supporting functions: writeBucket, createNewBucket
//
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <iostream.h>
#include <btree.h>

extern "C" {
int isprint(char);
}

char* bTree::verStr(void)
{
	return "$Id: btree.cc,v 1.38 1993/06/23 05:21:22 kevinl Exp $";
}

// The create constructor. This creates a new empty recServer correctly set
// up for the bTree.

bTree::bTree(char *name, long bucketSize, long keyLen, long newIndNum) : recServer()
{
	bucketId idx;
	
	initQueries();

	// This is the index number that this bTree is being used for.
	// It is passed to all object member functions.

	indNum = newIndNum;

	if (createDb(name, bucketSize, sizeof(bTreeHead)) != db_ok)
	{
		// Oops. recServer failed. Die very ungracefully
		dbErr(name);
		exit (1);
	}

	// Find a place for the root bucket

	if (newRec(idx) != db_ok)
	{
		dbErr("newRec");
		exit (1);
	}


	// This is a bit presumptuous, but it sort of ensures that an empty
	// recServer has being correctly created.

	if (idx)
	{
		cerr << "First record of created db should be 0 - " << idx << " instead" << endl;
		exit (1);
	}

	head.rootBucket = idx;
	head.keyLen = keyLen;

	// Create the root bucket. It is a LEAF bucket with no siblings or parent

	bucket b(idx, keyLen, bucketSize, LEAF, -1L, -1L, -1L, indNum);

	// Write out the header data.

	if (!writeData(0, (void *)&head, sizeof(bTreeHead)))
	{
		dbErr("Write bTreeHead");
		exit (1);
	}

	// Now write out the root bucket.

	if (putRec(idx, b.getData()) != db_ok)
	{
		cerr << "Record " << idx;
		dbErr("");
		exit (1);
	}
}

// fetchBucket:
// Create a new bucket and load the requested entry from the recServer.
// It is the responsibility of the calling function to deallocate the memory.
// The LEAF status, sibling and parent info are all overwritten by the
// recServer read.

bucket*
bTree::fetchBucket(bucketId idx)
{	
	bucket *buck = new bucket(idx, head.keyLen, header.recLength, LEAF, 0, 0, 0, indNum);
	getRec(idx, buck->getData());
	return buck;
}

// The load constructor. This is for instantiating an existing bTree and
// load it into the recServer. The recServer is told of the amount of user
// data that is present. All other data, besides the index number are stored
// in the recServer.

bTree::bTree(char *name, long newIndNum) : recServer(name, sizeof(bTreeHead))
{
	if (!isOpen)
	{	
		dbErr("bTree: recServer constructor failed");
		exit (1);
	}

	// Zero out the query array and store the index number

	initQueries();
	indNum = newIndNum;

	// Get the header data from the recServer.

	if (!readData(0, &head, sizeof(bTreeHead)))
	{
		dbErr("read bTreeHead");
		exit (1);
	}
}

//
// The internal find routine, as so accurately predicated by Kevin Lentin.
//
// (Thanks for the credit Daz :-)
// findInternal starts with the root bucket and continually searches for
// the requested object, loading the found (or following) bucket until a leaf
// node is found. It then returns if and where it found (or did not find) the
// requested key.
// bptr is a bucket** because findInternal actually returns to the caller
// the bucket where the object was found. findInternal frees any existing
// bucket and ensures all internally used buckets are freed, thus bptr is the
// only unfree bucket lieing around.
// 

void bTree::findInternal(
	object&		searchObject,	// What we're looking for
	bucketId 	rootBucket,		// Where to start
	bool&		Found,			// Did we find it?
	bucket**	bptr,			// The bucket to search for.
	long&		ptrIdx,
	int			fixParent
)
{

	// We start at the very beginning...

	bucketId 	nextBucket = rootBucket;
	bucketId	parent = -1;

	do {
		long found;
		
		// Get the next bucket, deleting the old one if appropriate

		if (*bptr) delete (*bptr);
		*bptr = fetchBucket(nextBucket);

		// The parent pointer can be corrupted during the sploosh process
		// because the bucket is split in half and it is wastefull to go down
		// and change all buckets under the new node.
		// Hence, if, while searching, we find a full bucket and are part
		// of an add (fixParent == 1), we ensure the parent is correct so that
		// the possibly impending sploosh will be able to traverse up the tree
		// correctly.
		// If we are doing a delete and the bucket is about to become
		// empty, we similarly fix the parent pointer.
		// To save time, we only do this if the parent is wrong.

		if	((		(fixParent==1 && (*bptr)->full())
				||	(fixParent==2 && (*bptr)->header()->activeKeys==1))
			&& (*bptr)->getParent() != parent) 
		{
			(*bptr)->setParent(parent);
#ifdef DEBUG
			cout << "Fixing parent pointer" << endl;
#endif
			writeBucket(*bptr);
		}

		found = (*bptr)->searchForKey(searchObject,ptrIdx);
		// This returns the ptrIdx of the ptr after the key (if found)
		// or the ptr to follow if it wasn't found. If you're in an internal
		// node, then you probably want to follow the following pointer
		if (found) {
			// We got one!
			if ((*bptr)->header()->leaf == LEAF) {
				// We have found the record in a leaf, so this is
				// the real thing.
				Found = true; 
				return;
			} else {
				// Found the record in an internal node, so it's just a
				// guide index, follow the pointer next to it.
				parent = nextBucket;// Store the current bucket as the
									// parent of the next one
				nextBucket = (*bptr)->getLink(ptrIdx);
			}
		} else { // Not found
			if ((*bptr)->header()->leaf == LEAF) {
				// End of the road - report not found.
				Found = false;
				return;
			}
			// ok, we didn't find it in this internal node, but
			// we have the ptrIdx for the bucket pointer we should
			// follow.
			parent = nextBucket;
			nextBucket = (*bptr)->getLink(ptrIdx);
		}
	} while(1); // Return is through the found and not found states above
}

// del:
// Finally, we have a delete. We start by searching down the tree for the
// object. If it is not there we return an error, otherwise we remove it
// from the bucket. If the bucket is empty, we delete the bucket from the
// recServer, patch up the neighbouring previous and next pointers and then we
// we load up the parent bucket and search for the pointer to the current
// bucket. If this is not found, this is a serious error. We continue up the
// tree in the way till a non-empty bucket is found or we reach the root.

dbError
bTree::del(object& theObject)
{

	bucket *bptr = 0;
	dbError err;
	bool found;
	long ptrIdx;
	bool first = true;

	while (1)
	{
		// Let's find the record or bucket
		// If this is the first time around, we look for the record...
		if (first)
			findInternal(theObject, head.rootBucket, found, &bptr, ptrIdx, 2);
		else
			// Otherwise we search for the bucket we just deleted
			//
			// findIndex replaces ptrIdx with the position where ptrIdx was
			// found. This is ok, since we are finished with ptrIdx once
			// it is found.
			bptr->findIndex(ptrIdx, found);
		
		// Not there. BANG!

		if (!found)
		{
			delete bptr; // Make sure we take care of our memory.

			// If this is the first time around, the object was not found,
			// otherwise there was a serious structural error in the bTree.

			return dbErr(first?db_nfound:db_err);
		}

		// We got one!! Tell the bucket to nuke the entry
		// If there's anything left in the bucket, save it and all is well

		bptr->del(ptrIdx);
		if (!bptr->empty())
		{
			writeBucket(bptr);
			delete bptr; // Take care of our buckets
			return dbErr(db_ok);
		}

		// We have to delete the current bucket that is in bptr
		// Find out who it is ...

		ptrIdx = bptr->getId(); // This is the bucket to delete.

		// If this is the root bucket, leave it there empty

		if (ptrIdx == head.rootBucket)
		{
			// If the root bucket is empty and an INNER node, make it a LEAF
			if (bptr->header()->leaf==INNER)
			{
				bptr->setActiveKeys(0);
				bptr->header()->leaf=LEAF;
			}
			writeBucket(bptr);
			delete bptr;
			return dbErr(db_ok);
		}

		// First fix up the next and prev pointers in the adjoining buckets

		if (bptr->header()->nextBucket != -1)
		{
			// There is a next bucket so fix it up

			bucket* b = fetchBucket(bptr->header()->nextBucket);
			b->header()->prevBucket = bptr->header()->prevBucket;
			writeBucket(b);
			delete b;
		}
		if (bptr->header()->prevBucket != -1)
		{
			// There is a prev bucket so fix it up

			bucket* b = fetchBucket(bptr->header()->prevBucket);
			b->header()->nextBucket = bptr->header()->nextBucket;
			writeBucket(b);
			delete b;
		}

		// Take the bucket out of the recServer

		if ((err = delRec(ptrIdx)) != db_ok)
			return dbErr(err);
		loseBucket(ptrIdx);

		// Now it's time to move up the tree and delete this entry

		first = false;

		// The next bucket is the parent of this one
		// Out of that we will delete the record pointing to ptrIdx

		// Need a temporary spot to remember what is next
		bucketId next = bptr->header()->parentId;

		// Nuke the current bucket and get the next one.
		delete bptr;
		bptr = fetchBucket(next);
	}
	
	// Never executed. Just here to make the compiler happy.
#ifndef __CC
	return dbErr(db_err);
#endif

}

// add:
// Add first checks that the key does not already exist in the btree. If it
// does it errors. Otherwise we call the internal insert routine.

dbError
bTree::add(object& newObject, long extPtr)
{
	bucket *bptr = 0;
	dbError err;
	bool found;
	long ptrIdx;

	findInternal(newObject,head.rootBucket,found,&bptr,ptrIdx,1);

	if (found) 
	{
		err = db_dup;
	}
	else
	{
		// Not found, so we now have a pointer to the bucket involved, and
		// know the pointer position just before the insertion point.
		err = insertInternal(bptr, newObject, extPtr);
	}

	// Clean up and return the appropriate error code.
	delete bptr;
	return dbErr(err);
}

#if 0
void swapKeyPtr(pKeyType a,pKeyType b)
{
	pKeyType t = a;
	a = b;
	b = t;
}
#endif

// insertInternal:
// This function has the solemn duty of inserting the actual
// index/pointer pair into the bucket. If the bucket is going to
// overflow, then a new bucket has to be created - and the contents
// split etc.
// extPtr holds the pointer half of the pair, and the indexing
// info can come out of the object itself.

dbError
bTree::insertInternal(bucket*& bptr,object &newObject,long extPtr)
{
	// This will be used below to hold temporary keys en route
	// between buckets.
	// NB we are responsible for the temporary key in outKey

	pKeyType outKey = bptr->allocTmpKey();

	// NB The contents of the key of newObject may be modified by this
	// function. So we take a copy of it here to restore later.

	pKeyType inKey = newObject.getKey(indNum);

	// Is the bucket too full to accomodate one more entry ?
	while (bptr->full())
	{
		// Overfull bucket - create a new bucket, split contents
		// between the two buckets, then migrate the median
		// object and the pointer to the NEW bucket (which contains the
		// bigger half of the values, up into the parent.

#ifdef DEBUG
		cout << "Splitting ..." << endl;
#endif

		// Make new bucket. It has the same parent and next as the current
		// bucket and the current bucket will be its prevBucket.

		headerData *h = bptr->header();
		bucket *newBucket = createNewBucket(h->leaf, h->parentId, h->nextBucket, bptr->id);
		if (!newBucket) return dbErr(db_nobuckets);

		// The current bucket's nextBucket is now the new bucket
		h->nextBucket = newBucket->id;

		// We need to move half of the elements (the bigger half)
		// across into the new bucket, chuck in the new object, and retrieve
		// an object which has both the bucket pointer and key value, for
		// passing to our superior.

		bptr->sploosh(newBucket,0,outKey,newObject,extPtr);

		// sploosh has told us what new key and id to promote up the tree.
		// We therefore set a new extPtr and modify the object's key.
		// NB This destroys the key section of the original object. It is
		// restored just before exiting this funciton below.

		extPtr = newBucket->id;
		newObject.setKey(outKey, indNum);

		// Finally, head up to the parent bucket. If we are at the root,
		// create a new root node above it.

		if (bptr->id == head.rootBucket) {
			// Hmm, we've hit the ceiling, make a new bucket
			bucket* rootBptr = createNewBucket(INNER, -1L, -1L, -1L);
			if (!rootBptr) return dbErr(db_nobuckets);

			// This is the parent of the old and new buckets
			bptr->header()->parentId = newBucket->header()->parentId = rootBptr->id;

			// Set the less than all link to the old root (bptr)
			rootBptr->setLink(0, bptr->id);
			head.rootBucket = rootBptr->id; // update root node.
			writeBucket(bptr);
			delete bptr; // We're finished with the current bucket now

			bptr = rootBptr; // We're now dealing with the root
			if (!writeData(0, (void *)&head, sizeof(bTreeHead)))
				// This is actually quite fatal!
				return dbErr(db_err);

		} else {
			// worry about the deletion of memory later - I don't
			// think fetchBucket should expect the memory to be
			// deallocated.
			// Kev: Actually fetchBucket's users are expected to delete
			// their returned buckets.

			// Move up 
			bucketId parent = bptr->header()->parentId;
			writeBucket(bptr);
			delete bptr;
			bptr = fetchBucket(parent);

			// We also need to examine the next bucket in the chain and
			// update its predecessor bucket. (if necessary)

			if (newBucket->header()->nextBucket != -1)
			{
				bucket *tmpBptr;

				// ***** Have to do something about repsonsibilty for mem allocation
				// Yeah, we are responsible.

				tmpBptr = fetchBucket(newBucket->header()->nextBucket);
				tmpBptr->header()->prevBucket = newBucket->id;
				writeBucket(tmpBptr);
				delete tmpBptr;
			}
		}
		// Make sure the newly created bucket is tucked away and deleted.
		writeBucket(newBucket);
		delete newBucket;
	};

	delete outKey; // Get rid of the temporary storage

	// Yay - something easy at last - inserting a key/pointer pair
	// which doesn't overflow a bucket.

	// Shove it into the bucket, giving the bucket a hint as to
	// where it will go.

	bptr->insert(newObject, extPtr);

	// Ok, put the key back, in case it was nuked.
	newObject.setKey(inKey, indNum);
	delete inKey; // Because getKey returns our own private copy
	
	// Write out the bucket. Note that we return the bucket intact, and do
	// not delete it.
	writeBucket(bptr);
	return dbErr(db_ok);
}

// createNewBucket:
// Open up a new record in the recServer and create a new bucket
// We don't actually put anything in the bucket or save it to the
// recServer.

bucket*
bTree::createNewBucket(e_leaf leaf, bucketId parentId, bucketId nextBucket, bucketId prevBucket)
{
	bucket*		b;
	bucketId	idx;

	// Create the record
	newRec(idx);

	// Create the bucket and return it.
	b = new bucket(idx, head.keyLen, header.recLength, leaf, parentId, nextBucket, prevBucket, indNum);
	return b;
}

// Because this bucket has been modified, we check for all used
// queries that are using this bucket and, if so, we set them as
// unsafe.

void
bTree::loseBucket(bucketId id)
{
	for (long i=0; i < MAX_QUERY; i++)
		if (query[i].used)
			if (query[i].bptr && query[i].bptr->id == id)
				query[i].unsafe = true;
}

// Write out a bucket.

void
bTree::writeBucket(bucket* bptr)
{
	if (putRec(bptr->id, bptr->getData()) != db_ok)
	{
		cerr << "writeBucket: " << bptr->id; 
		dbErr("");
		exit (1);
	}
	
	loseBucket(bptr->id);
}

void
bTree::dump(bool intonly)
{
	// Do an inorder traversal of the btree.
	bucket *b;

	b = fetchBucket(head.rootBucket);
	traverse(b, intonly);
	delete b;
}

// Dump out the whole bucket.

void
bTree::traverse(bucket* b, bool intonly)
{	
	headerData *h = b->header();
	cout << "Bucket id: " << b->id << "   " << ((h->leaf)?"LEAF":"INNER")
		 << "     " << h->activeKeys << " active keys,  parent = " << h->parentId << endl;
	b->info();
	for (long i=((h->leaf==INNER)?0:1); i<=h->activeKeys; i++)
	{
		if (h->leaf==INNER) cout << "Bucket id: " << b->id;
		if (i)
		{
			unsigned char* c = (unsigned char*)b->getKeyAddr(i);
			bool lastchar = true;
			cout << "  Key: " << hex;
			for (long j=0; j < b->keyLength; j++)

			// If intonly is set then all elements are printed as hex
			// integers, otherwise printable characters are printed out.
				if (!intonly && isprint(c[j]))
				{
					if (!lastchar)
					{
						lastchar = true;
						cout << ",";
					}
					cout << c[j];
				}
				else
				{
					if (j) cout << ",";
					cout << c[j] / 16 << c[j] % 16;
					lastchar = false;
				}
			cout << dec;
//			long l;
//			bcopy(b->getKeyAddr(i), &l, sizeof(long));
//			cout << "  Key: " << hex << l << dec;
		}
		if (h->leaf == INNER)
		{
			cout << "  Link: " << b->getLink(i) << endl;
			bucket *next = fetchBucket(b->getLink(i));
			traverse(next, intonly);
			delete next;
		}
		else
			cout << endl;
	}
}

// Verify all the pointers and the structure of the bTree

bool
bTree::checkIntegrity(void)
{
	return internalCheckIntegrity(head.rootBucket);
}

bool
bTree::internalCheckIntegrity(bucketId buck)
{
	bucket* b;
	bool res = true;

#pragma warn -pia
	if (!(b = fetchBucket(buck)))
#pragma warn .pia
	{
		cerr << "checkIntegrity: Unable to fetch bucket number " << buck;
		dbErr("");
		return false;
	}

	if (!b->header()->leaf)
	{
		for (long i=0; i<=b->header()->activeKeys; i++)
		{
			bucket* b1 = fetchBucket(b->getLink(i));
			if (b1->getId() != b->getLink(i))
			{
				cerr << "Bucket " << buck << "(" << i << ")=" << b->getLink(i) << " failed id check - returned " << b1->getId() << " instead" << endl;
				res = false;
			}
			if (i && b->getLink(i-1) != b1->header()->prevBucket)
			{
				cerr << "Bucket id " << buck << " link " << i-1 << " does not equal bucket " << b1->getId() << " previous pointer" << endl;
				res = false;
			}
			if (i<b->header()->activeKeys && b->getLink(i+1) != b1->header()->nextBucket)
			{
				cerr << "Bucket id " << buck << " link " << i+1 << " does not equal bucket " << b1->getId() << " next pointer" << endl;
				res = false;
			}
			if (!internalCheckIntegrity(b1->getId()))
			{
			//	delete b1;
				res = false;
			}
			delete b1;
		}
	}
	delete b;
	return res;
}

// Initialise the query array.

void
bTree::initQueries(void)
{
	for (long i=0; i<MAX_QUERY; query[i++].used = false);
}

// Start a new query. This sets up an empty (unsafe) query

dbError
bTree::qBegin(long& qNum)
{
	// Find an empty query.
	for (long i=0; i < MAX_QUERY && query[i].used; i++);

	// None left, too bad.
	if (i == MAX_QUERY)
		return dbErr(db_toomany, "bTree queries");
	else
	{
		qNum = i;
		query[qNum].unsafe = true;
		query[qNum].used = true;
		query[qNum].bptr = 0;
		query[qNum].key = 0;
	}
	return dbErr(db_ok);
}

// End a query, freeing appropriate memory

dbError
bTree::qEnd(long qNum)
{
	// Is this query number in range?
	if (qNum < 0 || qNum >= MAX_QUERY)
		return dbErr(db_range);

	// Is it currently active
	else if (!query[qNum].used)
		return dbErr(db_noquery);
	else
	{
		if (query[qNum].bptr)
			delete query[qNum].bptr; // Clean up the bucket
		if (query[qNum].key)
			delete query[qNum].key; // Clean up the key
		query[qNum].used = false;	// We're done with it now
		return dbErr(db_ok);
	}
}

// Is the object in the bTree?
// Eventually, this should be clever enough to somehow remember the
// position so that subsequent (inevitable) adds can save themselves a
// (potentially expensive) findInternal call.

bool
bTree::inBTree(object& theObject, long& idx)
{
	bool found;
	bucket *bptr = 0;

	findInternal(theObject, head.rootBucket, found, &bptr, idx);
	// Convert bucket link number to recServer record number
	idx = bptr->getLink(idx);
	delete bptr; // findInternal always gives back a bucket
	return found;
}

// qFind:
// This does a find on the bTree base on the findObject and updates the
// appropriate query entry.

dbError
bTree::qFind(long qNum, object& findObject, long& extPtr)
{
	bool found;
	long idx;

	// Sorry wrong one!
	if (!query[qNum].used)
		return dbErr(db_noquery);

	findInternal(findObject, head.rootBucket, found, &query[qNum].bptr, idx);

	// If it is not found then findInternal returns the position _AFTER_
	// which it should go (for add purposes). In this case, we actually want
	// to leave the query pointing to the record _after_ where it should go.
	if (!found)
		idx++;

	// Store the point in the bucket we are at and set the query as safe
	// (we can use the data in the bucket).
	query[qNum].idx = idx;
	query[qNum].unsafe = false;

	// If the object was not found, then it is possible that we have now
	// moved off the end of the bucket. If so we shouldmove to the next bucket
	// (if there is one!)

	if (idx > query[qNum].bptr->header()->activeKeys)
	{
		bucketId next = query[qNum].bptr->header()->nextBucket;
		if (next == -1) // There isn't a next one!
			return dbErr(db_eof);

		// Get the new bucket
		delete query[qNum].bptr;
		query[qNum].bptr = fetchBucket(next);

		// And we're at the beginning of it.
		query[qNum].idx = 1;
		idx = 1;
	}

	// idx shows where the entry is. Now get out the stored record number
	extPtr = query[qNum].bptr->getLink(idx);

	// If we don't have a key, get space for one.
	if (!query[qNum].key)
		query[qNum].key = query[qNum].bptr->allocTmpKey();

	// Now copy the key out of the object to here. This key is stored
	// so that when the query becomes unsafe, we can still do a search to
	// carry on from where we left off. This ensures that if records get
	// inserted around where we are, we always get current data.
	bcopy(query[qNum].bptr->getKeyAddr(idx), query[qNum].key, query[qNum].bptr->keyLength);

	// And tell the world what happened.
	return dbErr(found?db_ok:db_nfound);
}

// qPeekNext:
// This function returns the key that is next in the bTree index.
// It makes use of the data stored in the query to speed up the lookup.

dbError
bTree::qPeekNext(long qNum, object &theObject, long &extPtr)
{
	// This just makes the references to the query look shorter
	bTreeQuery *q = &query[qNum];

	// Is it in use?
	if (!q->used)
		return dbErr(db_noquery);

	// If the query is unsafe or we don't have a bucket, we have to do a
	// find
	if (q->unsafe || !q->bptr)
	{
		dbError res;

		// If we have a key, we set it in the object and do a find
		// If not, we take the first record.
		// Either of these 2 will ensure we have a valid bptr and key in
		// the query.
		if (q->key)
		{
			theObject.setKey(q->key, indNum);

			switch (res = qFind(qNum, theObject, extPtr))
			{
				case db_nfound:
				case db_ok:
				case db_eof: // effectively a nfound when empty
					break;
				default:
					cerr << "Time/space continuum anomoly calling find in qPeekNext - error code " << res << endl;
					exit (1);
			}
		}
		else
			switch (res = qFirst(qNum, theObject, extPtr))
			{
				case db_ok:
				case db_eof:
					break;
				default:
					cerr << "Time/space continuum anomoly calling first in qPeekNext - error code " << res << endl;
					exit (1);
			}
			
	}

	// We're off the edge, go to the next bucket
	if (q->idx > q->bptr->header()->activeKeys)
	{
		bucketId next = q->bptr->header()->nextBucket;
		if (next == -1)
			return dbErr(db_eof);
		delete q->bptr;
#pragma warn -pia
		if (!(q->bptr = fetchBucket(next)))
#pragma warn .pia
			return dbErr(db_err, "qPeekNext: Unable to fetch bucket ");
		q->idx = 1;
		q->unsafe = false; // Now we're safe
	}

	// idx points to where in the bucket it is.
	// extract the external record pointer and the key for return.

	extPtr = q->bptr->getLink(q->idx);
	theObject.setKey(q->bptr->getKeyAddr(q->idx), indNum);

	// If we don't have a key, get space for one.
	if (!q->key)
		q->key = q->bptr->allocTmpKey();

	// Save the key
	bcopy(q->bptr->getKeyAddr(q->idx), q->key, q->bptr->keyLength);

	return dbErr(db_ok);
}

// qNext:
// Return the next entry in the bTree based solely on the query

dbError
bTree::qNext(long qNum, object& theObject, long& extPtr)
{
	// Use qPeekNext to check what the next record is. This returns with
	// the query safe, with a bucket and key (if not eof)
	// qPeekNext checks the validity of the query.
	dbError res = qPeekNext(qNum, theObject, extPtr);

	if (res != db_ok)
		return res;

	bTreeQuery* q = &query[qNum]; // It's easier to type

	// We have used PeekNext to find out what the next one is, now advance
	// the bTree.
	q->idx++;

	// We're off the end of the bucket, so go to the next one
	if (q->idx > q->bptr->header()->activeKeys)
	{
		bucketId next = q->bptr->header()->nextBucket;
		if (next == -1)
			return dbErr(db_ok);	// NB This is not an EOF condition
									// EOF has been reached but error is
									// on next call
		delete q->bptr;
#pragma warn -pia
		if (!(q->bptr = fetchBucket(next)))
#pragma warn .pia
			return dbErr(db_err, "qNext: Unable to fetch bucket ");

		q->idx = 1;
		q->unsafe = false;
	}

	return dbErr(db_ok);
}

// qFirst:
// This returns the first key in the bTree.
// NB This function acts like a Peek. The next call to PeekNext or Next
// will return the same thing. This behaviour can be (and in dbObj, is)
// overridden by simply calling next after first.

dbError
bTree::qFirst(long qNum, object& theObject, long& extPtr)
{
	// Is it in use?
	if (!query[qNum].used)
		return dbErr(db_noquery);

	// We start at the root
	bucket* b = fetchBucket(head.rootBucket);

	// Find the leftmost leaf
	while (b && !b->header()->leaf)
	{
		bucketId bid = b->getLink(0);
		delete b;
		b = fetchBucket(bid);
	}

	if (!b)
		return dbErr(db_err, "qFirst: fell off the beginning of the world");

	// Make the query point to what we've done
	query[qNum].unsafe = false;
	query[qNum].idx = 1;
	if (query[qNum].bptr)
		delete query[qNum].bptr;
	query[qNum].bptr = b;

	// This is a very convenient way to get a look at the next record and
	//return appropriate info. Since we have ensured that the query is safe,
	//most of the dirty work in qPeekNext will be skipped over but we will
	//get back a proper error code etc.
	return qPeekNext(qNum, theObject, extPtr);
}

// qLast is analogous to qFirst except almost everything is switched
// around.

dbError
bTree::qLast(long qNum, object& theObject, long& extPtr)
{
	// Is it in use?
	if (!query[qNum].used)
		return dbErr(db_noquery);

	bucket* b = fetchBucket(head.rootBucket);

	while (b && !b->header()->leaf)
	{
		bucketId bid = b->getLink(b->header()->activeKeys);
		delete b;
		b = fetchBucket(bid);
	}

	if (!b)
		return dbErr(db_err, "qLast: fell off the end of the world");

	query[qNum].unsafe = false;
	query[qNum].idx = b->header()->activeKeys + 1;
	if (query[qNum].bptr)
		delete query[qNum].bptr;
	query[qNum].bptr = b;
	return qPeekPrev(qNum, theObject, extPtr);
}

// qPeekPrev is analogous to qPeekNext but everything is turned around

dbError
bTree::qPeekPrev(long qNum, object &theObject, long &extPtr)
{
	bTreeQuery *q = &query[qNum];
	if (!q->used)
		return dbErr(db_noquery);

	if (q->unsafe || !q->bptr)
	{
		dbError res;
		if (q->key)
		{
			theObject.setKey(q->key, indNum);

			switch (res = qFind(qNum, theObject, extPtr))
			{
				case db_nfound:
				case db_ok:
				case db_eof: // effectively a nfound when empty
					break;
				default:
					cerr << "Time/space continuum anomoly calling find in qPeekPrev - error code " << res << endl;
					exit (1);
			}
		}
		else
			qLast(qNum, theObject, extPtr);
	}
	if (q->idx <= 1)
	{
		bucketId prev = q->bptr->header()->prevBucket;
		if (prev == -1)
			return dbErr(db_eof);
		delete q->bptr;
#pragma warn -pia
		if (!(q->bptr = fetchBucket(prev)))
#pragma warn .pia
			return dbErr(db_err, "qPeekPrev: Unable to fetch bucket");

		q->idx = q->bptr->header()->activeKeys+1;
		q->unsafe = false;
	}
	extPtr = q->bptr->getLink(q->idx-1);
	theObject.setKey(q->bptr->getKeyAddr(q->idx-1), indNum);

	// If we don't have a key, get space for one.
	if (!q->key)
		q->key = q->bptr->allocTmpKey();

	// Save the key
	bcopy(q->bptr->getKeyAddr(q->idx-1), q->key, q->bptr->keyLength);

	return dbErr(db_ok);
}

// qPrev is like qNext, but oposite

dbError
bTree::qPrev(long qNum, object& theObject, long& extPtr)
{
	dbError res = qPeekPrev(qNum, theObject, extPtr);

	if (res == db_eof)
		return res;

	bTreeQuery* q = &query[qNum];

	q->idx--;

	if (q->idx <= 1)
	{
		bucketId prev = q->bptr->header()->prevBucket;
		if (prev == -1)
			return dbErr(db_ok);	// NB This is not an EOF condition
									// EOF has been reached but error is
									// on next call
		delete q->bptr;
#pragma warn -pia
		if (!(q->bptr = fetchBucket(prev)))
#pragma warn .pia
			return dbErr(db_err, "qPrev: Unable to fetch bucket");

		q->idx = q->bptr->header()->activeKeys+1;
		q->unsafe = false;
	}
	return dbErr(db_ok);
}
