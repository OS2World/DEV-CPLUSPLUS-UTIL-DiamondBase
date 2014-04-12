/**************************************************************************
 * Source Id :
 *
 * $Id: diarel.h,v 1.20 1993/10/24 15:05:33 darrenp Exp $
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
 * The diaRel is inherited into application classes to provide database
 * functionality.
 *
 *  Original Author : Daz
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: diarel.h,v $
 * Revision 1.20  1993/10/24  15:05:33  darrenp
 * Changed the MULTI dependent compilation parts.
 *
 * Revision 1.19  1993/10/18  11:01:26  darrenp
 * Inhibited some code when compiled with MULTI
 *
 * Revision 1.18  1993/10/05  07:33:35  kevinl
 * Now handles dbObjData
 *
 * Revision 1.17  1993/08/29  13:08:35  kevinl
 * Problems with relation assignment
 *
 * Revision 1.16  1993/07/05  05:35:41  darrenp
 * Added MULTI specific includes
 *
 * Revision 1.15  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.14  1993/05/11  14:44:50  kevinl
 * Added version number output
 *
 * Revision 1.13  1993/05/01  14:40:14  kevinl
 * Stripped ;'s in inlines
 *
 * Revision 1.12  1993/04/27  07:19:18  kevinl
 * Added write
 *
 * Revision 1.11  1993/04/26  01:20:56  kevinl
 * Comments
 *
 * Revision 1.10  1993/04/11  05:49:02  kevinl
 * Rationalised find/seek/peek methods etc
 *
 * Revision 1.9  1993/04/09  13:00:29  kevinl
 * Stats can be called from diaRel now.
 *
 * Revision 1.8  1993/04/04  23:59:17  kevinl
 * destructor to detach
 *
 * Revision 1.7  1993/04/01  04:24:26  kevinl
 * Added/changed transactions
 * Now we have get, extract and find
 *
 * Revision 1.6  1993/03/30  14:38:35  kevinl
 * Modified index passing to database transactions
 *
 * Revision 1.5  1993/03/30  07:39:26  kevinl
 * Made status public
 *
 * Revision 1.4  1993/03/29  23:27:37  kevinl
 * Now holds it's own copy of last error
 *
 * Revision 1.3  1993/03/26  05:49:24  darrenp
 * changed error codes.
 *
 * Revision 1.2  1993/03/24  06:16:38  kevinl
 * Changed constructor to remove pTransferArea
 *
 **************************************************************************/

#ifndef __DIAREL__INCL_
#define __DIAREL__INCL_

#include <object.h>
#ifdef MULTI
#  include "dclient.h"
class dbObjData;
#else
#  include "dbase.h"
#endif
#include <d_types.h>
#include <dberr.h>

char* diaVerStr(void);

class diaRel : public object { // protected really
	private:
		long	refId;		// Used for correspondance with server.
	public:
		dbObjData* objData;
		long	getRefId(void) { return refId; }
		char*	verStr(void);
		dbError status;
		virtual	~diaRel		(void);
		void	perror		(char *);
		dbError	diaErr		(dbError);
				diaRel		(const char *name);
		dbObjData* getObjData(void) { return objData; }

		// operator= must not copy the refId.
		diaRel& operator=(diaRel& X);

		// Was the last operation successful
		inline	bool	ok	() { return (bool)(status==db_ok); }

		dbError add			(void);
		dbError del			(void);
		dbError put			(void);
		dbError begin		(int idx=0);
		dbError end			(void);
		dbError get			(int idx=0);
		dbError find		(void);
		dbError seek		(void);
		dbError extract		(void);
		dbError seekFirst	(void);
		dbError first		(void);
		dbError seekLast	(void);
		dbError last		(void);
		dbError next		(void);
		dbError peekNext	(void);
		dbError peekPrev	(void);
		dbError prev		(void);
		dbError write		(void);
		void	stats		();
};
#endif
