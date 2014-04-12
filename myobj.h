/**************************************************************************
 * Source Id :
 *
 * $Id: myobj.h,v 1.6 1993/03/18 04:38:22 kevinl Exp $
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
 *  Class definition for my test object.
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: myobj.h,v $
 * Revision 1.6  1993/03/18  04:38:22  kevinl
 * Changed const's to enum
 *
 * Revision 1.5  1993/03/17  14:31:16  kevinl
 * Added operators void* char*
 * Added getDataLength, compare, removed isLessThan, isEqual
 *
 * Revision 1.4  1993/02/18  22:28:30  davison
 * Added numKey and default return values
 *
 * Revision 1.3  1993/02/17  12:16:19  kevinl
 * Added isEqual
 * fixed setKey
 *
 * Revision 1.2  1993/02/13  00:56:32  kevinl
 * added setKey and new constructor
 *
 * Revision 1.1  1993/02/08  14:41:47  darrenp
 * Initial revision
 *
 **************************************************************************/

#ifndef __MYOBJ_H__
#define __MYOBJ_H__

#include "object.h"

class myObject : public object
{
	long theKey;
	char theData[11];

public:

	enum {longKey, charKey};

	inline virtual operator void*() { return (void*)theData; }
	inline virtual operator char*() { return (char*)theData; }
	inline virtual long getDataLength() { return sizeof(theKey) + sizeof(theData); };
	inline virtual long getKeyLength(long keyNum=0);
	inline virtual pKeyType getKey(long keyNum=0);
	virtual int compare(pKeyType key, long keyNum=0);
	myObject();
	myObject(pKeyType newKey, long keyNum=0);
	void setKey(pKeyType newKey, long keyNum=0);
};

#endif
