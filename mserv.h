/**************************************************************************
 * Source Id :
 *
 * $Id: mserv.h,v 1.9 1993/10/24 09:14:11 kevinl Exp $
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
 *  mserv.h: Header file for...
 *     A memory server. Effectively a malloc style space allocator in
 *     a file to provide variable length records.
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: mserv.h,v $
 * Revision 1.9  1993/10/24  09:14:11  kevinl
 * Fixed empty string problem
 *
 * Revision 1.8  1993/09/26  06:40:32  kevinl
 * Added dbData support
 *
 * Revision 1.7  1993/08/29  10:46:31  kevinl
 * Include file fixes
 *
 * Revision 1.6  1993/07/19  11:57:13  kevinl
 * Fixed up constant initialisation
 *
 * Revision 1.5  1993/07/11  09:42:05  kevinl
 * Changed String to dbString
 *
 * Revision 1.4  1993/07/02  05:18:37  kevinl
 * delString added
 *
 * Revision 1.3  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.2  1993/06/20  13:38:11  kevinl
 * strclass now called mystring
 *
 * Revision 1.1  1993/06/18  12:28:43  kevinl
 * Initial revision
 *
 **************************************************************************/

#include <fcntl.h>
#if !defined(__SASC)
#	if defined(__BORLANDC__) || defined(__EMX__)
#      include <io.h>
#	else
#		include <unistd.h>
#	endif
#endif
#include <iostream.h>
//#include <stdio.h>
#include <mystring.h>
#include <defs.h>
#include <dberr.h>

#define D(x) "  "#x": " << x

// The memory server is effectively a file based malloc

class memServer {
	static	const	unsigned long GRAN; // = 1<<3;
	static	const	long END_HEADER; // = 12;
							// Size of header info at front
							// (2 del pointers + lastEntry)
	static	const	long EXTRA; // = 8;
							// Amount of info needed in each block
							// (size, back)
	long	startDelChain;	// First position in delete chain
	long	endDelChain;	// Last position in delete chain
	long	lastEntry;		// Last block in chain
	long	nextFree;		// End of file
	int		fd;

	void	readHeader(void);
	void	writeHeader(void);
	void	setNextFree(void);
	void	deleteBlock(long off);
	dbError removeDel(long off);
	long	getChunk(long& newSize, long& back);
public:
	// open constructor
	memServer(const char* name);

	// empty constructor
	memServer(void) { fd = -1; }

	bool	isOpen(void) { return (fd==-1)?false:true; }
	dbError createMem(const char* name);
	dbError getString(dbData& st, long off);
	dbError putString(dbData& s, long& off);
	dbError delString(dbData& s, long& off);
	bool	checkChain(ostream& o, bool dump = false);
	bool	checkDeleteChain(ostream& o);
	inline	int mSeek(long off)
		{
			return lseek(fd, off, SEEK_SET);
		}
	inline	void mRead(long& data, long off = -1)
		{
			if (off != -1)
				mSeek(off);
			read(fd, (char*)&data, sizeof(long));
		}
	inline	void mWrite(long& data, long off = -1)
		{
			if (off != -1)
				mSeek(off);
			write(fd, (char*)&data, sizeof(long));
		}
};
