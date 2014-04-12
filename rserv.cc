/**************************************************************************
 * Source Id :
 *
 * $Id: rserv.cc,v 1.30 1993/07/19 11:58:50 kevinl Exp $
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
 *  recServer module
 *
 *  Original Author : Darren Platt
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: rserv.cc,v $
// Revision 1.30  1993/07/19  11:58:50	kevinl
// Added a cast for bitPoolRecs
//
// Revision 1.29  1993/06/23  05:21:22	kevinl
// Mallocs are now in angular brackets
//
// Revision 1.28  1993/05/26  01:01:39	kevinl
// MALLOC_H_MISSING
//
// Revision 1.27  1993/05/25  12:59:27	kevinl
// Fixed #elif
//
// Revision 1.26  1993/05/06  04:01:10	kevinl
// SASC define for malloc.h, unistd.h is now stdio.h and BINARY mode opens
//
// Revision 1.25  1993/05/03  23:35:22	kevinl
// theData not data
//
// Revision 1.24  1993/05/01  14:43:17	kevinl
// Got rid of ints
//
// Revision 1.23  1993/04/28  14:45:52	darrenp
// Tidied up debuggin info.
//
// Revision 1.22  1993/04/28  14:42:40	darrenp
// Made delRec more paranoid - can now return db_nfound
//
// Revision 1.21  1993/04/27  06:59:47	kevinl
// Comments
//
// Revision 1.20  1993/04/15  04:21:52	kevinl
// Moved malloc.h
//
// Revision 1.19  1993/04/09  03:11:03	darrenp
// fixed a bug in db extension which made it break after 47,509 additions.
//
// Revision 1.18  1993/04/05  07:50:39	kevinl
// Put new records retrieved from file into cache
//
// Revision 1.17  1993/04/05  01:29:01	kevinl
// Rmeoved midnless debug ouput
//
// Revision 1.16  1993/04/04  23:59:57	kevinl
// Added cache support
//
// Revision 1.15  1993/03/29  08:20:09	darrenp
// Added new malloc library support
//
// Revision 1.14  1993/03/28  11:51:24	kevinl
// Modified for dbErr
//
// Revision 1.13  1993/03/28  19:08:35	davison
// Changed references to pError to dbErr().
//
// Revision 1.12  1993/03/26  06:16:38	darrenp
// standardized error codes.
//
// Revision 1.11  1993/03/24  06:22:09	kevinl
// Added better error handling code
//
// Revision 1.10  1993/03/20  14:14:00	kevinl
// Check return code of putBitPull in createDb
//
// Revision 1.9  1993/03/15  01:06:37  darrenp
// delRec added
//
 **************************************************************************/

#include <fcntl.h>
#if defined(__BORLANDC__) || defined(__SASC) || defined(__EMX__)
#   ifndef __SASC
#      include <io.h>
#   endif
#   include <stdio.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif
#include <iostream.h>
#include <rserv.h>

//
// A record server which just retrieves records from a file,
// monitoring where the free slots occur.
//
// Turn on RSERV_DEBUG for crude debugging info
//
recServer::recServer(const char *name, long offset, long cSize) : cache()
{
	commonInit();

#ifdef __BORLANDC__
	fd = open(name,O_RDWR | O_BINARY);
#elif defined(__SASC)
	fd = open((char*)name,O_RDWR | O_BINARY);
#else
	fd = open(name,O_RDWR);
#endif
	if (fd==-1) {
		// Failed to open.
		isOpen = false;
	} else {
		headerOffset = offset;
		dataOffset = headerOffset + sizeof header;

		if (headerOffset) {
			// Seek to the header position if non-zero.
			if (lseek(fd,headerOffset,SEEK_SET)!=headerOffset) {
				dbErr("Seeking to header:");
				exit(1);
			}
		}
		// Now read in the header structure.
		if (!readHeader(header)) {
			// Eeek - can't read in header.
			dbErr("Failed reading header because:");
			exit(1);
		}

		blockLength = bitPoolBytes + header.recLength * bitPoolRecs;

		// Read in the starting bitPool;
		if (!getBitPool(currentPool,indelPool)) {
			isOpen = false;
			return;
		}
		// Create a cache to speed things up
		createCache(header.recLength, cSize);
		isOpen = true;
	}
}

// Just create a recServer without opening any files

recServer::recServer(void) : cache()
{
	commonInit();
	isOpen = false;
}

recServer::~recServer()
{
	if (isOpen) close(fd);
}

// Create a new recServer file

dbError recServer::createDb(const char *name, long recLen, long offset, long cSize)
{
	// Create a new db.
	if (isOpen) return dbErr(db_alreadyopen);
	createCache(recLen, cSize);

	headerOffset = offset;
	dataOffset = headerOffset + sizeof header;

#ifdef __BORLANDC__
	fd = open(name,O_CREAT | O_RDWR | O_BINARY, 0644);
#elif defined(__SASC)
	fd = open((char*)name,O_CREAT | O_RDWR | O_BINARY, 0644);
#else
	fd = open(name,O_CREAT | O_RDWR , 0644);
#endif
	if (fd==-1) {
		// Failed to open.
		isOpen = false;
		return dbErr(db_err);
	} else {

		headerOffset = offset;

		// Setup the header details.
		header.recLength = recLen;
		header.totalRecs = bitPoolRecs;
		header.recsUsed = 0;

		blockLength = bitPoolBytes + header.recLength * bitPoolRecs;

		indelPool.zero();
		if (!putBitPool(0,indelPool)) return dbErr(db_err);

		if (!writeHeader(header)) return dbErr(db_err);

		isOpen = true;
		return dbErr(db_ok);
	}
}

dbError recServer::getRec(long idx,void *buffer)
{
	if (!isOpen) return dbErr(db_notopen);

	// Can we get it from the cache?
	if (getCache((char*)buffer, idx))
	{
//		cout << "Let's go to the video tape" << endl;
		return dbErr(db_ok);
	}

	// Not in the cache, read it in and put it in the cache

	if (lseek(fd,idx2offset(idx),SEEK_SET)!=idx2offset(idx)) return dbErr(db_err);
	if (read(fd,buffer,header.recLength)!=header.recLength) return dbErr(db_err);
	putCache((char*)buffer, idx);
	return dbErr(db_ok);
}

dbError recServer::putRec(long idx,void *buffer)
{
	if (!isOpen) return dbErr(db_notopen);

	// Save to the file and put it in the cache

	if (lseek(fd,idx2offset(idx),SEEK_SET)!=idx2offset(idx)) return dbErr(db_err);
	if (write(fd,buffer,header.recLength)!=header.recLength) return dbErr(db_err);
	putCache((char*)buffer, idx);
	return dbErr(db_ok);
}

// Open up a new spot in the recServer

dbError recServer::newRec(long &newIdx)
{
	if (!isOpen) return dbErr(db_notopen);

	long i;
	long currNumBlocks = header.totalRecs >> bitPoolBits;

	// If the db gets over 80% full, extend it.

	if (header.recsUsed > (long)(0.8*header.totalRecs)) {
		// time to build a bigger house.
		// Extend the database by 25% of its current size.

#ifdef RSERV_DEBUG
		cout << "Expanding database" << endl;
#endif
		long newNumBlocks = (long)(currNumBlocks * 1.25);
		if (newNumBlocks==currNumBlocks) newNumBlocks++;

		// Create a whole lot of zero bit blocks.
		// record where the end of the file is, since
		// that's a good place to continue insertion after extending
		// the file.
		currentPool = currNumBlocks;

		bitPool dummy;
		for(i=0;i<(newNumBlocks-currNumBlocks);i++) {
#ifdef RSERV_DEBUG
			cout << "Adding block" << endl;
#endif
			putBitPool(currNumBlocks+i,dummy);
		}
		header.totalRecs+=i*bitPoolRecs;
		// put it back on disk like a good boy - or we could
		// wait for the inevitable update below. nah.
		if (!writeHeader(header)) return dbErr(db_err);

		// we know that the pool at the end of the file is
		// empty, so don't fetch it, just use an empty one.
		indelPool.zero();
	}
	// Ok, there is enough space now.

	do { // Loop till we find an empty bitPool.
		// Is there enough space on the current bitpool.
		if (indelPool.bitsUsed<bitPoolRecs) {
			// Ok, this sucker is not full yet.

			// First find the byte where the zero bit is living.
			for(i=0;indelPool.map[i]==255;i++);
			assert(i<bitPoolRecs);

			unsigned char t=indelPool.map[i],mask = 0x80;

			// convert x to an index offset.
			// This is really psychedelic, the arrangement of >>s and //s
			unsigned long x=i << 3;
			while(t&mask) { mask >>= 1; x++; };

			t |= mask; // it was clear to start with, so set it.

			indelPool.map[i] = t; // put it back into the bitPool.

			// x is the index of the record we want into the block.

			indelPool.bitsUsed++;
			putBitPool(currentPool,indelPool);

			header.recsUsed++;
			if (!writeHeader(header)) return dbErr(db_err);

			// set the variable they passed in.
			newIdx = x + (currentPool << bitPoolBits);
#ifdef RSERV_DEBUG
			cout << "+++++ allocating bucket (" << fd << "," << newIdx
				<< ")" << endl;
#endif
			return dbErr(db_ok);
		}
		// Ok, this bitPool was full, so move to the next one.
		currentPool++;
		if (currentPool == currNumBlocks) currentPool = 0;
		if (!getBitPool(currentPool,indelPool)) return dbErr(db_err);
#ifdef RSERV_DEBUG
		cout << "Moving to bitpool #" << currentPool << endl;
#endif
	} while (1);
}

// Delete a record from the recServer

dbError recServer::delRec(long idx)
{
#ifdef RSERV_DEBUG
	cout << "+++++ attempting delete of (" << fd << "," << idx << ")" << endl;
#endif
	if (!isOpen) return dbErr(db_notopen);

	// First locate the appropriate bitPool, then the appropriate
	// bit within it, then zero it, update the number of records used
	// and update the bitPool and header.

	static char bitTable[8] = {128,64,32,16,8,4,2,1};
	currentPool = idx >> bitPoolBits;

	// Fetch that bit pool;
	if (!getBitPool(currentPool,indelPool)) return dbErr(db_err);

	// Find out which bit number to delete.
	long whichBit = idx & ((1<<bitPoolBits) -1);

	// Find out which byte that occurs in.
	long whichByte = whichBit >> 3;

	// And which bit within the byte.
	whichBit &=7;

	if (!(indelPool.map[whichByte] & bitTable[whichBit])) {
		// Blow the whistle - deleting a non-existant record.
		return dbErr(db_nfound);
	}

	// Zero that bit
	indelPool.map[whichByte] ^= bitTable[whichBit];
	indelPool.bitsUsed--;
	header.recsUsed--;

	if (!writeHeader(header)) return dbErr(db_err);
	if (!putBitPool(currentPool,indelPool)) return dbErr(db_err);
#ifdef RSERV_DEBUG
	cout << "+++++ succeeded " << endl;
#endif
	return dbErr(db_ok);
}

// Read in the header of the record storage file.

bool recServer::readHeader(rsHeader &h)
{
	// Seek to the right part of the file.
	if (lseek(fd,headerOffset,SEEK_SET)!=headerOffset) return false;

	// Now read in the header structure.
	if (read(fd,&h,sizeof h)!=(sizeof h)) {
		// Eeek - can't read in header.
		return false;
	}
	return true;
}

// Write out the header

bool recServer::writeHeader(rsHeader &h)
{
	// Seek to the right part of the file.
	if (lseek(fd,headerOffset,SEEK_SET)!=headerOffset) return false;

	// Now write the header structure.
	if (write(fd,&h,sizeof h)!=(sizeof h)) {
		// Eeek - can't write the header.
		return false;
	}
	return true;
}

// take an index and calculate where to look for it
// in the file - need to allow for the regular insertion
// of free bitpools.

long recServer::idx2offset(long idx)
{
	// every 2^bitPoolBits records, there is a bit pool of size
	// 2^(bitPoolBits / 8) to record which buckets are free in the
	// next partition.

	long whichBlock = idx >> bitPoolBits;
	long whichWithinBlock = idx & (bitPoolRecs - 1);

	return (whichBlock * blockLength) + whichWithinBlock * header.recLength
		+ dataOffset+sizeof(bitPool);
}

// Performs any initialization common to all constructors.

void recServer::commonInit(void)
{
	bitPoolBits = 12; // magic number. 4096 records between bitPools
	bitPoolRecs = (short)(1 << bitPoolBits);

	// The number of bytes in the bitPool is the number of bits
	// needed to map the following block of records, plus a short
	// to say how many are used in this block.

	bitPoolBytes = sizeof(bitPool);

	// currentPool is used to keep track of where we are inserting/
	// looking for spare buckets at the moment. It is the number of
	// the bitPool - insertions/ deletions occur from this block
	// until an operation can't be performed, and then we rotate
	// through the file until we find a suitable pool and the stop
	// there. If there is no more space in the file, then it gets
	// extended.
	currentPool = 0;

	dbErr(db_ok);
}

// fetch a particular bitpool.

bool recServer::getBitPool(long idx,bitPool &bp)
{
	if (lseek(fd,bpIdx2Offset(idx),SEEK_SET)!=bpIdx2Offset(idx)) return false;
	if (read(fd,&bp,sizeof bp)!=(sizeof bp)) return false;
	return true;
}

// Write back the bitpool.

bool recServer::putBitPool(long idx,bitPool &bp)
{
	if (lseek(fd,bpIdx2Offset(idx),SEEK_SET)!=bpIdx2Offset(idx)) return false;
	if (write(fd,&bp,sizeof bp)!=(sizeof bp)) return false;
	return true;
}

long recServer::bpIdx2Offset(long idx)
{
	return dataOffset + idx * blockLength;
}

bool recServer::readData(long offset,void *theData,long length)
{
	if (lseek(fd,offset,SEEK_SET)!=offset) return false;
	if (read(fd,theData,length)!=length) return false;
	return true;
}

bool recServer::writeData(long offset,void *theData,long length)
{
	if (lseek(fd,offset,SEEK_SET)!=offset) return false;
	if (write(fd,theData,length)!=length) return false;
	return true;
}
