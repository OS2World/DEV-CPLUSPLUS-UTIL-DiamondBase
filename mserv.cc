/**************************************************************************
 * Source Id :
 *
 * $Id: mserv.cc,v 1.10 1993/10/24 09:14:11 kevinl Exp $
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
 *  mserv.cc:
 *     A memory server. Effectively a malloc style space allocator in
 *     a file to provide variable length records.
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: mserv.cc,v $
// Revision 1.10  1993/10/24  09:14:11  kevinl
// Fixed empty string problem
//
// Revision 1.9  1993/10/19  11:25:06  kevinl
// Removed use of operator=(int) on dbData's
//
// Revision 1.8  1993/09/26  06:40:32  kevinl
// Added dbData support
//
// Revision 1.7  1993/08/29  10:46:26  kevinl
// A small assertion
//
// Revision 1.6  1993/07/19  11:57:13  kevinl
// Fixed up constant initialisation
//
// Revision 1.5  1993/07/11  09:42:05  kevinl
// Changed String to dbString
//
// Revision 1.4  1993/07/02  05:18:37  kevinl
// delString added
//
// Revision 1.3  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.2  1993/06/20  13:40:17  kevinl
// Fixed multiple mallocs
// Added extra error strings to some returns
// Size/back in wrong order in getString. Also reduce size by 2 longs in getString
// Removed extra char* in getString. Now manipulate String directly
// writes fill in padding so last record will read properly
//
// Revision 1.1  1993/06/18  12:28:43  kevinl
// Initial revision
//
 **************************************************************************/

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <mserv.h>

const unsigned long memServer::GRAN = 1<<3;
const long memServer::END_HEADER = 12;
const long memServer::EXTRA = 8;

void
memServer::readHeader(void)
{
	mRead(startDelChain, 0);
	mRead(endDelChain);
	mRead(lastEntry);
}

void
memServer::writeHeader(void)
{
	mWrite(startDelChain, 0);
	mWrite(endDelChain);
	mWrite(lastEntry);
}

void
memServer::setNextFree(void)
{
	if (lastEntry)
	{
		// Read in the size of the last entry
		mRead(nextFree, lastEntry);
		if (nextFree < 0) nextFree = -nextFree;
		nextFree+=lastEntry;
	}
	else
		nextFree = (END_HEADER + GRAN - 1) & ~(GRAN-1);
}

memServer::memServer(const char* name)
{
#ifdef __BORLANDC__
	fd = open(name,O_RDWR | O_BINARY);
#elif defined(__SASC)
	fd = open((char*)name,O_RDWR | O_BINARY);
#else
	fd = open(name,O_RDWR);
#endif

	if (fd==-1) return;
	readHeader();
	setNextFree();
}

dbError
memServer::createMem(const char* name)
{
#ifdef __BORLANDC__
    fd = open(name,O_CREAT | O_RDWR | O_BINARY, 0644);
#elif defined(__SASC)
    fd = open((char*)name,O_CREAT | O_RDWR | O_BINARY, 0644);
#else
    fd = open(name,O_CREAT | O_RDWR | O_TRUNC, 0644);
#endif
    if (fd==-1)
        return dbErr(db_err);

	startDelChain = 0;
	endDelChain = 0;
	lastEntry = 0;
	writeHeader();
	setNextFree();
	return dbErr(db_ok);
}
	
dbError
memServer::getString(dbData& st, long off)
{
	// If the string doesn't have an offset it is meant to be null 

	if (off == 0) {
		st.dispose();
		return dbErr(db_ok);
	}

	if (fd == -1)
		return dbErr(db_notopen);

	if ((off < END_HEADER) || (off & (GRAN-1)))
		return dbErr(db_range, "Granularity problem"); // Must be on multiple of granularity

	if (off > nextFree)
		return dbErr(db_eof, "Offset too high"); // Can't be off the end

	if (mSeek(off) != off)
		return dbErr(db_eof, "Seek error");

	long size; mRead(size); size -= 2*sizeof(long);
	long back; mRead(back);

	if (size == 0) {
		st.dispose();
		return dbErr(db_ok);
	}

#if DEBUG
	cout << "getString request: " << off << D(lastEntry) << D(nextFree) << D(size) << endl;
#endif
	st.setSize(size+1);
	if (read(fd, (char*)st, size) != size)
	{
#if DEBUG
		cout << "Actually managed to read " << k << endl;
#endif
		return dbErr(db_eof, "Problem reading from file");
	}

	return dbErr(db_ok);
}

// Remove the deleted segment at off from the delete chain

dbError
memServer::removeDel(long off)
{
	if (mSeek(off) != off)
		return dbErr(db_eof);
	long size; mRead(size);
	long back; mRead(back);
	long prev; mRead(prev);
	long next; mRead(next);
	if (prev == 0)
	{
		startDelChain = next;
		writeHeader();
	}
	else
	{
		// change the 'next delete' pointer of the previous deleted block
		mWrite(next, prev+(3*sizeof(long)));
	}

	if (next == 0)
	{
		endDelChain = prev;
		writeHeader();
	}
	else
	{
		// change the 'prev delete' pointer of the next deleted block
		mWrite(prev, next+2*sizeof(long));
	}
	return dbErr(db_ok);
}

// Find a block of size size or make a new one

long
memServer::getChunk(long& newSize, long& back)
{
	long delSize;
	long cur = startDelChain;
	newSize = (newSize+GRAN-1) & ~(GRAN-1);
	long actualSize = newSize+EXTRA;
#if DEBUG
	cout << "getChunk:" << D(newSize) << D(actualSize) << endl;
#endif
	while (cur)
	{
		mRead(delSize, cur);
		mRead(back);
#if DEBUG
		cout << "getChunk:" << D(cur) << D(delSize) << D(back) << endl;
#endif
		if (delSize <= -actualSize) // Got one!
		{
			// Remove the block form the delete chain
			removeDel(cur);

			// If this block was too big, make a smaller block
			// and then delete it
#if DEBUG
		cout << "got one:" << D(actualSize) << D(delSize) << D(-2*EXTRA) << endl;
#endif
			// Does the new block leave enough for a deleted bit on the end?

			if (actualSize + delSize <= -2*EXTRA)
			{
				long extraSize = -delSize-actualSize;
				long where = cur+actualSize;
				mWrite(actualSize, cur);
				mWrite(extraSize, where);
				mWrite(cur); // set the back pointer
				if (cur == lastEntry)
				{
					lastEntry = where;
					nextFree = cur-delSize;
				}
				else
				{
					mWrite(where, cur-delSize+sizeof(long)); // set back of next
				}
				deleteBlock(where);
			}
			else
			{ // we have to increase newSize a bit (max 2*EXTRA)
				actualSize = -delSize;
				newSize = actualSize-EXTRA;
			}
			return cur;
		}
		mRead(cur, cur+3*sizeof(long));
	}

#if DEBUG
	cout << "getChunk:" << D(lastEntry) << D(nextFree) << D(cur) << endl;
#endif
	// We have to make some more space at the end
	back=lastEntry;
	cur=lastEntry=nextFree;
	nextFree+=actualSize;
	writeHeader();
#if DEBUG
	cout << "getChunk:" << D(lastEntry) << D(nextFree) << D(cur) << endl;
#endif
	return cur;
}

dbError
memServer::delString(dbData& s, long& off)
{
	if (off == 0) {
		s.dispose();
		return dbErr(db_ok);
	}

	dbError err = getString(s, off);
	if (err != db_ok)
		return err;

	deleteBlock(off);
	return dbErr(db_ok);
}

void
memServer::deleteBlock(long off)
{
	// Find the newly nuked block and get the size and back pointer
	long size; mRead(size, off);
	long back; mRead(back);
	long done = 0;

	if (back) // Let's see if it is also deleted
	{
		long backSize; mRead(backSize, back);
		if (backSize < 0) // It's deleted
		{
			// Fix the size of the previous block
			backSize -= size;
			mWrite(backSize, back);

			// Fix the back pointer of the next block
			mWrite(back, off+size+sizeof(long));
			done = 1;
			off = back;
			size = -backSize;
		}
	}

	if (off != lastEntry)
	{
		long next = off + size;
		long nextSize; mRead(nextSize, next);
#if DEBUG
		cout << "Check next" << D(next) << D(nextSize) << endl;
#endif
		if (nextSize < 0)
		{ // We have a deleted block
			// Remove it from the deleted chain
			removeDel(next);

			// Fix the backward pointer of the next block
			if (lastEntry == next)
			{
				// The next block was the last, the new one is now
				lastEntry = off;
			}
			else
			{
				// Fix the next block's back pointer
#ifdef DEBUG
				cout << "Fixing back" << D(off) << D(off+size-nextSize+sizeof(long)) << endl;
#endif
				mWrite(off, off + size - nextSize + sizeof(long));
			}
			
			// Fix the size of the deleted block
			size = -size + nextSize;
			if (done)
			{
				// We're dealing with a deleted block
				mWrite(size, off);
			}
			else
			{
				// Need to delete it below
				size = -size;
			}
		}
	}

	if (!done)
	{
		// If there is already a chain...
		if (endDelChain)
		{
			// Find end of del chain + size & back &prev
			// Previous pointer stays the same

			// Next pointer is now the new block
			mWrite(off, endDelChain+3*sizeof(long));
		}

		size = -size;
		// put it back negative
		mWrite(size, off);

		// Prev is the old endDelChain
		mWrite(endDelChain, off+2*sizeof(long));
		// Next is 0
		long next=0; mWrite(next);

		if (!startDelChain)
			startDelChain=off;
		endDelChain=off;
		writeHeader();
	}
}

dbError
memServer::putString(dbData& s, long& off)
{
	if (fd == -1)
		return dbErr(db_notopen);

	// Must be on multiple of granularity (can be 0)
	if (off && (off < END_HEADER) || (off & (GRAN-1)))
		return dbErr(db_range, "Bad offset");

	long len = s.len();

	// If the string is null length. It should not be added in and the off
	// should become 0. Before doing that however, we delete the string
	// that was there.
	if (len < 1)
		return delString(s, off);

	if (off > nextFree)
		return dbErr(db_eof); // Can't be off the end
	
	if (off)
		if (mSeek(off) != off)
			return dbErr(db_eof);

	// no -1 on length so that we have space for the 0
	long newSize = (len + GRAN) & ~(GRAN-1);
	long actualSize = newSize+EXTRA;
	long size = 0;
	long back = 0;
	if (off)
	{
		mRead(size);
		mRead(back);
	}

	long newoff = off;

	if (actualSize > size)
	{
#if DEBUG
		cout << "It's too big!" D(actualSize) << D(size) << endl;
#endif
		bool gotit = false;
		long next = off + size;
		if (off && off == lastEntry) // Just extend it!
		{
#if DEBUG
			cout << "Extending last entry" << endl;
#endif
			newoff = off;
			gotit = true;
			nextFree+=actualSize-size;
		}
		if (off && !gotit && next < nextFree) // We're in range
		{
#if DEBUG
			cout << "Checking next block at " << next << D(nextFree) << endl;
#endif
			// Is the next block free and big enough?
			if (mSeek(next) != next)
				return dbErr(db_err);
			long delsize; mRead(delsize);
#if DEBUG
			cout << "next block sizes:" << D(size) << D(delsize) << D(size-delsize) << D(actualSize) << endl;
#endif
			if (size-delsize >= actualSize)
			{
#if DEBUG
				cout << "...Got one" << D(delsize) << D(newSize) << D(-actualSize) << endl;
#endif
				newoff = off;
				actualSize = size-delsize;
				gotit = true;
				removeDel(next);
#if DEBUG
				cout << "Fix back of next" << D(off) << D(actualSize) << endl;
#endif
				mWrite(off, off+actualSize+sizeof(long));
			}
		}
		if (off && !gotit && back >= END_HEADER) // We're in backward range
		{
#if DEBUG
			cout << "Checking previous block" << endl;
#endif
			if (mSeek(back) != back)
				return dbErr(db_err);
			long delsize; mRead(delsize);
#if DEBUG
			cout << "previous block sizes:" << D(size) << D(delsize) << D(size-delsize) << D(actualSize) << endl;
#endif
			if (size-delsize >= actualSize)
			{
#if DEBUG
				cout << "...Got one" << endl;
#endif
				gotit = true;

				// New position is previous block
				newoff = back;

				// And get new previous
				mRead(back);

				// Put this block number into next blocks previous because
				// we are joing the two blocks.
				mWrite(newoff, next+sizeof(long));

				off = 0; // So we don't delete it!
				removeDel(newoff);
#if DEBUG
				cout << "removing:" << D(newoff) << D(back) << endl;
#endif
				actualSize = size-delsize;
			}
		}
		if (!gotit)
		{
#if DEBUG
			cout << "Get a new chunk then" << endl;
#endif
			newoff = getChunk(newSize, back);
#if DEBUG
			if (actualSize != newSize+EXTRA)
				cout << "Sizes changed" << D(actualSize) << D(newSize) << endl;
#endif
			actualSize = newSize+EXTRA; // In case newSize changes
		}
	}
	//
	// Is it shorter than the block and thus leaves some space for a
	// deleted block?
	//
	else if (off && (size - actualSize > 2*EXTRA))
	{
#if DEBUG
		cout << "Making a smaller deleted block" << endl;
#endif
		long extraSize = size - actualSize;
		long where = off+actualSize;
		mWrite(extraSize, where);
		mWrite(newoff); // set the back pointer
		if (lastEntry == off)
		{
			lastEntry = where;
			nextFree = newoff+size;
		}
		else
		{
			// set back of next
			mWrite(where, newoff+size+sizeof(long));
		}
		deleteBlock(where);
	}
	else
	{
#if DEBUG
		cout << "Leaving exactly as it was! Will use size" << D(actualSize) << D(size) << endl;
#endif
		actualSize = size;
	}

#if DEBUG
	cout << "Selected a spot" << D(newoff) << D(back) << D(actualSize) << endl;
#endif
	// OK, now newoff points where we want and it is big enough.
	mWrite(actualSize, newoff);
	mWrite(back);

	// This is causing problems at the end of the file. We should add
	// In the padding space so that the last entry is big enough.
	//write(fd, (char*)s, len+1);
	write(fd, (char*)s, newSize);

	if (off && (newoff != off)) // It's moved
	{
#if DEBUG
		cout << "Deleting old block" << D(off) << endl;
#endif
		deleteBlock(off);
	}

	off = newoff;
	return dbErr(db_ok);
};

bool
memServer::checkChain(ostream& o, bool dump)
{
	char s[20];
	if (dump)
		o << D(startDelChain) << D(endDelChain) << D(lastEntry) << D(nextFree) << endl;
	long cur = (END_HEADER + GRAN - 1) & ~(GRAN-1);
	long last = 0;

	while (cur < nextFree)
	{
		long next = cur;
		long size; mRead(size, cur);
		long back; mRead(back);
		if (dump)
			o << D(cur) << D(size) << D(back);
		if (size < 0)
		{
			long prevDel; mRead(prevDel);
			long nextDel; mRead(nextDel);
			if (dump)
				o << D(prevDel) << D(nextDel);
			if (prevDel < 0 || prevDel > lastEntry || nextDel < 0 || nextDel > lastEntry)
			{
				o << "Bad Del pointers" << endl;
				return false;
			}
			next-=size;
		}
		else
		{
			read(fd, s, (size<10)?size:10);
			if (dump)
			{
				s[(size<10)?size:10]=0;
				o << "  " << s;
			}
			next+=size;
		}
		if (dump)
			o << endl;
		if (back != last)
		{
			o << "Back pointer mismatch" << endl;
			return false;
		}
		last = cur;
		cur = next;
	}
	return true;
}

bool
memServer::checkDeleteChain(ostream& o)
{
	// Check that the delete chain makes sense.
	// This does not verify that there are no lost deleted entries

	long cur = startDelChain;
	long last = 0;

	while (cur)
	{
		long size; mRead(size, cur);
		long back; mRead(back);
		long prev; mRead(prev);
		long next; mRead(next);

		if (prev != last)
		{
			o << "Deleted Block " << cur << D(prev) << " !=" << D(last) << endl;
			return false;
		}

		last  = cur;
		cur = next;
	}

	if (last != endDelChain)
	{
		o << "Last block " << last << " !=" << D(endDelChain) << endl;
		return false;
	}

	return true;
}
