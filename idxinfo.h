/**************************************************************************
 * Source Id :
 *
 * $Id: idxinfo.h,v 1.10 1993/10/18 11:35:01 kevinl Exp $
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
 *  Index/Field Information Headers.
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: idxinfo.h,v $
 * Revision 1.10  1993/10/18  11:35:01  kevinl
 * Added number()
 *
 * Revision 1.9  1993/05/03  01:35:37  kevinl
 * Got rid of externs on the consts
 *
 * Revision 1.8  1993/04/28  11:31:09  kevinl
 * Fixed up inlines
 *
 * Revision 1.7  1993/04/27  07:00:19  kevinl
 * Comments
 *
 * Revision 1.6  1993/04/15  06:06:38  davison
 * Fixed up inline not expanded errors etc
 *
 * Revision 1.5  1993/04/15  04:21:52  kevinl
 * Moved malloc.h
 *
 * Revision 1.4  1993/03/21  01:15:33  davison
 * Ooops. Fixed that last one proper like now ...
 *
 * Revision 1.3  1993/03/21  01:02:11  davison
 * Made a couple of const's #defines.
 *
 * Revision 1.2  1993/03/15  19:24:05  davison
 * Implemented a few nice things, like more than 2 index fields, line number
 * reporting on syntax errors, and usage reporting.
 *
 * Just for you, Daz :-)
 *
 * Revision 1.1  1993/03/15  19:22:53  davison
 * Initial revision
 *
 **************************************************************************/
#ifndef __IDX_DEFINITIONS__
#define __IDX_DEFINITIONS__

#include <string.h>
#include <iostream.h>
#include <fstream.h>

const int IDX_SINGLE        = 0x01;
const int IDX_MULTIPLE      = 0x02;
const int IDX_FIRST         = 0x04;
const int IDX_SECOND        = 0x08;
const int IDX_DUPLICATES    = 0x10;
const int IDX_NODUPLICATES  = 0x20;

const int IDX_INFO_MAGIC    = 0xFEED;
const int IDX_LIST_MAGIC    = 0x1DEA;
const int FLD_LIST_MAGIC    = 0xBEAD;
const int FLD_INFO_MAGIC    = 0xBABE;

const int MAX_FIELDS_IN_INDEX = 10;
const int MAX_NAME_LENGTH = 300;

//---- Misc rountines...

void checkStream(ifstream&);
void checkStream(ofstream&);

typedef int TIndicies[MAX_FIELDS_IN_INDEX];
struct indexInfo
{
	int			idxType;
	int			idxFields[MAX_FIELDS_IN_INDEX];
	indexInfo*	nextIdx;

	// Moved the definition of the constructor into the .cc file
	// to stop complaints about it not getting expanded inline.

	indexInfo(int type, TIndicies indicies ); 
/*	{
		idxType = type;
		for(int i=0;i<MAX_FIELDS_IN_INDEX;i++)
		{
			idxFields[i] = indicies[i];
		}
		nextIdx = 0;
	}
*/
	~indexInfo();
friend ostream& operator << (ostream&, indexInfo&);
friend ofstream& operator << (ofstream&, indexInfo&);
};

// Holds the list of indicies
struct indexList
{
	int			numIndicies;
	indexInfo*	indicies;

	indexList() {numIndicies = 0; indicies = 0;}
	~indexList()
	{
		delete indicies;
	}
	long number(char* name);

friend ostream& operator << (ostream&, indexList&);
friend ofstream& operator << (ofstream&, indexList&);
friend ifstream& operator >> (ifstream&, indexList&);

};


// Holds the info on a field
struct fieldInfo
{
	int			fldType;
	int			fldSize;
	char*		fldName;
	fieldInfo*	nextFld;

	fieldInfo(char* name, int ty, int sz);
	~fieldInfo();
friend ostream& operator << (ostream&, fieldInfo&);
friend ofstream& operator << (ofstream&, fieldInfo&);

};

// Holds the list of fields

struct fieldList
{
	int		numFields;
	fieldInfo*	fields;
	fieldList() {fields = 0; numFields = 0;}

	~fieldList()
	{
		delete fields;
	}
friend ostream& operator << (ostream&, fieldList&);
friend ofstream& operator << (ofstream&, fieldList&);
friend ifstream& operator >> (ifstream&, fieldList&);

};

#endif
