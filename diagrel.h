/**************************************************************************
 * Source Id :
 *
 * $Id: diagrel.h,v 1.7 1993/10/28 07:49:01 kevinl Exp $
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
 *
 * The diaGRel class is used to provide generalised access to relations
 * without the need for associated compiled comparison code. It effectively
 * sits on top of a diaRel class and provides comparison, load and store 
 * support for the object methods (must inherit an object base class too).
 *
 *  Original Author : Daz
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: diagrel.h,v $
 * Revision 1.7  1993/10/28  07:49:01  kevinl
 * Added diaGRel dbString/dbData support
 *
 * Revision 1.6  1993/10/18  11:33:17  kevinl
 * Added fieldNum
 *
 * Revision 1.5  1993/10/18  10:59:43  darrenp
 * Moved some things into public
 *
 * Revision 1.4  1993/10/05  07:32:11  kevinl
 * Moved all info out and into dbObj. Made all accesses to relation data
 * via dbObjData pointer.
 *
 * Revision 1.3  1993/09/28  13:04:39  kevinl
 * Can now pass data pointer to constructor
 *
 * Revision 1.2  1993/09/26  23:35:07  darrenp
 * Fixed indexedOn allocation bug, expanded test code.
 *
 */

#ifndef DIAGREL_INCL
#define DiAGREL_INCL
#include <diarel.h>
#include <idxinfo.h>

#define MFII MAX_FIELDS_IN_INDEX

//
// This class provides access to generic relations.
// It uses the diaRel class to define its interface to
// users, and an object to define its responsibility to
// the database engine. It effectively provides an interpretted
// replacement for the DSC generated comparison code.
//

class diaGRel : public diaRel {
	private:
		// ----------------------------
		// Methods in object which
		// we must override to fulfill the db engines
		// requests.
		// ----------------------------
		operator 	void*();
		virtual	void 		getData(void);
		virtual	void 		putData(void);
		virtual	void 		startData(void);
		virtual	void 		endData(void);
		pKeyType 	getKey(long keyNum=0);
		bool 		hasUnique(void);
		long		getUnique(void);
		void 		setUnique(long uniq);
		long 		getKeyLength(long keyNum=0);
		void 		setKey(pKeyType newKey, long keyNum=0);
		long 		compare(pKeyType key, long keyNum=0);
		bool 		isUnique(long keyNum=0);
		//dbObjData*	data;

		_db_strStruct*	theArray;
		dbData*		getString(long num);
		long* 		getLong(long num);
		_db_strStruct* getArray(void);

	public:
		long 		getDataLength(void); // Public so that 
										// we know how long theData is
		long		fieldNum(char* name);
		long*		uniqOffset;		// Pointer into unique in theData
		void		*theData; 		// Where we hold the record.
		long 		numStrings(void);
					diaGRel(const char *relName, void* newData = 0);
};
#endif
