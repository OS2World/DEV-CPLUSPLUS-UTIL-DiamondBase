/**************************************************************************
 * Source Id :
 *
 * $Id: idxinfo.cc,v 1.13 1993/06/23 05:21:22 kevinl Exp $
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
 *		Code to support the field and header strctures for the database 
 *	files themselves.
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: idxinfo.cc,v $
// Revision 1.13  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.12  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.11  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.10  1993/05/03  23:35:47  kevinl
// the MAGIC nums are const
//
// Revision 1.9  1993/05/03  01:35:37  kevinl
// Got rid of externs on the consts
//
// Revision 1.8  1993/04/28  11:31:09  kevinl
// Fixed up inlines
//
// Revision 1.7  1993/04/27  07:00:19  kevinl
// Comments
//
// Revision 1.6  1993/04/15  06:06:38  davison
// Fixed up inline not expanded errors etc
//
// Revision 1.5  1993/04/15  04:21:52  kevinl
// Moved malloc.h
//
// Revision 1.4  1993/04/08  05:26:40  kevinl
// FGixed some memory leaks
//
// Revision 1.3  1993/03/29  08:06:22  darrenp
// Can't add to a pointer. Add to size of data instead
// malloc lib.
//
// Revision 1.2  1993/03/15  19:09:15  davison
// Implemented a few nice things, like more than 2 index fields, line number
// reporting on syntax errors, and usage reporting.
//
// Just for you, Daz :-)
//
// Revision 1.1  1993/03/15  19:07:31  davison
// Initial revision
//
 **************************************************************************/

#include <stdlib.h>
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

#include <idxinfo.h>

//--------------------------------------------------------------
// Constructor
//

indexInfo::indexInfo(int type, TIndicies indicies )
{
	idxType = type;
	for(int i=0;i<MAX_FIELDS_IN_INDEX;i++)
	{
		idxFields[i] = indicies[i];
	}
	nextIdx = 0;
}

indexInfo::~indexInfo()
{
	if (nextIdx)
		delete nextIdx;
}

fieldInfo::fieldInfo(char* name, int ty, int sz)
{
	fldName = new char[strlen(name)+1];
	strcpy(fldName,name);
	fldType = ty;
	fldSize = sz;

	nextFld = 0;
}

fieldInfo::~fieldInfo()
{
	delete fldName;
	if (nextFld)
		delete nextFld;
}

//--------------------------------------------------------------
// Stream checking routines.

void checkStream(ifstream& s) 
{
	if (!s)
	{
		cerr << "Error : Problem reading relation file. Aborting." << endl;
		exit(1);
	}
}

void checkStream(ofstream& s) 
{
	if (!s)
	{
		cerr << "Error : Problem writing to relation file. Aborting." << endl;
		exit(1);
	}
}

//--------------------------------------------------------------
// Standard stream insertors/extractors.

ostream& operator << (ostream& s, indexList& list)
{
	if (list.indicies!=0)
		s << *(list.indicies);
	return s;
}


ostream& operator << (ostream& s, indexInfo& info)
{
	s << "Indexed on field " << info.idxFields[0];
	for(int i=1;i<MAX_FIELDS_IN_INDEX;i++)
	{
		if(info.idxFields[i] != -1)
			s << ", " << info.idxFields[i];
	}
	if (info.idxType & IDX_NODUPLICATES)
		s << " without duplicates." << endl;
	else
		s << " with duplicate indicies permitted." << endl;
	if (info.nextIdx)
		s << *(info.nextIdx);

	return s;
}

ostream& operator << (ostream& s, fieldList& list)
{
	s << *(list.fields);
	return s;
}

ostream& operator << (ostream& s, fieldInfo& info)
{
	s << "Name : " << info.fldName << endl;
	s << "Type : " << info.fldType << endl;
	s << "Size : " << info.fldSize << " bytes" << endl;

	if (info.nextFld)
		s << *(info.nextFld);
	return s;
}
//----------------------------------------------------------------
// File stream insertors/extractors.

ofstream& operator << (ofstream& s, indexList& list)
{
	s.write((const char*)&IDX_LIST_MAGIC,sizeof(int));
	checkStream(s);
	s.write((const char*)&list.numIndicies,sizeof(int));
	checkStream(s);

	if(list.indicies != 0)
		s << *(list.indicies);
	return s;
}

ofstream& operator << (ofstream& s, indexInfo& info)
{
	s.write((const char*)&IDX_INFO_MAGIC, sizeof(int));
	checkStream(s);
	s.write((char*)&info.idxType,sizeof(int));
	checkStream(s);
	s.write((char*)&info.idxFields[0],MAX_FIELDS_IN_INDEX*sizeof(int));
	checkStream(s);

	if (info.nextIdx != 0)
		s << *(info.nextIdx);
	return s;
}

ofstream& operator << (ofstream& s, fieldList& list)
{
	s.write((const char*)&FLD_LIST_MAGIC,sizeof(int));
	checkStream(s);
#ifdef DEBUG
	cout << "Dumping :" << hex << FLD_LIST_MAGIC << dec << endl;
#endif
	s.write((char*)&list.numFields,sizeof(int));
#ifdef DEBUG
	cout << "Number of fields = " << list.numFields << endl;
#endif
	checkStream(s);

	if (list.fields != 0)
		s << *(list.fields);
	return s;
}

ofstream& operator << (ofstream& s, fieldInfo& info)
{
	s.write((const char*)&FLD_INFO_MAGIC,sizeof(int));
	checkStream(s);
	s.write((char*)&info.fldType,sizeof(int));
	checkStream(s);
	s.write((char*)&info.fldSize,sizeof(int));
	checkStream(s);
	int len = strlen(info.fldName)+1;
	s.write((char*)&len,sizeof(int));
	checkStream(s);
	s.write((char*)info.fldName,len);
	checkStream(s);

	if (info.nextFld != 0)
		s << *(info.nextFld);
	
	return s;
}

// Extract the fieldlist from a stream

ifstream& operator >> (ifstream& s, fieldList& list)
{
	int temp;

#ifdef DEBUG
	cout <<"Reading field list magic number." << endl;
#endif
	s.read((char*)&temp,sizeof(int));
	checkStream(s);
	if(temp != FLD_LIST_MAGIC)
	{
		cerr << "Bad field list in relation file. Aborting." << endl;
		exit(1);
	}
#ifdef DEBUG
	cout <<"Reading number of fields" << endl;
#endif

	s.read((char*)&list.numFields,sizeof(int));
	checkStream(s);

	fieldInfo* currentFld = 0;

	for(int i=0;i<list.numFields;i++)
	{
		int type,size,len;
		char* name;
#ifdef DEBUG
	cout <<"Reading field info magic number." << endl;
#endif

		s.read((char*)&temp,sizeof(int));
		checkStream(s);
		if(temp != FLD_INFO_MAGIC)
		{
			cerr << "Bad field info in relation file. Aborting."<< endl;
			exit(1);
		}
#ifdef DEBUG
	cout <<"Reading type of field." << endl;
#endif
		s.read((char*)&type,sizeof(int));
		checkStream(s);
#ifdef DEBUG
	cout << "Reading size of field." << endl;
#endif
		s.read((char*)&size,sizeof(int));
		checkStream(s);
#ifdef DEBUG
	cout <<"Reading length of field name." << endl;
#endif
		s.read((char*)&len,sizeof(int));
		checkStream(s);
		name = new char[len+1];
#ifdef DEBUG
	cout << "Reading field name." << endl;
#endif
		s.read((char*)name,len);
		checkStream(s);

		fieldInfo* tInfo = new fieldInfo(name,type,size);
		delete name;

		if(currentFld != 0)
			currentFld->nextFld = tInfo;
		else
			list.fields = tInfo;
		currentFld = tInfo;
	}
#ifdef DEBUG
	cout << "Finished reading field information." << endl;
#endif
	return s;
}

// Extract the indexlist from a stream

ifstream& operator >> (ifstream& s, indexList& list)
{
	int temp;

	s.read((char*)&temp,sizeof(int));
	checkStream(s);
	if(temp != IDX_LIST_MAGIC)
	{
		cerr << "Bad index list in relation file. Aborting." << endl;
		exit(1);
	}

#ifdef DEBUG
	cout <<"Reading Number of indicies." << endl;
#endif
	s.read((char*)&list.numIndicies,sizeof(int));
	checkStream(s);

	indexInfo* currentIdx = 0;

	for(int i=0;i<list.numIndicies;i++)
	{
		int type
		//,field1,field2
		;

#ifdef DEBUG
	cout <<"Reading Index Info Magic number." << endl;
#endif
		s.read((char*)&temp,sizeof(int));
		checkStream(s);
		if(temp != IDX_INFO_MAGIC)
		{
			cerr << "Bad index info in relation file. Aborting."<< endl;
			exit(1);
		}
		TIndicies indicies;
#ifdef DEBUG
	cout <<"Reading type of index." << endl;
#endif
		s.read((char*)&type,sizeof(int));
		checkStream(s);
#ifdef DEBUG
	cout << "Reading index field array." << endl;
#endif
		s.read((char*)&indicies,MAX_FIELDS_IN_INDEX*sizeof(int));
		checkStream(s);

		indexInfo* tInfo = new indexInfo(type,indicies);
		if(currentIdx != 0)
			currentIdx->nextIdx = tInfo;
		else
			list.indicies = tInfo;
			
		currentIdx = tInfo;
	}
	return s;
}

