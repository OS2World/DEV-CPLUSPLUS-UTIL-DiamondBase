/**************************************************************************
 * Source Id :
 *
 * $Id: d_types.cc,v 1.8 1993/06/23 05:21:22 kevinl Exp $
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
 *	Diamond Types.
 *		Special type definitions and constant instatiations.
 *
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: d_types.cc,v $
// Revision 1.8  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.7  1993/06/20  13:43:05  kevinl
// Fixed multiple mallocs
//
// Revision 1.6  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.5  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.4  1993/04/15  04:21:52  kevinl
// Moved malloc.h
//
// Revision 1.3  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.2  1993/03/21  23:35:07  kevinl
// Changed D_* constant ints to an enum
//
// Revision 1.1  1993/03/15  19:27:48  davison
// Initial revision
//
 **************************************************************************/

#ifdef __BORLANDC__
#	include <mem.h>
#endif

#include <d_types.h>

// This stuff was going to be used to make the db file system independent,
// but has been sidelined for the time being.
/*
d_int::d_int(long val)
{
	data[0] = val&0xff;
	data[1] = val&0xff00;
	data[2] = val&0xff0000;
	data[3] = val&0xff000000;
}

d_int::d_int(const d_int& from)
{
	memcpy(data,from.data,4);
}

d_int& d_int::operator = (const d_int& from)
{
	memcpy(data,from.data,4);
	return *this;
}

d_int& d_int::operator =(const long val)
{
	data[0] = val&0xff;
	data[1] = val&0xff00;
	data[2] = val&0xff0000;
	data[3] = val&0xff000000;
	return *this;
}

d_int::operator long() const
{
	return (data[0]) + (data[1]<<8) + (data[2]<<16) + (data[3]<<32);
}


*/
