/**************************************************************************
 * Source Id :
 *
 * $Id: diagrel.cc,v 1.12 1993/11/03 12:21:12 davison Exp $
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
 * $Log: diagrel.cc,v $
// Revision 1.12  1993/11/03  12:21:12  davison
// Ahh - maybe I cant count parenthesis ?
//
// Revision 1.11  1993/11/03  12:17:27  davison
// Made implicit boolean tests explicit in comparison functions.
//
// Revision 1.10  1993/11/03  11:05:19  kevinl
// Missing char* cast on theData
//
// Revision 1.9  1993/11/03  10:04:46  kevinl
// Added ichar and utils.{h,cc}
//
// Revision 1.8  1993/10/31  16:25:15  darrenp
// uniqueOffset not set correctly when no unique present - and
// has unique failed, and writing to unique gave wierd results.
//
// Revision 1.7  1993/10/28  07:49:01  kevinl
// Added diaGRel dbString/dbData support
//
// Revision 1.6  1993/10/18  11:38:31  kevinl
// Added 0 on end of char array fields
// Added fieldNum
//
// Revision 1.5  1993/10/05  07:32:11  kevinl
// Moved all info out and into dbObj. Made all accesses to relation data
// via dbObjData pointer.
//
// Revision 1.4  1993/09/28  13:03:43  kevinl
// Moved some includes, fixed the extern diamondBase
// Added UNIQUE to some of the assignment functions
// Added the result check to the strncmp in compare D_CHAR
//
// Revision 1.3  1993/09/27  05:22:25  darrenp
// Removed test code.
//
// Revision 1.1  1993/09/26  16:06:20  darrenp
// Initial revision
//
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif

#include <diagrel.h>
#include <ordcomp.h>

//
// Now for my first trick, I will attempt to
// examine the header of a db file.

extern diamondBase theDiamondBase;

#ifdef DIAGREL_TEST

struct dnaTagRel {
	long	length;
	long	id;
	long	experiment;
	char	name[10];
	char 	circular;

{

	for(testGRel.first();testGRel.ok();testGRel.next()) {
		dnaTagRel *d = (dnaTagRel *)testGRel.theData;			
		cout << "id         =" << d->id << endl;
		cout << "circular   =" << d->circular << endl;
		cout << "------------------------------" << endl;
	}
	testGRel.end();
}
#endif
//
// Implementation of the diaGRel methods.
// ---

diaGRel::diaGRel(const char *relName, void* newData) : diaRel(relName)
{
	if (status != db_ok)
		return;

	// Allocate a transfer area or use a given one.
	if (newData)
		theData = newData;
	else
		theData = new char[objData->dataLength];

	if (objData->uniqOff != -1) {
		uniqOffset = (long *)((char *)theData+objData->uniqOff);
	} else {
		uniqOffset = 0;
	}

	if (numStrings()) 
	{
		theArray = new _db_strStruct[numStrings()];
		for (int i=0; i<numStrings(); i++)
		{
			if (objData->fieldType[objData->stringField[i]] == D_STRING)
				theArray[i].theString = new dbString;
			else
				theArray[i].theString = new dbData;
			theArray[i].theLong = (long*)((char*)theData+objData->fieldOffset[objData->stringField[i]]);
		}
	}
	else
		theArray = 0;
}	

diaGRel::operator void*()
{
	return theData;
}

// These are just cues to the person using the
// database, that they should have the data ready for fetching.
// This is to make inter process transfer efficient when the engine
// and the application aren't the same process. Don't worry about them
// for now.

void diaGRel::getData(void)
{
}
void diaGRel::putData(void)
{
}
void diaGRel::startData(void)
{
}
void diaGRel::endData(void)
{
}
long diaGRel::getDataLength(void)
{
	return objData->dataLength;
}
pKeyType diaGRel::getKey(long keyNum)
{
	// Retrieve the data associated with keyNum.
	char *data = new char[objData->keyLength[keyNum]];
	long i,j;
	j = 0;

	// Copy the
	for(i=0;i<MAX_FIELDS_IN_INDEX;i++) {
		long k = objData->indexedOnP[keyNum][i];
		if (k==-1) break;

		// Copy each field of the record according to
		// its type.
		// Switch statement is probably overkill for this kind of
		// situation.
		switch(objData->fieldType[k]) {
			case D_UNIQUE:
			case D_STRING:
			case D_DATA:
			case D_LONG:
			case D_ULONG:
			case D_DOUBLE:
			case D_MONEY:
			case D_DATE:
			case D_SHORT:
			case D_USHORT:
			case D_FLOAT:
				// For most things, just copy a long across.
				// It might not be correct for some of the above
				// data types, but we don't use them - I think ?
				*(long *)(data+j) = *(long *)((char *)theData + objData->fieldOffset[k]);
				break;
			case D_CHAR:
			case D_ICHAR:
				strncpy(data+j,((char*)theData+objData->fieldOffset[k]),objData->fieldLength[k]);
				*(data+j+objData->fieldLength[k]-1) = 0;
				break;
			default:
				cerr << "Illegal field type" << endl;
				assert(0);
		}
		j+=objData->fieldLength[k];
	}
	return data;
}
bool diaGRel::hasUnique(void)
{
	return (uniqOffset!=0)?true:false;
}
long diaGRel::getUnique(void)
{
	return *uniqOffset;
}
void diaGRel::setUnique(long uniq)
{
	*uniqOffset = uniq;
}
long diaGRel::getKeyLength(long keyNum)
{
	if (keyNum==-1) return 0;
	return objData->keyLength[keyNum];
}

void diaGRel::setKey(pKeyType newKey, long keyNum)
{
	// Unpack a key that is given to us.
	long i,j;
	j = 0;

	for(i=0;i<MAX_FIELDS_IN_INDEX;i++) {
		long k = objData->indexedOnP[keyNum][i];
		if (k==-1) break;

		// Copy each field of the record according to
		// its type.
		// Switch statement is probably overkill for this kind of
		// situation.
		switch(objData->fieldType[k]) {
			case D_UNIQUE:
			case D_STRING:
			case D_DATA:
			case D_LONG:
			case D_ULONG:
			case D_DOUBLE:
			case D_MONEY:
			case D_DATE:
			case D_SHORT:
			case D_USHORT:
			case D_FLOAT:
				// For most things, just copy a long across.
				// It might not be correct for some of the above
				// data types, but we don't use them - I think ?
				*(long *)((char *)theData + objData->fieldOffset[k]) =
					*(long *)(newKey+j);
				break;
			case D_CHAR:
			case D_ICHAR:
				strncpy(
					(char *)theData+objData->fieldOffset[k],
					newKey + j,objData->fieldLength[k]);
				*((char*)theData+objData->fieldOffset[k]+objData->fieldLength[k]-1) = 0;
				break;
			default:
				cerr << "Illegal field type" << endl;
				assert(0);
		}
		j+=objData->fieldLength[k];
	}
}

long diaGRel::compare(pKeyType key, long keyNum)
{
	// This is the nasty one which does a lot of hard work.
	// It performs the necessary comparisons on the necessary
	// keys. The actual fields which are to be compared come from
	// various arrays.

	// Retrieve the data associated with keyNum.
	long i,j,k,res;

	// Compare each field in turn - as soon as they
	// differ, return +1 or -1 - if they match all the
	// way down, return 0.

	// j tracks the index into the packed key supplied.
	j = 0;
	for(i=0;i<MAX_FIELDS_IN_INDEX;i++) {
		k = objData->indexedOnC[keyNum][i];
		//cout << "Compare   i=" << i << "   k=" << k << endl;
		if (k==-1) return 0; // Have reached end with no differences.

		// Compare each field of the record according to
		// its type.

		switch(objData->fieldType[k]) {
			case D_STRING:
				cerr << "String comparison unimplemented" << endl;
				abort();
			case D_DATA:
				cerr << "String comparison unimplemented" << endl;
				abort();
			case D_UNIQUE:
				if ((res = compare_uniqueType(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_LONG:
				if ((res = compare_long(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_ULONG:
				if ((res = compare_ulong(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_DOUBLE:
				if ((res = compare_double(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_MONEY:
				if ((res = compare_moneyType(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_DATE:
				if ((res = compare_dateType(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_SHORT:
				if ((res = compare_short(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_USHORT:
				if ((res = compare_ushort(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_FLOAT:
				if ((res = compare_float(
					(pKeyType)theData + objData->fieldOffset[k],
					(pKeyType)key + j))!=0) {
					return res;
				}
				break; // continue comparison.
			case D_ICHAR:
				//cout << "Compare:" << (char *)(theData+objData->fieldOffset[k]) << ":" << (char *)(key + j) << ":" << endl;
				if ((res = strncasecmp(
					(char *)theData+objData->fieldOffset[k], key + j,
					objData->fieldLength[k]))!=0) {
					return res;
				}
				break;
			case D_CHAR:
				//cout << "Compare:" << (char *)(theData+objData->fieldOffset[k]) << ":" << (char *)(key + j) << ":" << endl;
				if ((res = strncmp(
					(char *)theData+objData->fieldOffset[k], key + j,
					objData->fieldLength[k]))!=0) {
					return res;
				}
				break;
			default:
				cerr << "Illegal field type" << endl;
				abort();
		}
		j+=objData->fieldLength[k]; // Move to next section of the packed key.
	}
	return 0; // All fields done up to MAX FIELDS, and no difference.
}

bool diaGRel::isUnique(long keyNum)
{
	// Return true if the index number 'keyNum' includes
	// a unique field.
	if (keyNum == -1) return false; // Not sure why this is here.
	return objData->containsUniq[keyNum]?true:false;
}

long 
diaGRel::numStrings(void)
{
	return objData->stringCount;
}

_db_strStruct* 
diaGRel::getArray(void)
{
	return theArray;
}

dbData*
diaGRel::getString(long num)
{
	return theArray[num].theString;
}

long*
diaGRel::getLong(long num)
{
	return theArray[num].theLong;
}

long
diaGRel::fieldNum(char* name)
{
	for (int i=0; i<objData->numFields; i++) 
		if (!strcmp(objData->fieldName[i], name))
			return i;
	return -1;
}
