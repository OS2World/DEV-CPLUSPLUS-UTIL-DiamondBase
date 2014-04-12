/**************************************************************************
 * Source Id :
 *
 * $Id: dbobj.cc,v 1.42 1993/11/04 14:59:41 kevinl Exp $
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
 *		dbObject	: Maintains information for a single database relation.
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dbobj.cc,v $
// Revision 1.42  1993/11/04  14:59:41  kevinl
// Fixed record locking
//
// Revision 1.41  1993/11/03  10:04:46  kevinl
// Added ichar and utils.{h,cc}
//
// Revision 1.40  1993/10/28  07:49:01  kevinl
// Added diaGRel dbString/dbData support
//
// Revision 1.39  1993/10/18  11:36:57  kevinl
// Added field names and setRecNum calls
//
// Revision 1.38  1993/10/07  13:22:17  darrenp
// removed debugging info.
//
// Revision 1.37  1993/10/05  07:31:07  kevinl
// Added in generation of info for dbObjData for generic relations (diaGRel)
//
// Revision 1.36  1993/09/26  06:40:07  kevinl
// Added dbData support
//
// Revision 1.35  1993/08/12  05:31:50  darrenp
// Zeroed theMemServer pointer, in case it doesn't get initialised
//
// Revision 1.34  1993/07/19  11:52:51  kevinl
// Removed an extraneous return
//
// Revision 1.33  1993/07/13  04:45:25  kevinl
// Added getData, putData, startData and endData for MULTI
//
// Revision 1.32  1993/07/01  13:26:30  kevinl
// We now delete Strings
// Pointers to uniques no longer returned. {has,get,set}Unqiue members
//
// Revision 1.31  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.30  1993/06/20  13:42:44  kevinl
// Fixed multiple mallocs
// String support added
//
// Revision 1.29  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.28  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.27  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.26  1993/05/03  23:34:09  kevinl
// Removed err shadowing local var
//
// Revision 1.25  1993/05/03  01:34:48  kevinl
// No more extern consts
// Cosmetic changes for CC
//
// Revision 1.24  1993/05/01  14:37:59  kevinl
// Got rid of ints
//
// Revision 1.23  1993/04/28  14:57:37  kevinl
// Give records back when add fails
//
// Revision 1.22  1993/04/28  11:30:36  kevinl
// write now checks for locks
//
// Revision 1.21  1993/04/27  07:15:24  kevinl
// Added qWrite
// Fixed qId bounds checks on all query functions
//
// Revision 1.20  1993/04/25  13:20:00  kevinl
// Fixed up error code returns
// Commetns
//
// Revision 1.19  1993/04/15  07:58:55  kevinl
// Added del
//
// Revision 1.18  1993/04/15  04:49:23  kevinl
// Fixed up include files
//
// Revision 1.17  1993/04/11  05:49:02  kevinl
// Rationalised find/seek/peek methods etc
//
// Revision 1.16  1993/04/09  13:00:29  kevinl
// Stats can be called from diaRel now.
//
// Revision 1.15  1993/04/08  15:40:00  kevinl
// MAde sure unique fields are not touched when they don't exist
//
// Revision 1.14  1993/04/07  02:29:21  kevinl
// Changed qEnd to reset qId to -1
//
// Revision 1.13  1993/04/06  05:48:23  kevinl
// Range check in qEnd
//
// Revision 1.12  1993/04/05  07:50:21  kevinl
// Fixed the rereading in add
//
// Revision 1.11  1993/04/04  23:58:11  kevinl
// Handle unique fields
// Reget record during add
//
// Revision 1.10  1993/04/01  11:28:40  kevinl
// Range check on qBegin index field
//
// Revision 1.9  1993/04/01  11:22:03  kevinl
// Set unique member to record number (till further notice)
//
// Revision 1.8  1993/04/01  04:23:18  kevinl
// Fixed locking
// Added qFind, fixed the rest
//
// Revision 1.7  1993/03/30  07:14:47  davison
// Add initialisation of the usageCount.
//
// Revision 1.6  1993/03/29  08:05:47  darrenp
// Nuked pathinfo
// Fixed eof errors
// Malloc lib
//
// Revision 1.5  1993/03/28  13:58:45  kevinl
// Modified add to check for duplicates first.
//
// Revision 1.4  1993/03/28  10:32:33  davison
// Modified for dbErr
// ./
//
// Revision 1.3  1993/03/28  04:53:59  root
// more error code standardization.
//
// Revision 1.2  1993/03/26  06:15:57  darrenp
// standardised error codes.
//
// Revision 1.1  1993/03/25  22:29:25  davison
// Initial revision
//
 **************************************************************************/

#include <stdio.h>

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif

#include <dbobj.h>
#include <btree.h>
#include <d_types.h>

char* dbObject::verStr(void)
{
	return "$Id: dbobj.cc,v 1.42 1993/11/04 14:59:41 kevinl Exp $";
}

// Constructor. Creates a new dbObject

dbObject::dbObject(char* newpath, char* newprefix)
{
	int	dataOffset = 0;
	theMemServer = 0;
	long i,j,k;
	
	// Give recserver the database filename.
	path=new char[strlen(newpath)+3];
	prefix=new char[strlen(newprefix)+1];

	strcpy(path,newpath);
	if(path[strlen(path)-1] != '/')
		 strcat(path,"/");

	strcpy(prefix,newprefix);

	char* dbName = new char[strlen(path) + strlen(prefix) + 5];
	strcpy(dbName,path);
	strcat(dbName,prefix);
	strcat(dbName,".db");

	// Ok. Now lets extract the field and index info for future reference.
	
#	ifdef __BORLANDC__
		ifstream dbFile(dbName,ios::binary);
#	else
		ifstream dbFile(dbName);
#	endif

	checkStream(dbFile);	
	dbFile.read((char*)&dataOffset,sizeof(int));
	checkStream(dbFile);

	dbFile >> fldList >> idxList;
	checkStream(dbFile);

	// Now create a recServer.

	theRecServer = new recServer(dbName,dataOffset);

	// Do we need a string server. Don't have an object to ask, so 
	// scan the fields

	stringCount = 0;
	theMemServer = 0;
	fieldInfo *f = fldList.fields;
	while (f)
	{
		if (f->fldType == D_STRING || f->fldType == D_DATA)
		{
			if (!theMemServer)
			{
				char* memName = new char[strlen(path) + strlen(prefix) + 5];
				sprintf(memName,"%s%s.str", path, prefix);
				theMemServer = new memServer(memName);
				delete memName;
			}
			stringCount++;
		}
		f = f->nextFld;
	}
	
	// And the list of bTrees.

	theBTreeList = new bTree *[idxList.numIndicies];
	for(i=0;i<idxList.numIndicies;i++)
	{
		char* idxFile = new char[strlen(path) + strlen(prefix) + 5];
		sprintf(idxFile,"%s%s.id%d",path,prefix,i);

		// Load the bTree for index i
		theBTreeList[i] = new bTree(idxFile,i);
//		theBTreeList[i]->dump();
//		currentRecs[i] = NO_REC;
		delete idxFile;
	}

	// Finally clear out the query information;

	for(i=0;i<MAX_QUERY;i++)
		queryInfo[i] = 0;
	
	// Nobody is using us
	usageCount=0;
	delete dbName;

	// OK, now reorganise all this into a form that can be used to
	// understand the relation shape.

	// Now - construct the following tables:

	numIndices		=idxList.numIndicies;	// How many indices are there ?
	numFields		=fldList.numFields;	// How many fields.

	fieldLength		= new long[numFields];	// How long each field is.
	fieldOffset		= new long[numFields];	// Where each field is.
	containsUniq	= new long[numIndices];	// If the index contains a uniq.
	keyLength		= new long[numIndices];	// Length of the keys.
	fieldType		= new char[numFields];	// What type each field is.
	fieldName		= new char*[numFields];	// What they are called.

	indexedOnP		// Which fields are in each index. - packing order
					   = (long (*)[MFII])new long[MFII * numIndices];

	indexedOnC		// Which fields are in each index. - comparison
					   = (long (*)[MFII])new long[MFII * numIndices];

	// Memory ist allocated - now fill out the tables. This is
	// considerably complicated by the fact that dsc reorders the
	// fields to bypass alignment problems. Non char fields come
	// out first, and then the char fields.

	// Iterate through the fields - on this pass, calculate the
	// offsets of the non-char fields, the size of every field
	// the offsets of the char fields relative to the start of the
	// char section, and the field types.
	// i is the field number,
	// j is the offset into the non-strings.
	// k is the offset into the strings.

	fieldInfo	*fPtr;
	i = j = k = 0;
	if (stringCount)
	{
		theLongs = new long[stringCount];
		stringField = new long[stringCount];
	}
	else
	{
		theLongs = 0;
		stringField = 0;
	}
	int s = 0;
	for(fPtr=fldList.fields;fPtr;fPtr= fPtr->nextFld,i++) {
		fieldLength[i] = fPtr->fldSize;

		// Strings get shuffled to the end of the fields
		// as far as storage and structure layout goes.

		if (fPtr->fldType == D_CHAR || fPtr->fldType == D_ICHAR) {
			fieldOffset[i] = k;
			k+=fPtr->fldSize;
		} else {
			fieldOffset[i] = j;
			j+=fPtr->fldSize;
		}
		if (fPtr->fldType == D_STRING || fPtr->fldType == D_DATA)
		{
			stringField[s] = i;
			theLongs[s] = fieldOffset[i];
			s++;
		}
		fieldType[i] = fPtr->fldType;
		fieldName[i] = strdup(fPtr->fldName);
	}
	dataLength = j+k; // Cross fingers that compiler works this way.

	uniqOff = -1; // May be changed below.

	// Ok - now we have found out most of what we wanted to
	// know - now  move the string stuff to the end.
	for(i=0;i<numFields;i++) {
		if (fieldType[i] == D_CHAR || fieldType[i] == D_ICHAR) {
			fieldOffset[i] += j;
		}
		if (fieldType[i] == D_UNIQUE) {
			// Icky pooh - set the unique field to -1.
			// We don't do this anymore. It's in diagrel
			// *(long *)((char *)theData+fieldOffset[i]) = -1;

			// Remember the offset of the unique field for
			// future reference.
			// uniqOffset = (long *)((char *)theData+fieldOffset[i]);
			uniqOff = fieldOffset[i];
		}
	}

	// Traverse the index list to construct quick reference
	// tables to do comparisons quickly.
	// Calculate keyLength, containsUniq,
	//
	indexInfo	*iPtr;
	i = 0;
	for(iPtr = idxList.indicies;iPtr;iPtr=iPtr->nextIdx,i++) {
		memcpy(indexedOnC[i],iPtr->idxFields,
			MAX_FIELDS_IN_INDEX * sizeof(long));

		// Find out if this index contains a unique field.
		containsUniq[i] = false;
		// Tally key length
		keyLength[i] = 0;

		// During this loop, also copy all the non-string indexed
		// fields into the packing indexedOn array.
		k = 0;
		for(j=0;j<MAX_FIELDS_IN_INDEX;j++) {
			if (iPtr->idxFields[j]==-1) break;
			if (fieldType[iPtr->idxFields[j]]==D_UNIQUE) {
				containsUniq[i] = true;
			}
			keyLength[i] += fieldLength[iPtr->idxFields[j]];
			if (fieldType[iPtr->idxFields[j]]!=D_CHAR || fieldType[iPtr->idxFields[j]]!=D_ICHAR) {
				indexedOnP[i][k++] = indexedOnC[i][j];
			}
		}
		// Now finish copying across all the CHAR fields which
		// need to migrate to the end of the packed key.
		for(j=0;j<MAX_FIELDS_IN_INDEX;j++) {
			if (iPtr->idxFields[j]==-1) break;
			if (fieldType[iPtr->idxFields[j]]==D_CHAR || fieldType[iPtr->idxFields[j]]==D_ICHAR) {
				indexedOnP[i][k++] = indexedOnC[i][j];
			}
		}
		// Put the -1 marker on the end.
		if (j<MAX_FIELDS_IN_INDEX) {
			indexedOnP[i][j] = -1;
		}
	}
}

// A very neat, memory friendly destructor

dbObject::~dbObject()
{
	for(int i=0;i<idxList.numIndicies;i++)
		delete theBTreeList[i];

	delete theBTreeList;
	delete theRecServer;
	delete theMemServer;
	delete path;
	delete prefix;
}

// This is a multipurpose function.
// It sets the record number in the current query, unlocks the current
// query and then checks to see if any other queries already have this record
// locked.
dbError dbObject::checkRec(long qId, long recIdx, bool lockit)
{
	queryInfo[qId]->queryRec = recIdx;
	queryInfo[qId]->unlock();

	for (int i=0; i < MAX_QUERY; i++)
		if (queryInfo[i] && queryInfo[i]->locked() && queryInfo[i]->queryRec == recIdx)
			return dbErr(db_reclocked);

	// It should be ok now to lock it.

	if (lockit)
		queryInfo[qId]->lock();
	else
		queryInfo[qId]->unlock();

	return dbErr(db_ok);
}

// Output the cache stats for the recServer and each bTree

void dbObject::stats(void)
{
	cout << "Record Server cache: ";
	theRecServer->stats();

	for(int i=0;i<idxList.numIndicies;i++)
	{
		cout << "Index " << i << " cache: ";
		theBTreeList[i]->stats();
	}
}

// add a new object into the dbObject. This verifies that the object is not
// present in any bTree and if so proceeds to add it in.
// We do NOT allow duplicates. Unique fields can be used to circumvent
// this

dbError dbObject::add(object& theObject)
{
	long	newIdx;
	bool	unq;
	//long	uniq;
	dbError err;

	unq = theObject.hasUnique();

#if 0
	// We used to remember the unique id and set it to -1 to prevent bogus
	// clashes in the inBTree calls. Now we don't do this anymore since we
	// don't inBTree indexes with unique fields in them
	if (unq)
	{
		uniq = *unq;
		*unq = -1;
	}
#endif

	// Let's check the btrees to make sure it's not there already.
	// If an index has a unique field, don't check it. This makes things
	// much quicker (depending on the object concerned). It can't hurt.

	for(long i=0;i<idxList.numIndicies;i++)
		if (!theObject.isUnique(i))
			if (theBTreeList[i]->inBTree(theObject))
			{
#if 0
				// We don't save this anymore
				if (unq)
					*unq = uniq;
#endif
				return dbErr(db_dup);
			}

	// Ok. First get the recserver to insert the data into the database.

	err= theRecServer->newRec(newIdx);//Get the logical index of the new record.
	if (err != db_ok)
		return err;

	// If this object has a unique field, stick in the new value
	if (unq)
		theObject.setUnique(newIdx);

	// We must add to the memServer first so the long members are
	// set correctly.

	if (theMemServer)
	{
#ifdef DEBUG
		cout << "Adding to memServer now" << endl;
#endif
		for (int i=0; i < theObject.numStrings(); i++)
		{
			*theObject.getArray()[i].theLong=0;
			dbError err = theMemServer->putString(*theObject.getArray()[i].theString, *theObject.getArray()[i].theLong);
			if (err != db_ok)
				return err;
		}
	}

	// Where is the data to save?
	theObject.getData();
	void* thevoid = (void*)theObject;

	// Save it.
	err= theRecServer->putRec(newIdx,thevoid);
	theObject.setRecNum(newIdx);
	theObject.endData();
	if (err != db_ok)
		return err;

#ifdef DEBUG
	cout << "Adding to BTrees now..." << endl;
#endif

	// Then tell all the indicies to install this object.

	for(i=0;i<idxList.numIndicies;i++)
	{
		//pKeyType fred = theObject.getKey(i);
		//cout << *((long*)fred) << endl;
		//delete fred;

		err = theBTreeList[i]->add(theObject,newIdx);
		if (err != db_ok)
		{
			// Give the recServer its record back
			theRecServer->delRec(newIdx);
			return err;
		}
		// Since the btree insertion code can nuke the indexes,
		// we reread the data
		// Forget it, bTree::add is nice to us now.
//		theRecServer->getRec(newIdx,thevoid);
	}

	return dbErr(db_ok);
}

// Now we can delete! We first check all the bTree's and if it's in ALL of
// them, we delete the object from all of them!

dbError dbObject::del(object& theObject)
{
	long	delIdx;
	dbError err;

	// First make sure that no query has this record locked

	// Let's check the btrees to make sure it's in all of them

	for(int i=0;i<idxList.numIndicies;i++)
		if (!theBTreeList[i]->inBTree(theObject, delIdx))
			return dbErr(db_nfound);
		else if (!i)
		{
			// It is in the first BTree. Now let's see if anyone has it locked
			for (int j=0; j < MAX_QUERY; j++)
				if (queryInfo[j] && queryInfo[j]->locked() && queryInfo[j]->queryRec == delIdx)
					return dbErr(db_reclocked);
		}

	// First get rid of the strings

	if (theMemServer)
	{
#ifdef DEBUG
		cout << "Nuking from memServer now" << endl;
#endif
		for (int i=0; i < theObject.numStrings(); i++)
		{
			dbError err = theMemServer->delString(*theObject.getArray()[i].theString, *theObject.getArray()[i].theLong);
			if (err != db_ok)
				return err;
		}
	}

	// Ok. Now get the recserver to nuke the data from the database.

	err= theRecServer->delRec(delIdx);
	if (err != db_ok)
		return err;

	// Then tell all the indicies to delete this object.

	for(i=0;i<idxList.numIndicies;i++)
	{
		//pKeyType fred = theObject.getKey(i);
		//cout << *((long*)fred) << endl;
		//delete fred;

		err = theBTreeList[i]->del(theObject);
		if (err != db_ok)
			return err;
	}

	return dbErr(db_ok);
}

// qBegin:
// Start up a query. This makes it possible to keep track of searches and
// walks through the relation. The query has an index associated with it so it
// knows which bTree to use for the query

dbError dbObject::qBegin(long idxId, long& qId)                               
{                                                                             
	if (idxId < 0 || idxId >= idxList.numIndicies)
		return dbErr(db_range);

    dbError err;                                                       
	// Tell the appropriate bTree to start a query
    err = theBTreeList[idxId]->qBegin(qId);
    switch (err)                                                              
    {                                                                         
        case db_ok:                                                           
		{
			// Find a vacant queryInfo.
			for (int i=0;i<MAX_QUERY;i++)
				if (queryInfo[i] == 0) break;
			if (i >= MAX_QUERY)
			{
				// Nope, there is no space, end the query and get out
				theBTreeList[idxId]->qEnd(qId);
				return dbErr(db_toomany, "Creating database query");
			}

			// Make a new queryInfo for the query
			// qId is the number of the Btree query. Tell the dbQuery about
			// it.
			queryInfo[i] = new dbQueryInfo(qId, idxId);

			// Now we're finished with qId so we can set it to the dbQuery
			// id.
			qId = i;
            return dbErr(db_ok);
		}
        case db_toomany:
            return dbErr(db_toomany, "Creating btree query");
        default:                                                              
			return dbErr(db_err, "Strange error from btree");
    }
}                                                                             

// qSeekFirst:
// This moves to the beginning of the relation according to the index
// associated with the query concerned. NB after this call, the next call to
// dbObject::qNext will return the first record of the relation.

dbError dbObject::qSeekFirst(long qId, object& theObject)
{
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

// Get the bTree to do a first
	return theBTreeList[queryInfo[qId]->queryIdx]->
			qFirst(queryInfo[qId]->queryId,theObject,recIdx);
}

// qPeekNext:
// Show us what the next call to dbObject::qNext will return without
// altering the state of the relation

dbError dbObject::qPeekNext(long qId, object& theObject)
{
	dbError err;
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	err = theBTreeList[queryInfo[qId]->queryIdx]->
			qPeekNext(queryInfo[qId]->queryId,theObject,recIdx);

	if (err == db_eof)
	{
		return err;
	}
	else if (err != db_ok)
	{
		return dbErr(db_err, "Fatal error from bTree::qPeekNext");
	}

	// Now try to get the object from the record server... 

	theObject.startData();
	theRecServer->getRec(recIdx, (void*)theObject);
	theObject.setRecNum(recIdx);
	theObject.putData();

	// And it's strings

	dbError err2 = getStrings(theObject);
	if (err2 != db_ok)
		return err2;

	// Remember the current record number and check for locks

	return checkRec(qId, recIdx);
}

// qNext:
// Return the next entry in the relation and advance the relation
// If lock is true then we will attempt to lock the returned record

dbError dbObject::qNext(long qId, object& theObject, bool lock)
{
	dbError err;
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	err = theBTreeList[queryInfo[qId]->queryIdx]->
			qNext(queryInfo[qId]->queryId,theObject,recIdx);

	if (err == db_eof)
	{
		return err;
	}
	else if (err != db_ok)
	{
		return dbErr(db_err, "Fatal error from bTree::qNext");
	}

	// Now try to get the object from the record server...

	theObject.startData();
	theRecServer->getRec(recIdx, (void*)theObject);
	theObject.setRecNum(recIdx);
	theObject.putData();

	// And it's strings

	dbError err2 = getStrings(theObject);
	if (err2 != db_ok)
		return err2;

	// Remember the current record number

	// bTree::qNext would return db_reclocked if
	// the record was locked. Hence we can lock it
	return checkRec(qId, recIdx, (bool)(err == db_ok && lock));
}

// qPeekPrev:
// like qPeekNext but different

dbError dbObject::qPeekPrev(long qId, object& theObject)
{
	dbError err;
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	err = theBTreeList[queryInfo[qId]->queryIdx]->
			qPeekPrev(queryInfo[qId]->queryId,theObject,recIdx);

	if (err == db_eof)
	{
		return err;
	}
	else if (err != db_ok)
	{
		return dbErr(db_err, "Fatal error from bTree::qPeekPrev");
	}

	// Now try to get the object from the record server... :-?

	theObject.startData();
	theRecServer->getRec(recIdx, (void*)theObject);
	theObject.setRecNum(recIdx);
	theObject.putData();

	// And it's strings

	dbError err2 = getStrings(theObject);
	if (err2 != db_ok)
		return err2;

	// Remember the current record number

	return checkRec(qId, recIdx);
}

// qPrev:
// like qNext but different

dbError dbObject::qPrev(long qId, object& theObject, bool lock)
{
	dbError err;
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	err = theBTreeList[queryInfo[qId]->queryIdx]->
			qPrev(queryInfo[qId]->queryId,theObject,recIdx);

	if (err == db_eof)
	{
		return db_eof;
	}
	else if (err != db_ok)
	{
		return dbErr(db_err, "Fatal error from bTree::qPrev");
	}

	// Now try to get the object from the record server... :-?

	theObject.startData();
	theRecServer->getRec(recIdx, (void*)theObject);
	theObject.setRecNum(recIdx);
	theObject.putData();

	// And it's strings

	dbError err2 = getStrings(theObject);
	if (err2 != db_ok)
		return err2;

	// Remember the current record number

	return checkRec(qId, recIdx, (bool)(err==db_ok && lock));
}

// qSeekLast:
// Like qSeekFirst but different

dbError dbObject::qSeekLast(long qId, object& theObject)
{
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	return theBTreeList[queryInfo[qId]->queryIdx]->
			qLast(queryInfo[qId]->queryId,theObject,recIdx);
}

// qEnd:
// We make sure the query is in use and then tell the bTree to lose it.
// Then we lose our connection to it.

dbError dbObject::qEnd(long& qId)
{
	// Make sure the qId is a good one
	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);
	if (!queryInfo[qId])
		return dbErr(db_noquery);

	// Tell the relevant bTree to end it's query
	dbError err = theBTreeList[queryInfo[qId]->queryIdx]->qEnd(queryInfo[qId]->queryId);

	// Free our memory and reset the query to unused
	delete queryInfo[qId];
	queryInfo[qId] = 0;
	qId = -1;
	return err;
}                      

// qSeek:
// This searches the relation for a particular object. The next call to
// dbObject::qNext will return this searched for object, or the one that will
// follow it in the relation if it was not found

dbError dbObject::qSeek(long qId, object& theObject)
{
	long	recIdx;

	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	return theBTreeList[queryInfo[qId]->queryIdx]->
			qFind(queryInfo[qId]->queryId,theObject,recIdx);
}

dbError dbObject::qWrite(long qId, object& theObject)
{
	// Validate the query
	if (qId < 0 || qId > MAX_QUERY)
		return dbErr(db_range);

	if (!queryInfo[qId])
		return dbErr(db_noquery);

	// Count how many keys are already in the bTree's and make sure they
	// refer to the same record number.
	// NB We have to check unique fields here too, unlike in add.
	long count = 0;
	long recNum = -1;
	long thisRec;
	for (int i=0; i<idxList.numIndicies;i++)
		if (theBTreeList[i]->inBTree(theObject, thisRec))
		{
			count++; // Got one more
			if (recNum == -1)
				recNum = thisRec; // The first one
			else
				// If we already have encountered this key and we do so
				// again now, but the record number is different, then the
				// write is not allowed.
				if (thisRec != recNum)
					return dbErr(db_dup);
		}

	// If the record is not found at all we can't write, use add instead

	if (!count)
		return dbErr(db_nfound);

	// If _all_ bTree's didn't match then we can't write

	if (count != idxList.numIndicies)
		return dbErr(db_dup);

	// We are now assured that all the keys match to the same record
	// number, which will be in recNum (or thisRec).

	// Let's check to see if someone has it locked....

	dbError err;
	if ((err=checkRec(qId, thisRec)) != db_ok)
		return err;

	// Save the strings back. Note that we leave the offsets along this
	// time around as the string is assumed to be there already.

	if (theMemServer)
	{
#ifdef DEBUG
		cout << "Writing to memServer now" << endl;
#endif
		for (int i=0; i < theObject.numStrings(); i++)
		{
			if ((err = theMemServer->putString(*theObject.getArray()[i].theString, *theObject.getArray()[i].theLong)) != db_ok)
				return err;
		}
	}

	// Let's tell the recServer to save the data then.

	theObject.getData();
	err = theRecServer->putRec(thisRec, (void*)theObject);
	theObject.endData();
	return err;
}

dbError dbObject::getStrings(object& theObject)
{
	if (theMemServer)
	{
		dbError err;
#ifdef DEBUG
		cout << "Reading from memServer now" << endl;
#endif
		for (int i=0; i < theObject.numStrings(); i++)
		{
			if ((err = theMemServer->getString(*theObject.getArray()[i].theString, *theObject.getArray()[i].theLong)) != db_ok)
			{
#ifdef DEBUG
				cout << "memServer returned error " << err << endl;
#endif
				return err;
			}
		}
	}

	return dbErr(db_ok);
}
