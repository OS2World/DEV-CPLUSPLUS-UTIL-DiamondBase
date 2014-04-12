/**************************************************************************
 * Source Id :
 *
 * $Id: dispr.cc,v 1.4 1993/05/26 01:01:39 kevinl Exp $
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
 *		Relation Browser
 *
 *			Dumps out the format of a relation and it's associated 
 *	index information from the database file.
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dispr.cc,v $
// Revision 1.4  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.3  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.2  1993/03/15  19:26:55  davison
// Implemented a few nice things, like more than 2 index fields, line number
// reporting on syntax errors, and usage reporting.
//
// Just for you, Daz :-)
//
// Revision 1.1  1993/03/15  19:25:53  davison
// Initial revision
//
 **************************************************************************/
#include <iostream.h>
#include <fstream.h>
#include "idxinfo.h"
#include "d_types.h"
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

//-----------------------
//  Relation List program
//

void dispInfo(fieldList& list,indexList& idxList);

// Usage information.

void usage(void)
{
	cerr << "Usage : dispr <relation filename>" << endl;
	exit(1);
}


main(int argc, char* argv[])
{
	if (argc != 2)
		usage();

	char name[200];
	strcpy(name,argv[1]);

	// Strip the extension and replace it with ".db".

	char* dot = strchr(name,'.');
	if(dot)
		*dot=(char)0;
	strcat(name,".db");
	
	// Just define a few things...

	ifstream input(name);
	checkStream(input);
	int headerLength;
	fieldList fldList;
	indexList idxList;

	// Now read the database.

	// First the length of the header info.

	input.read((char*)&headerLength,sizeof(int));
	// Then get the field information
	input >> fldList;
	// Then get the index information.
	input >> idxList;

	// And finally diaplay it.

	dispInfo(fldList,idxList);

	return 0;
}

//--------------------------------------------------
// Display the information once it's been retrieved.
//
// Buggered if I'm going to comment this. Let's see how it goes...



void dispInfo(fieldList& list,indexList& idxList)
{
	fieldInfo* curFld = list.fields;

	cout << "\tField Name \tField Type\t\tField Size (in bytes)" << endl;
	cout << "===============================================================================" << endl;
	while (curFld)
	{
		cout << "\t" << curFld->fldName << "\t\t";
		switch (curFld->fldType)
		{
			case 3:
				cout << "short";
				if(curFld->fldSize != sizeof(short))
					cout << "[" << curFld->fldSize/sizeof(short) << "]";
				else
					cout << "\t";
				break;
			case 4:
				cout << "unsigned short";
				if(curFld->fldSize != sizeof(unsigned short))
					cout << "[" << curFld->fldSize/sizeof(unsigned short) << "]";
				else
					cout << "\t";
				break;
			case 1:
				cout << "long";
				if(curFld->fldSize != sizeof(long))
					cout << "[" << curFld->fldSize/sizeof(long) << "]";
				else
					cout << "\t";
				break;
			case 2:
				cout << "unsigned long";
				if(curFld->fldSize != sizeof(unsigned long))
					cout << "[" << curFld->fldSize/sizeof(unsigned long) << "]";
				else
					cout << "\t";
				break;
			case 5:
				cout << "double";
				if(curFld->fldSize != sizeof(double))
					cout << "[" << curFld->fldSize/sizeof(double) << "]";
				else
					cout << "\t";
				break;
			case 6:
				cout << "float";
				if(curFld->fldSize != sizeof(float))
					cout << "[" << curFld->fldSize/sizeof(float) << "]";
				else
					cout << "\t";
				break;
			case 7:
				cout << "money";
				if(curFld->fldSize != sizeof(moneyType))
					cout << "[" << curFld->fldSize/sizeof(moneyType) << "]";
				else
					cout << "\t";
				break;
			case 8:
				cout << "date";
				if(curFld->fldSize != sizeof(dateType))
					cout << "[" << curFld->fldSize/sizeof(dateType) << "]";
				else
					cout << "\t";
				break;
			case 9:
				cout << "char";
				if(curFld->fldSize != sizeof(char))
					cout << "[" << curFld->fldSize/sizeof(char) << "]";
				else
					cout << "\t";
				break;
			case 10:
				cout << "unique";
				if(curFld->fldSize != sizeof(uniqueType))
					cout << "[" << curFld->fldSize/sizeof(uniqueType) << "]";
				else
					cout << "\t";
				break;
			default:
				cerr << "Invalid data type in relation file. Aborting." << endl;
				exit(1);
		}
		
		cout << "\t\t" << curFld->fldSize << endl;
		curFld = curFld->nextFld;
	}

	//----------------------------------------------
	// Now display index information.

	cout << endl << endl 
	     << "\tIndexed Field(s)\t\t\tAllow Duplicates ?" << endl
	     << "==============================================================================" << endl;

	indexInfo* curIdx = idxList.indicies;
	while (curIdx)
	{
		curFld = list.fields;
		for(int i=0;i<curIdx->idxFields[0];i++)
			curFld=curFld->nextFld;
		cout << "\t" << curFld->fldName;

		// Now the second index (if necessary).

		if(curIdx->idxFields[1] != -1)
		{
			cout << ", ";
			curFld = list.fields;
			for(int i=0;i<curIdx->idxFields[1];i++)
				curFld=curFld->nextFld;
			cout << curFld->fldName;
		}
		else
			cout << "\t";
		if(curIdx->idxFields[2] != -1)
		{
			cout << ", ";
			curFld = list.fields;
			for(int i=0;i<curIdx->idxFields[2];i++)
				curFld=curFld->nextFld;
			cout << curFld->fldName;
		}
		else
			cout << "\t";

		if(curIdx->idxFields[3] != -1)
		{
			for(int q=3;q<MAX_FIELDS_IN_INDEX && curIdx->idxFields[q] != -1;q++)
			{
				cout << ", ";
				curFld = list.fields;
				for(int i=0;i<curIdx->idxFields[q];i++)
					curFld=curFld->nextFld;
				cout << curFld->fldName;
			}
			cout << endl << "\t\t\t";
		}
		cout << "\t\t\t";
		if((curIdx->idxType) & IDX_DUPLICATES)
			cout << "yes" << endl;
		else
			cout << "no" << endl;
		curIdx=curIdx->nextIdx;
	}
}

