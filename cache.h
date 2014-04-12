/*************************************************************************
 * Source Id :
 *
 * $Id: cache.h,v 1.6 1993/06/23 05:21:22 kevinl Exp $
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
 * $Log: cache.h,v $
 * Revision 1.6  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.5  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.4  1993/05/03  01:33:55  kevinl
 * whereCache not inline
 *
 * Revision 1.3  1993/04/25  10:11:47  kevinl
 * Added freeCache to allow destructing/reconstructing properly (in case)
 * Comments!
 *
 * Revision 1.2  1993/04/09  13:00:29  kevinl
 * Stats can be called from diaRel now.
 *
 * Revision 1.1  1993/04/05  00:00:08  kevinl
 * Initial revision
 *
 **************************************************************************/

#ifndef __CACHE_H
#define __CACHE_H

#include <defs.h>

const DEF_CACHE_SIZE = 10;

// A block of the cache has an id number and block of data

struct cacheBlock {
	char*	block;
	long	id;
};

// The cache class itself

class cache {
	cacheBlock*	data;		// The array of blocks
	long		cacheSize;	// The number of blocks to cache
	long		blockSize;	// How big each block is
	long		curSize;	// How many we have currently
	long		attempts;	// The stats
	long		hits;
	long		writes;
	long		dispose;

public:
		cache(long bSize, long cSize = DEF_CACHE_SIZE);
		cache(void);
		~cache();
	char*	verStr(void);
	void	createCache(long bSize, long cSize = DEF_CACHE_SIZE);
	void	freeCache(void);
	bool	inCache(long id);
	long	whereCache(long id);
	bool	getCache(char* where, long id);
	void	putCache(char* where, long id);
	void	stats(void);
};

#endif
