/**************************************************************************
 * Source Id :
 *
 * $Id: rserv.h,v 1.15 1993/06/23 05:21:22 kevinl Exp $
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
 *  recServer module
 *
 *  Original Author : Darren Platt
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: rserv.h,v $
 * Revision 1.15  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.14  1993/05/03  23:34:29  kevinl
 * writeData and readData use theData instead of data
 *
 * Revision 1.13  1993/05/01  14:43:11  kevinl
 * Removed ;'s from inlines
 *
 * Revision 1.12  1993/04/28  11:29:22  kevinl
 * Fixing up some inlines
 *
 * Revision 1.11  1993/04/27  06:59:47  kevinl
 * Comments
 *
 * Revision 1.10  1993/04/04  23:59:57  kevinl
 * Added cache support
 *
 * Revision 1.9  1993/03/28  11:51:24  kevinl
 * Modified for dbErr
 *
 * Revision 1.8  1993/03/28  19:09:22  davison
 * deleted pError member.
 *
 * Revision 1.7  1993/03/26  06:16:38  darrenp
 * standardized error codes.
 *
 * Revision 1.6  1993/03/24  06:21:47  kevinl
 * Added error handling code
 *
 * Revision 1.5  1993/03/11  06:22:33  kevinl
 * Added $ onto $Id and $Log in header
 *
 **************************************************************************/

#ifndef RSERV_INC_
#define RSERV_INC_

#include <defs.h>
#include <cache.h>
#include <memory.h>

// Default number of entries in the cache

const DEF_RS_CACHE_SIZE = 10;
struct rsHeader;

// the bitPool is a bitmap of used/unused records

struct bitPool {
	short	bitsUsed;
	unsigned	char	map[512];
	inline void	zero(void) {
				// warning - method unfriendly.
				// memset(&dummy,0,sizeof dummy);
				// Yet again... tut tut tut
				memset(map, 0, sizeof map);
				bitsUsed = 0;
			}
		inline bitPool(void) { zero();}
};

struct rsHeader {
	long	recLength;
	long	totalRecs;
	long	recsUsed;
};

class recServer : public cache {
	public:
//		dbError status;
//		inline dbError rserror(dbError err) { return (status = err); };

		recServer(const char *name,long offset=0, long cSize = DEF_RS_CACHE_SIZE);
		recServer(void);
		~recServer();
		dbError	createDb(const char *name,long recLen,long offset=0, long cSize = DEF_RS_CACHE_SIZE);

		dbError getRec(long idx,void *buffer);
		dbError putRec(long idx,void *buffer);
		dbError newRec(long &newIdx);
		dbError delRec(long idx);
		bool 	writeData(long offset,void *theData,long length);
		bool	readData(long offset,void *theData,long length);
		bool	isOpen;
	private:
		int		fd;
		void	commonInit(void);
		bool	getBitPool(long idx,bitPool &bp);
		bool	putBitPool(long idx,bitPool &bp);
		short	bitPoolBits; // how many bits stored in
		long	currentPool;
		short	bitPoolBytes;
		short	bitPoolRecs; // 2^bitPoolRecs
		long	idx2offset(long idx);
		long	bpIdx2Offset(long idx);
		bool	readHeader(rsHeader &);
		bool	writeHeader(rsHeader &);
		long	headerOffset;
		long	dataOffset;
		long	blockLength; // Length of one pool/block of records.
		bitPool	indelPool;
	protected:
		rsHeader header;
};
#endif
