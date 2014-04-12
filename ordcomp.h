/**************************************************************************
 * Source Id :
 *
 * $Id: ordcomp.h,v 1.2 1993/09/27 01:15:44 kevinl Exp $
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
 *     The comparison macros.
 *
 *  Original Author :
 *		Daz stole them from Kev and domesticated them.
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: ordcomp.h,v $
 * Revision 1.2  1993/09/27  01:15:44  kevinl
 * Changed ORD_COMPARE to use < and >
 *
 * Revision 1.1  1993/09/26  16:06:20  darrenp
 * Initial revision
 *
 **************************************************************************/

#ifndef __ORD_COMPARE_H
#define __ORD_COMPARE_H

#define ORD_COMPARE(t) \
	inline long compare_##t(pKeyType k1, pKeyType k2) \
	{ \
		t _t1 = *((t *)k1); \
		t _t2 = *((t *)k2); \
		return (_t1==_t2)?0:((_t1<_t2)?-1:1); \
	} 

#define REAL_COMPARE(t) \
	inline long compare_##t(pKeyType k1, pKeyType k2) \
	{ \
		t _t1 = *((t *)k1); \
		t _t2 = *((t *)k2); \
		return (_t1==_t2)?0:((_t1<_t2)?-1:1); \
	}

ORD_COMPARE(short);
ORD_COMPARE(ushort);
ORD_COMPARE(long);
ORD_COMPARE(ulong);
ORD_COMPARE(dateType);
ORD_COMPARE(char);
ORD_COMPARE(uniqueType);
REAL_COMPARE(float);
REAL_COMPARE(double);
REAL_COMPARE(moneyType);
#endif
