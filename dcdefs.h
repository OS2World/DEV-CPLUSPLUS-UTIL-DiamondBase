/**************************************************************************
 * Source Id :
 *
 * $Id: dcdefs.h,v 1.4 1993/11/05 13:46:57 kevinl Exp $
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
 *  Definitions shared between server and client in multi-db
 *  Original Author : daz.
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dcdefs.h,v $
 * Revision 1.4  1993/11/05  13:46:57  kevinl
 * Protocol and fixes
 *
 * Revision 1.3  1993/10/24  14:38:26  darrenp
 * SunOS fixes, big endian fixes, bug fixes.
 *
 * Revision 1.2  1993/07/21  14:25:42  daz
 * *** empty log message ***
 *
 * Revision 1.1  1993/07/04  12:31:35  daz
 * Initial revision
 *
 * Revision 1.1  1993/07/04  02:21:13  daz
 * Initial revision
 *
 **************************************************************************/

#ifndef __dcdefs_h__
#define __dcdefs_h__

#define ATTACH_CHANNEL 2
#define BASE_REQ 100000l
#define BASE_RESP 200000l
#define MAX_TRANS_AREA 4096

//
// Version information:
const char serverVersion[] = "0.0 alpha";
const long serverProtocol = 10; // Start at 10 and work upwards.

#endif
