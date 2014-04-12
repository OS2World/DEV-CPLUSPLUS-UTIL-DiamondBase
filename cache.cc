/*************************************************************************
 * Source Id :
 *
 * $Id: cache.cc,v 1.13 1993/06/23 05:21:22 kevinl Exp $
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
 *  Record caching for dbobj
 *
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: cache.cc,v $
// Revision 1.13  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.12  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.11  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.10  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.9  1993/05/01  14:30:47  kevinl
// added a missing bool cast
//
// Revision 1.8  1993/04/25  10:11:47  kevinl
// Added freeCache to allow destructing/reconstructing properly (in case)
// Comments!
//
// Revision 1.7  1993/04/15  04:21:52  kevinl
// Moved malloc.h
//
// Revision 1.6  1993/04/09  13:00:29  kevinl
// Stats can be called from diaRel now.
//
// Revision 1.5  1993/04/08  11:23:11  kevinl
// Fixed NaN% in destructor
//
// Revision 1.4  1993/04/08  05:26:40  kevinl
// FGixed some memory leaks
//
// Revision 1.3  1993/04/08  00:36:24  kevinl
// removed default arguments
//
// Revision 1.2  1993/04/07  06:38:57  kevinl
// Made cache stats part of DEBUG block
//
// Revision 1.1  1993/04/05  00:00:08  kevinl
// Initial revision
//
 **************************************************************************/

#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif
#include <iostream.h>
#include <cache.h>

char* cache::verStr(void)
{
	return "$Id: cache.cc,v 1.13 1993/06/23 05:21:22 kevinl Exp $";
}

// Create constructor
// Creates a cahce with cSize entries for bSize entities

cache::cache(long bSize, long cSize)
{
	createCache(bSize, cSize);
}

// Nothing constructor

cache::cache(void)
{
	data = 0;
}

// Destructor. Make sure the memory is gone

cache::~cache()
{
#ifdef DEBUG
	cout << "Cache dieing...";
	stats();
#endif
	freeCache();
}

// Frees all the cache memory

void
cache::freeCache(void)
{
	// Free the blocks allocated
	for (int i=0; i < cacheSize; i++)
		if (data[i].block)
			delete data[i].block;

	// And the array itself
	delete data;
	data = 0;
}

// Output some stats for the concerned user

void
cache::stats(void)
{
	cout << attempts << " attempts, " << hits << " hits = " << (attempts?(hits / (double)attempts * 100.00):0) << "%" << endl;
	cout << "		" << writes << " writes and " << dispose << " disposals" << endl;
}

// Let's make a cache

void
cache::createCache(long bSize, long cSize)
{
	// Are we replacing an old one?
	if (data)
		freeCache();

	// Make space for the array, setup the cache variables and init the stats
	data = new cacheBlock[cacheSize = cSize];
	blockSize = bSize;
	curSize = 0;
	attempts = 0;
	hits = 0;
	writes = 0;
	dispose = 0;
	for (int i=0; i < cacheSize; i++)
		data[i].block = 0;
}

// whereCache:
// Find an entry in the cache

long
cache::whereCache(long id)
{
	//cerr << "Searching for " << id << " in " << curSize << " entries" << endl;
	for (int i=0; i < curSize; i++)
		if (data[i].id == id)
			return i;
	return -1;
}

// inCache:
// A boolean front end to whereCache

bool
cache::inCache(long id)
{
	return (bool)(whereCache(id) != -1);
}

// getCache:
// Find an entry in the cache and if it is found, move it to the front

bool
cache::getCache(char* theData, long id)
{
	// Where is it?
	long where = whereCache(id);

	// Keep the stats
	attempts++;

	if (where == -1) 
	{
		//cerr << "Nup, not there!" << endl;
		return false;
	}

	//cerr << "Found at entry " << where << endl;

	// Found, so copy it into the data area.
	bcopy(data[where].block, theData, blockSize);

	// If the block was not at the front, make it so.
	if (where != 0)
	{
		// Save the block pointer
		cacheBlock temp = data[where];

		// Move everyone else down, thus taking up the where spot
		bcopy((char*)data, (char*)(data+1), where * sizeof(cacheBlock));

		// Put the stored one up front
		data[0] = temp;
	}

	// Stats
	hits++;
	return true;
}

// putCache:
// Write an entry into the cache. If it is already there move it to the
// front, otherwise insert it at the front.

void
cache::putCache(char* theData, long id)
{
	// We got this one?
	long where = whereCache(id);

	// Stats
	writes++;

	if (where == -1) // We don't have it.
	{
		// If the cache currently has space, make a new spot.
		if (curSize < cacheSize)
		{
			// Create a new spot on the end and put us in there.
			// where will be used to push it up later
			where = curSize++;
			data[where].block = new char[blockSize];
			data[where].id = id;
			//cerr << "Created block " << where << " with id " << id << endl;
		}
		else
		{
			// Nuke the last spot
			where = cacheSize - 1;
			data[where].id = id;
			dispose++;
			//cerr << "Putting it in block " << where << " with id " << id << endl;
		}
	}

	// where now points to the spot to put it in

	if (where != 0)
	{
		// If it's in the middle, move everybody down one and push us to 
		// the front of the cache.
		//cerr << "Moving " << where << " blocks up 1" << endl;
		cacheBlock temp = data[where];
		bcopy((char*)data, (char*)(data+1), where * sizeof(cacheBlock));
		data[where = 0] = temp;
	}

	// where will equal 0 now. copy the data in

	bcopy(theData, data[where].block, blockSize);
}
