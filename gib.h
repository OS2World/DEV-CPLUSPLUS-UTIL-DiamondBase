/**************************************************************************
 * Source Id :
 *
 * $Id: gib.h,v 1.10 1993/11/05 13:46:57 kevinl Exp $
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
 *  Header for gib.cc
 *
 *  Original Author : Darren Platt
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: gib.h,v $
 * Revision 1.10  1993/11/05  13:46:57  kevinl
 * Protocol and fixes
 *
 * Revision 1.9  1993/11/05  12:45:05  kevinl
 * Fixed problems that CC picked up (and some multi bugs)
 *
 * Revision 1.8  1993/11/04  15:10:53  kevinl
 * recMess now has 3 state return
 * Alarm set to clean up dead clients
 * creat replaces open
 * More signal handling
 * We now do the shared memory creation/removal
 * Removed fork
 *
 * Revision 1.7  1993/11/03  12:41:54  darrenp
 * Added sigterm shutdown facilitites.
 *
 * Revision 1.6  1993/10/31  17:22:11  darrenp
 * Fix to returnErr to shut up compiler.
 *
 * Revision 1.5  1993/10/29  15:54:37  darrenp
 * bug fixes in memorary attachment.
 *
 * Revision 1.4  1993/10/13  10:26:28  daz
 * *** empty log message ***
 *
 * Revision 1.3  1993/07/21  14:25:42  daz
 * *** empty log message ***
 *
 * Revision 1.2  1993/07/04  16:08:03  daz
 * *** empty log message ***
 *
 * Revision 1.1  1993/07/04  02:21:13  daz
 * Initial revision
 *
 * Revision 1.1  1993/07/04  02:21:13  daz
 * Initial revision
 *
 **************************************************************************/

#ifndef __gib_incl__
#define __gib_incl__


#include "diagrel.h"

const SLEEP_TIME = 60;
enum rcvRes {rcvOK, rcvINTR, rcvERR};

//
// Class for holding all the data pertaining to a single
// client. We will hold the data in a linked list for the moment.
//

class clientInfo {
	public:
		long	pid;		// Their process id
		long	shmId;		// Shared memory id
		char	*transArea;	// Their designated transfer area.
		clientInfo *next;
		clientInfo *prev;
};

// A fakeObject is instantiated in the server to provide all the
// services an object would normally provide - it cheats now that the
// diaGRel object is available. It used to pass info to the client to
// do the work, but now the diaGRel does most of the work, and the
// fakeObject just holds a little extra info.

class fakeObject : public diaGRel {
	private:
//		bool 	tradeLongs(char code,long send,long &rec);

				// We are interested in the redefining the following two
				// functions in order to detect when to transfer memory.
//		void 	getData(void);
//		void 	putData(void);
	public:
				fakeObject(const char *relName);
		long	client;
		char	*transArea; // copy of that application's transfer area
};

// Use fakenodes to keep a list of objects that are
// currently being mirrored in the server.
// Each fake node has a pointer to the node which it is faking,
// the client's pid and the refId for that object.

struct fakeNode {
	fakeObject	*obj;
	long		client; // equals pid.
	long		refId;
	fakeNode	*next; // Maintained in a list
	fakeNode	*prev;
};

// Prototypes.
bool sendMess(void *data,long len,long chan);
rcvRes recMess(void *data,long len,long chan);
void returnErr(long e,long client);

#endif
