/**************************************************************************
 * Source Id :
 *
 * $Id: dclient.h,v 1.5 1993/10/13 10:26:28 daz Exp $
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
 *     Defines a multi diamondBase object in the client for the
 *     multi-db
 *  Original Author : daz.
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dclient.h,v $
 * Revision 1.5  1993/10/13  10:26:28  daz
 * *** empty log message ***
 *
 * Revision 1.4  1993/07/21  14:25:42  daz
 * *** empty log message ***
 *
 * Revision 1.3  1993/07/04  16:08:03  daz
 * *** empty log message ***
 *
 * Revision 1.2  1993/07/04  13:42:19  daz
 * *** empty log message ***
 *
 * Revision 1.1  1993/07/04  02:21:13  daz
 * Initial revision
 *
 **************************************************************************/

#ifndef __mclient_h__
#define __mclient_h__

#include "defs.h"
#include "dcdefs.h"
// Message ids for communication between clients and server

// Dummy class defintion - the
// single user version allows data to be registered with the
// dbObjs - but we aren't premitting this in the multi user
// version. But, it's a defined part of the diamondBase interface
// unfortunately, so we must provide it.

class dbObjData {
	public:
};

class diamondBase
{
	public:
		enum transId {ti_add,ti_del,ti_begin,ti_end,ti_get,ti_put,ti_extract,
			ti_seekFirst,ti_first,ti_next,ti_prev,ti_seekLast,ti_last,
			ti_peekNext,ti_peekPrev,ti_seek,ti_find,ti_stats,ti_write};

		char* verStr(void);
		char* version(bool full = false);
		diamondBase(char* name = 0);
		dbError detach(long refId);
		dbError attach(const char* name, object* resBuffer, long& refId,
			dbObjData *&);
		dbError trans(long refId,transId id,long idx=0);
		~diamondBase();
	private:
		long	msqId; // Id for connection to server
		long	shmId; // Id for memory transfer to server
		long	sendChannel;
		long	recChannel;
		void	sendLong(long);
		bool	sendMess(void *,long len,long chan=-1);
		bool	recMess(void *,long len);
		char	*transArea; // Data transfer area to server.
};
#endif
