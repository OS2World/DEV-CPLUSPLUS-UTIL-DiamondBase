/**************************************************************************
 * Source Id :
 *
 * $Id: object.h,v 1.14 1993/10/18 11:21:42 kevinl Exp $
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
 *  Virtual object base class.
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: object.h,v $
 * Revision 1.14  1993/10/18  11:21:42  kevinl
 * Added record number to object
 *
 * Revision 1.13  1993/07/13  04:45:25  kevinl
 * Added getData, putData, startData and endData for MULTI
 *
 * Revision 1.12  1993/07/11  02:22:07  kevinl
 * Nuked some int\'s and ternery operators
 *
 * Revision 1.11  1993/07/02  05:17:46  kevinl
 * {get,has,set}Unique
 * Commented out invalidate
 *
 * Revision 1.10  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.9  1993/06/20  13:37:02  kevinl
 * Includes dbstr.h, numStrings(), getArray() for String support
 *
 * Revision 1.8  1993/05/01  14:42:24  kevinl
 * Removed ;'s from inlines
 *
 * Revision 1.7  1993/04/27  07:03:13  kevinl
 * added isUnique
 *
 * Revision 1.6  1993/04/01  11:21:16  kevinl
 * Added getUniqeu to class.
 *
 * Revision 1.5  1993/03/17  14:29:37  kevinl
 * Multiple include protection
 * Valid/invalid objects
 * added !, [] and void* operators
 * added compare and rewrote isEqual, isLessThan
 *
 * Revision 1.4  1993/02/18  22:32:08  davison
 * Added in numKey and default value
 *
 * Revision 1.3  1993/02/17  12:17:05  kevinl
 * Added isEqual
 *
 * Revision 1.2  1993/02/13  00:56:53  kevinl
 * added setKey
 *
 * Revision 1.1  1993/02/08  14:41:27  darrenp
 * Initial revision
 *
 **************************************************************************/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <defs.h>
#include <dbstr.h>

class object
{
	long recNum;
//	int valid;
public:

	//object(void) { valid = 1; }
	object(void) {}
	//void invalidate(void) {valid = 0;}
	//virtual inline int operator!()
	//	{
	//		return !valid;
	//	}
	inline	long	getRecNum(void) { return recNum; }
	inline	void	setRecNum(long r) { recNum = r; }
	virtual inline operator void*() = 0;
	virtual inline void getData(void) { }
	virtual inline void putData(void) { }
	virtual inline void startData(void) { }
	virtual inline void endData(void) { }
	virtual long getDataLength(void) = 0;
	virtual pKeyType getKey(long keyNum=0) = 0;
	virtual bool hasUnique(void) = 0;
	virtual long getUnique(void) = 0;
	virtual void setUnique(long uniq) = 0;
	virtual inline pKeyType operator [](long i)
		{
			return getKey(i);
		}
	virtual long getKeyLength(long keyNum=0) = 0;
	virtual void setKey(pKeyType newKey, long keyNum=0) = 0;
	virtual long compare(pKeyType key, long keyNum=0) = 0;
	virtual bool isUnique(long keyNum=0) = 0;
	virtual long numStrings(void) = 0;
	virtual _db_strStruct* getArray(void) = 0;
	virtual inline bool isLessThan(pKeyType key, long keyNum=0)
		{
			return (compare(key, keyNum)<0)?true:false;
		}
	virtual inline bool isEqual(pKeyType key, long keyNum=0)
		{
			return (compare(key, keyNum)==0)?true:false;
		}
};

#endif
