/**************************************************************************
 * Source Id :
 *
 * $Id: dberr.h,v 1.9 1993/11/05 13:46:57 kevinl Exp $
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
 *  Global error stuff for Diamond Base
 *
 *
 *  Original Author : Daz
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dberr.h,v $
 * Revision 1.9  1993/11/05  13:46:57  kevinl
 * Protocol and fixes
 *
 * Revision 1.8  1993/11/03  10:04:46  kevinl
 * Added ichar and utils.{h,cc}
 *
 * Revision 1.7  1993/08/05  11:52:08  darrenp
 * added db_refid error for MV.
 *
 * Revision 1.6  1993/07/05  05:36:19  darrenp
 * Added db_nomem and db_comms for the multi version.
 *
 * Revision 1.5  1993/04/08  01:21:11  kevinl
 * includes string.h now
 *
 * Revision 1.4  1993/03/30  14:37:48  kevinl
 * Modified error handling again. now we can get a string back (if we want)
 *
 * Revision 1.3  1993/03/29  23:27:06  kevinl
 * Support for diaRel's error handling
 *
 * Revision 1.2  1993/03/28  10:39:01  kevinl
 * Added in a title and the dbErr function declarations.
 *
 * Revision 1.1  1993/03/26  06:16:38  darrenp
 * Initial revision
 *
 *
 **************************************************************************/

#ifndef __DB_ERR__
#define __DB_ERR__

enum dbError {
	db_ok,
	db_alreadyregistered,
	db_unimp,
	db_nfound,
	db_range,
	db_toomany,
	db_noquery,
	db_querylocked,
	db_reclocked,
	db_dup,
	db_badName,
	db_notopen,
	db_err,
	db_alreadyopen,
	db_locked,
	db_unlocked,
	db_nobuckets,
	db_eof,
	db_nomem,
	db_comm,
	db_refid,
	db_protocol
};

dbError dbErr(dbError, char* extra = 0);
void dbErr(char *, dbError err);
void dbErr(char *);
char* dbErrStr(dbError err);

#endif
