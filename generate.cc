/**************************************************************************
 * Source Id :
 *
 * $Id: generate.cc,v 1.45 1993/11/05 12:45:05 kevinl Exp $
 *-------------------------------------------------------------------------
 * Project Notes :
 *
 *  Diamond Base
 *  ============
 *	A solid database implementation, spurred on by the continuing
 *  Metal (Lead) Base saga.
 *
 *  Project Team :
 *	  A. Davison
 *	  K. Lentin
 *	  D. Platt
 *
 *	Project Commenced : 05-02-1993
 *
 *-------------------------------------------------------------------------
 *  Module Notes :
 *
 *  Object header and code generator.
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: generate.cc,v $
// Revision 1.45  1993/11/05  12:45:05	kevinl
// Fixed problems that CC picked up (and some multi bugs)
//
// Revision 1.44  1993/11/03  12:19:27	kevinl
// Added ctype.h to generated code
//
// Revision 1.43  1993/11/03  12:09:11	davison
// Replaced missing brace in ichar comparision code.
//
// Revision 1.42  1993/11/03  11:51:31	davison
// Removed the default value from the (pKeyType,long) constructor.
//
// Revision 1.41  1993/11/03  10:04:46	kevinl
// Added ichar and utils.{h,cc}
//
// Revision 1.40  1993/10/28  00:40:08	davison
// Another missing backslash.
//
// Revision 1.39  1993/10/25  08:32:51	kevinl
// Let's try dbData constructors
//
// Revision 1.38  1993/10/24  15:35:45	kevinl
// Added derived class generation
//
// Revision 1.37  1993/10/24  09:14:26	kevinl
// Fixed unused idxList in genCons????
//
// Revision 1.36  1993/10/23  06:35:34	davison
// Changed bzero reference to memset.
//
// Revision 1.35  1993/10/19  14:27:37	kevinl
// Fixed constructors/retrievers
//
// Revision 1.34  1993/10/19  11:43:02	kevinl
// Default values
//
// Revision 1.33  1993/10/19  02:36:08	kevinl
// Added get to genConsDefn
//
// Revision 1.32  1993/10/19  01:06:40	davison
// Fixed a missing slash...
//
// Revision 1.31  1993/10/18  08:01:41	kevinl
// Added constructors and fixed some probs
//
// Revision 1.30  1993/09/27  01:15:44	kevinl
// Changed ORD_COMPARE to use < and >
//
// Revision 1.29  1993/09/26  06:40:32	kevinl
// Added dbData support
//
// Revision 1.28  1993/09/06  13:25:57	kevinl
// Removed a remnant hard coded classname, replaced with structName
//
// Revision 1.27  1993/08/29  12:56:27	kevinl
// Fixed problems of assigning relations to each other.
//
// Revision 1.26  1993/07/19  11:53:42	kevinl
// Removed some Borland warnings re incorrect assignments and a non-
// terminated string
//
// Revision 1.25  1993/07/11  09:42:05	kevinl
// Changed String to dbString
//
// Revision 1.24  1993/07/09  04:02:36	kevinl
// Fixed return in setUnique
// Nuked some ints
//
// Revision 1.23  1993/07/01  13:27:16	kevinl
// Pointers to uniques no longer returned. {has,get,set}Unqiue members
//
// Revision 1.22  1993/06/23  05:21:22	kevinl
// Mallocs are now in angular brackets
//
// Revision 1.21  1993/06/20  13:41:59	kevinl
// Fixed multiple mallocs
// Added String support
//
// Revision 1.20  1993/05/27  07:31:29	kevinl
// Some missing \'s which gcc -Wall didn't pick up!!!
//
// Revision 1.19  1993/05/26  00:58:24	kevinl
// Arrays for uniques and field lengths now not static
// Fixed inlines with multiple returns
// MALLOC_H_MISSING
//
// Revision 1.18  1993/05/12  02:19:25	kevinl
// Added verStr to generated class
// Fixed REAL_COMPARE
//
// Revision 1.17  1993/05/06  04:26:30	kevinl
// SASC for malloc.h
// Now uses key structures to align the keys
// Also fixed empty unique arrays
//
// Revision 1.16  1993/05/01  14:41:47	kevinl
// Conditional operator instead of trusting value of !=
//
// Revision 1.15  1993/04/27  07:10:23	kevinl
// Added uniqueArray and isUnique
// Comments
//
// Revision 1.14  1993/04/15  04:21:52	kevinl
// Moved malloc.h
//
// Revision 1.13  1993/04/14  14:17:15	kevinl
// Now we close the output streams
//
// Revision 1.12  1993/04/11  00:57:15	davison
// Added a dbError() cast operator to return the error status for the class.
//
// Revision 1.11  1993/04/08  15:40:23	kevinl
// Adjusted spacing in generated constructor
//
// Revision 1.10  1993/04/06  23:23:09	kevinl
// Bye Bye bcopy
//
// Revision 1.9  1993/04/04  23:59:34  kevinl
// Generate structs to hold aligned keys
//
// Revision 1.8  1993/04/01  11:23:23  kevinl
// Set unique field to -1 in constructor
//
// Revision 1.7  1993/03/30  14:39:02  kevinl
// Generate address of structure members so struct packing is not a factor
//
// Revision 1.6  1993/03/30  06:54:16  kevinl
// Added getUnique member
//
// Revision 1.5  1993/03/29  08:20:09  darrenp
// Added new malloc library support
//
// Revision 1.4  1993/03/25  22:20:10  davison
// Fixed last patch - needed a space after "public" in the
// class definition.
//
// Revision 1.3  1993/03/25  18:17:16  davison
// Make the generated class inherit the structure publicly.
//
// Revision 1.2  1993/03/24  06:17:54  kevinl
// fixed include file name, struct and class name
// Fixed duplicate/unused a loop variable counter, indentation etc
// Added theData() function
//
// Revision 1.1  1993/03/21  23:36:54  kevinl
// Initial revision
//
 **************************************************************************/

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <fstream.h>
#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <string.h>
#include <defs.h>
#include <utils.h>
#include <idxinfo.h>
#include <d_types.h>

strstream keyStr;

// mySizeOf:
// Since all our types are merely numbers, we need out own sizeof

long mySizeOf(int tp)
{
	switch (tp)
	{
		case D_SHORT: return sizeof(short);
		case D_USHORT: return sizeof(ushort);
		case D_DATA:
		case D_STRING:
		case D_LONG: return sizeof(long);
		case D_ULONG: return sizeof(ulong);
		case D_DOUBLE: return sizeof(double);
		case D_FLOAT: return sizeof(float);
		case D_MONEY: return sizeof(moneyType);
		case D_DATE: return sizeof(dateType);
		case D_ICHAR:
		case D_CHAR: return sizeof(char);
		case D_UNIQUE: return sizeof(uniqueType);
	}
	return 0;
}

// typeName:
// This is needed to output the correct code based on our form of types

char*
typeName(long tp, bool realData=false)
{
	static char tpName[20];

	switch (tp) {
		case D_SHORT: strcpy(tpName, "short"); break;
		case D_USHORT: strcpy(tpName, "ushort"); break;
		case D_DATA: strcpy(tpName, realData?"dbData":"long"); break;
		case D_STRING: strcpy(tpName, realData?"dbString":"long"); break;
		case D_LONG: strcpy(tpName, "long"); break;
		case D_ULONG: strcpy(tpName, "ulong"); break;
		case D_DOUBLE: strcpy(tpName, "double"); break;
		case D_FLOAT: strcpy(tpName, "float"); break;
		case D_MONEY: strcpy(tpName, "moneyType"); break;
		case D_DATE: strcpy(tpName, "dateType"); break;
		case D_CHAR: strcpy(tpName, "char"); break;
		case D_ICHAR: strcpy(tpName, "ichar"); break;
		case D_UNIQUE: strcpy(tpName, "uniqueType"); break;
		default: strcpy(tpName, "THISISABUG"); break;
	}
	return tpName;
}

// genUniqueArray:
// Generate the list of bool's that specify is a field is unique

void genUniqueArray(ofstream& of, indexList* idxList, fieldList* fldList)
{
	indexInfo* i = idxList->indicies;
	long u = -1;
	long fldNum = 0;

	// Do we have a unique field?

	for (fieldInfo* f = fldList->fields; f; f=f->nextFld, fldNum++)
		if (f->fldType == D_UNIQUE)
		{
			u = fldNum;
			break;
		}

	// No, leave
	if (u==-1)
	{
		for (long l=0; l < idxList->numIndicies; l++)
		{
			if (l)
				of << ",";
			of << "false";
		}
		return;
	}

	// Now go through the indicies
	while (i)
	{
		if (i != idxList->indicies) of << ","; // Add a comma to separate
		bool gotIt = false;

		for (int j=0; j<MAX_FIELDS_IN_INDEX && i->idxFields[j] != -1;  j++)
		{
			// Is this the unique field?
			if (u == i->idxFields[j])
			{
				gotIt = true;
				break;
			}
		}

		// output it
		of << (gotIt?"true":"false");
		i=i->nextIdx;
	}
}

// genArray:
// Generate the list of index sizes for the indexSize array

//void genArray(ofstream& of, indexList* idxList, fieldList* fldList)
void genArray(ofstream& of, char* className, indexList* idxList)
{
	for (long i=0; i < idxList->numIndicies; i++)
	{
		if (i)
			of << ",";
		of << "sizeof(_" << className << "_idxStr" << i << ")";
	}
#if 0
	indexInfo* i = idxList->indicies;

	while (i)
	{
		if (i != idxList->indicies) of << ","; // Add a comma to separate
		long len = 0;

		// This ugly loopy thing turns up a lot. It is because we store
		// field numbers in the index list, yet our fields are not directly
		// accessable by those numbers. Given the infrequency of use of the
		// gerenator compared to the rest of the system, there are more
		// important things to fix.

		for (int j=0; j<MAX_FIELDS_IN_INDEX && i->idxFields[j] != -1;  j++)
		{
			fieldInfo* f = fldList->fields;
			for (int k=0; k < i->idxFields[j]; k++)
				f=f->nextFld;

			// Add up the field length
			len+=f->fldSize;
		}

		// output it
		of << len;
		i=i->nextIdx;
	}
#endif
}

// genStructs:
// This generates the structure to store the relation data in.

void genStructs(ofstream& of, char* className, indexList* idxList, fieldList* fldList)
{
	indexInfo* i = idxList->indicies;
	int num=0;

	while (i)
	{
		// OK, here's another ugli-ish thing. For historical (or maybe
		// hysterical) reasons, the \n's are at the start of all these long
		// strings. One day, when I have lots of time and patience, I will put
		// them on the end of the strings, and get rid of the ugly \'s on the
		// ends of all the lines.

		of << "\
\ntypedef struct _" << className << "_idxStr" << num << " {\
";
		for (int ch=0; ch <=1; ch++)
			for (int j=0; j<MAX_FIELDS_IN_INDEX && i->idxFields[j] != -1;  j++)
			{
				fieldInfo* f = fldList->fields;
				for (int k=0; k < i->idxFields[j]; k++)
					f=f->nextFld;

				// This loop is executed twice. During the first pass, no
				// char types are included. These are added on the end during
				// the second pass. This is to solve alignment problems caused
				// by single chars in the middle of a series of word aligned
				// fields.
				if (ch ^ ((f->fldType != D_CHAR && f->fldType != D_ICHAR)?1:0))
				{
					of << "\
\n	" << typeName(f->fldType) << "  ";

					// This won't happen since they're not supported yet
					// but I typed it in by mistake so leave it here. It will
					// be needed later.
					if (f->fldType == D_STRING || f->fldType == D_DATA)
					{
						of << "_off_";
					}
					of << f->fldName;

					// Is this an array?
					if (f->fldSize != mySizeOf(f->fldType))
					{
						of << "[" << f->fldSize / mySizeOf(f->fldType) << "]";
					}
					of << ";";
				}
			}
		of << "\
\n};\
\n\
";
		i=i->nextIdx;
		num++;
	}
}

// genUnique:
// This generates 3 different methods. {get,set,has}Unique
// They manipulate the Unqiue member if it exists.
// The type parameter is number 0 1 2 for set get has

void genUnique(ofstream& of, fieldList* fldList, long type)
{
	if (type)
		of << "return";
	for (fieldInfo* f = fldList->fields; f; f=f->nextFld)
		if (f->fldType == D_UNIQUE)
		{
			if (!type) // set
				of << " " << f->fldName << " = uniq;";
			else if (type == 1) // get
				of << " " << f->fldName << ";";
			else // has
				of << " true;";
			return;
		}
	// There is no unique field
	if (!type) // set
		of << ";";
	else if (type == 1) // get
		of << " -1;";
	else // has
		of << " false;";
}

// genConsDecls:
// Generate header declarations for the constructors.

void genConsDecls(ofstream& of, char* className, fieldList* fldList, indexList* consList, bool& voidDone)
{
	indexInfo* c = consList->indicies;

	while (c)
	{
		of << "\
\n	" << className << "(";

		long count = 0;
	for (int j=0; j<MAX_FIELDS_IN_INDEX && c->idxFields[j] != -1;  j++)
	{
	    fieldInfo* f = fldList->fields;
	    for (int k=0; k < c->idxFields[j]; k++)
		f=f->nextFld;
			if (j)
				of << ", ";
			of << typeName(f->fldType,true);
			if (f->fldSize != mySizeOf(f->fldType))
				of << "*";
			of << " _cons_" << f->fldName;
			count++;
	}

		if (!count)
			voidDone = TRUE;
		of << ");";

	c=c->nextIdx;
	}
}

// genConsDefns:
// Generate definitions for the constructors.

void genConsDefns(ofstream& of, char* className, char* structName, fieldList* fldList, indexList* consList)
{
	indexInfo* c = consList->indicies;

	while (c)
	{
		of << "\
\n" << className << "::" << className << "(";

	for (int j=0; j<MAX_FIELDS_IN_INDEX && c->idxFields[j] != -1;  j++)
	{
	    fieldInfo* f = fldList->fields;
	    for (int k=0; k < c->idxFields[j]; k++)
		f=f->nextFld;
			if (j)
				of << ", ";
			of << typeName(f->fldType, true);
			if (f->fldSize != mySizeOf(f->fldType))
				of << "*";
			of << " _cons_" << f->fldName;
	}

		of << ") : diaRel(\"" << className << "\")," << structName << "() {\
\n	commonInit();\
";

	for (j=0; j<MAX_FIELDS_IN_INDEX && c->idxFields[j] != -1;  j++)
	{
	    fieldInfo* f = fldList->fields;
	    for (int k=0; k < c->idxFields[j]; k++)
		f=f->nextFld;
			if (f->fldSize / mySizeOf(f->fldType) == 1)
				of << "\
\n	" << f->fldName << " = _cons_" << f->fldName << ";";
			else if (f->fldType == D_CHAR || f->fldType == D_ICHAR)
				of << "\
\n	strNcpy(" << f->fldName << ",_cons_" << f->fldName << "," << f->fldSize << ");";
			else of << "\
\n	bcopy(_cons_" << f->fldName << "," << f->fldName << "," << f->fldSize << ");";
	}

		if (c->idxType != -1)
			of << "\
\n	get(" << c->idxType << ");\
";
		of << "\
\n};\
\n\
";

	c=c->nextIdx;
	}
}

// genGetSetCases:
// This is used to generated the case's contained in the switches in getKey
// and setKey.

void genGetSetCases(ofstream& of, char* className, indexList* idxList, fieldList* fldList, bool isGet)
{
	indexInfo* i = idxList->indicies;
	long caseNum = 0;

	while (i)
	{
		of << "\
\n		case " << caseNum++ << ":\
\n		{\
";
		long keyOff = 0;
		long forNum = 0;
		for (int j=0; j<MAX_FIELDS_IN_INDEX && i->idxFields[j] != -1;  j++)
		{
			bool tab;
			fieldInfo* f = fldList->fields;
			for (int k=0; k < i->idxFields[j]; k++)
			{
				f=f->nextFld;
			}

			// Got the right field

			// If this is a CHAR type of more than one char (a string) then
			// use strncpy to get the string. Makes things a little quicker.
			// NB Earlier I commented about not worrying about efficiency of
			// generate. This is true, however, the efficiency of the code that
			// genarate produces is quite important.

			if ((f->fldType == D_CHAR || f->fldType == D_ICHAR) && f->fldSize != 1)
				if (isGet)
					of << "\
\n			strNcpy(((_" << className << "_idxStr" << caseNum-1 << "*)buffer)->" << f->fldName << ", " << f->fldName << ", " << f->fldSize << ");\
";
				else
					of << "\
\n			strNcpy(" << f->fldName << ", ((_" << className << "_idxStr" << caseNum-1 << "*)buffer)->" << f->fldName << ",  " << f->fldSize << ");\
";
			else
			{
				// If this is an array then we must loop around copying
				// each of the elements individually.
				// Why not use bcopy of the whole block? Because the code was
				// 50 times slower when we did that!

				// Note that we use the variable a##forNum so that the loop
				// variables to not clash, given the unfortunate scope rules
				// of C++ and for loops.

				if (f->fldSize != mySizeOf(f->fldType))
				{
					of << "\
\n			for (int a" << forNum << "=0;a" << forNum << "<" << f->fldSize << ";a"
<< forNum++ << "+=" << mySizeOf(f->fldType) << ")\
";
					// The tab is just to make sure that things all line up
					// correctly in the generated code. We might as well make
					// things look good.

					tab = true;
				}
				else
					tab = false;

				if (isGet)
					of << "\
\n		" << (tab?"\t":"") << " ((_" << className << "_idxStr" << caseNum-1 << "*)buffer)->" << f->fldName << " = " << f->fldName << ";";
				else
					of << "\
\n		" << (tab?"\t":"") << " " << f->fldName << " = ((_" << className << "_idxStr" << caseNum-1 << "*)buffer)->" << f->fldName << ";";

			}

			// The offset into the key where we cast the data we want from
			keyOff += f->fldSize;
		}
		of << "\
\n			break;\
\n		}\
";
		i = i->nextIdx;
	}
}

// genCompareCases:
// This does a simialr job to genGetSetCases except it's used for the
// compare function.

void genCompareCases(ofstream& of, char* className, indexList* idxList, fieldList* fldList)
{
	indexInfo* i = idxList->indicies;
	long caseNum = 0;

	while (i)
	{
		of << "\
\n		case " << caseNum++ << ":\
\n		{\
";
		long keyOff = 0;
		long forNum = 0;
		for (int j=0; j<MAX_FIELDS_IN_INDEX && i->idxFields[j] != -1;  j++)
		{
			bool tab;
			fieldInfo* f = fldList->fields;
			for (int k=0; k < i->idxFields[j]; k++)
				f=f->nextFld;

			if (f->fldType == D_CHAR && f->fldSize != 1)
				of << "\
\n			if (res=strncmp(" << f->fldName << ", ((_" << className << "_idxStr" << caseNum-1 << "*)key)->" << f->fldName << ", " << f->fldSize << "))\
\n				break;\
";
			else
			if (f->fldType == D_ICHAR && f->fldSize != 1)
				of << "\
\n			if (res=strncasecmp(" << f->fldName << ", ((_" << className << "_idxStr" << caseNum-1 << "*)key)->" << f->fldName << ", " << f->fldSize << "))\
\n				break;\
";
			else
			{
				if (f->fldSize != mySizeOf(f->fldType))
				{
					of << "\
\n			for (int a" << forNum << "=0;a" << forNum << "<" << f->fldSize << ";a"
<< forNum++ << "+=" << mySizeOf(f->fldType) << ")\
";
					tab = true;
				}
				else
					tab = false;

				// We use the generated compare functions
				of << "\
\n		" << (tab?"\t":"") << " if (res=compare_" << typeName(f->fldType) << "((char*)(&" << f->fldName << "), (char*)(&((_" << className << "_idxStr" << caseNum-1 << "*)key)->" << f->fldName << ")))\
\n			" << (tab?"\t":"") << " goto end_compare;\
";
			}
			keyOff += f->fldSize;
		}

		of << "\
\n			break;\
\n		}\
";
		i = i->nextIdx;
	}
	of << "\
\n#pragma warn .pia\
";
}

// generateStart:
// This starts off the generation process by forming the file names and
// creating the relevant streams. It also outputs all the non-schema-depandant
// stuff that starts each file.

bool generateStart(ofstream& hf, ofstream& cf, char* filename)
{
	char *s;
	char *incname;
	char *fle = strdup(filename);
	char *file = fle;

	// Strip off all leading paths (Unix and PC style)

	if ((s=strrchr(file, '/')) != 0)
		file= s+1;
	if ((s=strrchr(file, '\\')) != 0)
		file= s+1;

	incname = new char[strlen(filename)+5];
	strcpy(incname, filename);
	strcat(incname, ".h");

	hf.open(incname);
	if (!hf)
	{
		// Couldn't open, clean up and go.
		delete fle;
		delete incname;
		return false;
	}

	char *codename = new char[strlen(filename)+6];
	strcpy(codename, filename);
	strcat(codename, ".cc");
	cf.open(codename);
	if (!cf)
	{
		delete fle;
		delete incname;
		delete codename;
		return false;
	}

	// The top of the include file
	hf << "\
\n//\
\n// Include file for " << filename << ".s schema file.\
\n//\
\n// Automatically generated: Generator Revison $Rev$\
\n//\
\n\
\n#ifndef __" << file << "_H_\
\n#define __" << file << "_H_\
\n\
\n#include <diarel.h>\
\n#include <utils.h>\
\n\
\n#include \"" << file << "_s.h\"\
";

	// The .cc class definition file.
	// The ORD_COMPARE and REAL_COMPARE defines are used to declare COMPARE
	// functions for each of our types. I leave them in here, and not in the
	// library or somewhere else so that these files are fairly self
	// contained.
	//
	// Hopefully, most compilers can inline these nicely for us.

	cf << "\
\n//\
\n// Class definition file for " << filename << ".s schema file.\
\n//\
\n// Automatically generated: Generator Revison $Rev$\
\n//\
\n\
\n#include <iostream.h>\
\n#include <stdlib.h>\
\n#include <string.h>\
\n#include <stdio.h>\
\n#include <ctype.h>\
\n#include \"" << file << ".h\"\
\n\
\nchar* strNcpy(char* d, char* s, int n);\
\n#define ORD_COMPARE(t) \\\
\n	inline long compare_##t(pKeyType k1, pKeyType k2) \\\
\n	{ \\\
\n		t _t1 = *((t *)k1); \\\
\n		t _t2 = *((t *)k2); \\\
\n		return (_t1==_t2)?0:((_t1<_t2)?-1:1); \\\
\n	} \
\n\
\n#define REAL_COMPARE(t) \\\
\n	inline long compare_##t(pKeyType k1, pKeyType k2) \\\
\n	{ \\\
\n		t _t1 = *((t *)k1); \\\
\n		t _t2 = *((t *)k2); \\\
\n		return (_t1==_t2)?0:((_t1<_t2)?-1:1); \\\
\n	}\
\n\
\ninline long compare_ichar(pKeyType k1, pKeyType k2)\
\n{\
\n	char _t1 = tolower(*((char*)k1));\
\n	char _t2 = tolower(*((char*)k2));\
\n	return (_t1==_t2)?0:((_t1<_t2)?-1:1);\
\n}\
\n\
\nORD_COMPARE(short);\
\nORD_COMPARE(ushort);\
\nORD_COMPARE(long);\
\nORD_COMPARE(ulong);\
\nORD_COMPARE(dateType);\
\nORD_COMPARE(char);\
\nORD_COMPARE(uniqueType);\
\nREAL_COMPARE(float);\
\nREAL_COMPARE(double);\
\nREAL_COMPARE(moneyType);\
";

	// Clean up and go.
	delete fle;
	delete incname;
	delete codename;
	return true;
}

// generateClass:
// This does all the work generating a class. It uses all the above
// functions to keep things neat (well, sort of neat anyway).

bool generateClass(ofstream& hf, ofstream& cf, long recLen, indexList* idxList, fieldList* fldList, char* structName, char* className, int stringCount, indexList* consList, char** defaults)
{
	if (!hf) return false;

	hf << "\
\n//\
\n// Declaration of class for " << className << "\
\n//\
";
	genStructs(hf, className, idxList, fldList);
	hf << "\
\n\
\nclass " << className << " : \
\n		public diaRel,\
\n		public " << structName;
	if (stringCount)
		hf << ",\
\n		public _Strings_" << structName << "\
";
	hf << "\
\n{\
\npublic:\
\n\
\n	inline virtual operator void*() { return (void*)((" << structName << "*)this); }\
\n	inline virtual operator char*() { return (char*)((" << structName << "*)this); }\
\n	inline virtual char* theData() { return (char*)((" << structName << "*)this); }\
\n	inline virtual long getDataLength(void) { return " << recLen << "; };\
\n#ifndef __CC\
\ninline\
\n#endif\
\n		virtual long getKeyLength(long keyNum=0);\
\n	inline virtual pKeyType getKey(long keyNum=0);\
\n	virtual long compare(pKeyType key, long keyNum=0);\
";
	bool voidCons = FALSE;
	genConsDecls(hf, className, fldList, consList, voidCons);
	if (!voidCons)
		hf << "\
\n	" << className << "();\
";
	hf << "\
\n	" << className << "(pKeyType newKey, long keyNum);\
\n	" << className << "& operator=(" << className << "& X);\
\n	void commonInit(void);\
\n	void setKey(pKeyType newKey, long keyNum=0);\
\n	long getUnique(void) { ";
	genUnique(hf, fldList, 1);
	hf << " }\
\n	bool hasUnique(void) { ";
	genUnique(hf, fldList, 2);
	hf << " }\
\n	void setUnique(long uniq) { ";
	genUnique(hf, fldList, 0);
	hf << " }\
\n	bool isUnique(long keyNum=0);\
\n	long numStrings(void) { return " << stringCount << "; }\
";
	if (stringCount)
		hf << "\
\n	_db_strStruct strArray[" << stringCount << "];\
\n	dbData* getString(long num) { return strArray[num].theString; }\
\n	long* getLong(long num) { return strArray[num].theLong; }\
";
	hf << "\
\n	_db_strStruct* getArray(void) { return \
";
	hf << (stringCount?"strArray":"0") << ";}\
\n	operator dbError();\
\n	char* verStr(void) { return \"$Id: generate.cc,v 1.45 1993/11/05 12:45:05 kevinl Exp $\"; }\
\n};\
";

	if (!cf) return false;

	cf << "\
\n\
\n//\
\n// Definitions for class " << structName << "\
\n//\
\nvoid " << className << "::commonInit(void)\
\n{\
\n	memset((" << structName << "*)this, 0, sizeof(" << structName << "));\
\n	setUnique(-1);\
";
	int i=0,j=0;
	for (fieldInfo *f = fldList->fields; f; f=f->nextFld, j++) {
		if (defaults[j])
			if ((f->fldType == D_CHAR || f->fldType == D_ICHAR) && f->fldSize != 1)
				cf << "\
\n	strNcpy(" << f->fldName << ", " << defaults[i] << ", " << f->fldSize << ");\
";
			else
				cf << "\
\n	" << f->fldName << " =" << defaults[j] << ";";

		if (f->fldType == D_STRING || f->fldType == D_DATA)
			cf << "\
\n	_off_" << f->fldName << "=0;\
\n	strArray[" << i++ << "] = _db_strStruct(&_off_" << f->fldName << ",&" << f->fldName << ");\
";
	}
	cf << "\
\n}\
\n\
";
	genConsDefns(cf, className, structName, fldList, consList);
	if (!voidCons)
		cf << "\
\n" << className << "::" << className << "() : diaRel(\"" << className << "\")," << structName << "()\
\n{\
\n	commonInit();\
\n}\
";
	cf << "\
\n\
\n" << className << "::" << className << "(pKeyType newKey, long numKey) : diaRel(\"" << className << "\"), " << structName << "()\
\n{\
\n	commonInit();\
\n	setKey(newKey, numKey);\
\n}\
\n\
\n" << className << "& " << className << "::operator=(" << className << "& X)\
\n{\
\n	if (&X != this) {\
\n		diaRel::operator=(X);\
\n		(" << structName << ")(*this) = (" << structName << ")(X);\
\n	}\
\n	return *this;\
\n}\
\n\
\nbool " << className << "::isUnique(long keyNum)\
\n{\
\n	static const bool uniqueFields[" << idxList->numIndicies << "] = {";

	genUniqueArray(cf, idxList, fldList);

	cf << "};\
\n	if (keyNum==-1)\
\n		return false;\
\n	else\
\n		return uniqueFields[keyNum];\
\n}\
\n\
\nlong " << className << "::getKeyLength(long keyNum)\
\n{\
\n	static const long keyLengths[" << idxList->numIndicies << "] = {";

	genArray(cf, className, idxList);

	cf << "};\
\n	if (keyNum==-1)\
\n		return 0;\
\n	else\
\n		return keyLengths[keyNum];\
\n}\
\n\
\npKeyType " << className << "::getKey(long keyNum)\
\n{\
\n	char* buffer = new char[getKeyLength(keyNum)];\
\n\
\n	switch (keyNum)\
\n	{";

	genGetSetCases(cf, className, idxList, fldList, true);

	cf << "\
\n	}\
\n	return (pKeyType)buffer;\
\n}\
\n\
\nvoid " << className << "::setKey(pKeyType buffer, long keyNum)\
\n{\
\n	switch (keyNum)\
\n	{";

	genGetSetCases(cf, className, idxList, fldList, false);

	cf << "\
\n	}\
\n}\
\n\
\n" << className << "::operator dbError()\
\n{\
\n	return status;\
\n}\
\n\
\nlong " << className << "::compare(pKeyType key, long keyNum)\
\n{\
\n	//char* myKey = (char*)getKey(keyNum);\
\n	long res=0;\
\n\
\n	switch (keyNum)\
\n	{\
\n#pragma warn -pia\
";
	genCompareCases(cf, className, idxList, fldList);

	cf << "\
\n	}\
\n\
\nend_compare:\
\n\
\n	//delete myKey;\
\n	return res;\
\n}\
";
	return true;
}

// generateEnd:
// End off the include file and close the streams.

bool generateEnd(ofstream& hf, ofstream& cf)
{
	if (!hf) return false;
	if (!cf) return false;

	hf << "\
\n\
\n#endif\
";
	hf.close();
	cf.close();
	return true;
}

void genDeriveCons(ofstream& of, dbString base, dbString derive, fieldList* fldList, indexList* consList, bool doCode)
{
	indexInfo* c = consList->indicies;

	while (c)
	{
		if (doCode)
			of << "\
\n" << derive << "::" << derive << "(";
		else
			of << "\
\n	" << derive << "(";

	for (int j=0; j<MAX_FIELDS_IN_INDEX && c->idxFields[j] != -1;  j++)
	{
	    fieldInfo* f = fldList->fields;
	    for (int k=0; k < c->idxFields[j]; k++)
		f=f->nextFld;
			if (j)
				of << ", ";
			of << typeName(f->fldType);
			if (f->fldSize != mySizeOf(f->fldType))
				of << "*";
			of << " _cons_" << f->fldName;
	}

		of << ")";

		if (doCode)
		{
			of << " : " << base << "(";
			for (j=0; j<MAX_FIELDS_IN_INDEX && c->idxFields[j] != -1;  j++)
			{
				fieldInfo* f = fldList->fields;
				for (int k=0; k < c->idxFields[j]; k++)
					f=f->nextFld;
				if (j)
					of << ", ";
				of << " _cons_" << f->fldName;
			}
			of << ")\
\n{\
\n	// Something in here would be nice\
\n}\
\n\
";
		}
		else
		{
			of << ";";
		}

	c=c->nextIdx;
	}
}

bool
generateDerived(dbString base, dbString derive, dbString deriveName, fieldList* fldList, indexList* consList)
{
	dbString s;
	s = deriveName + ".h";
	ofstream hf(s);
	s = deriveName + ".cc";
	ofstream cf(s);

	cf << "\
\n//\
\n// Derived class " << derive << " from base class " << base << ".\
\n// Generated by DiamondBase\
\n// $Id: generate.cc,v 1.45 1993/11/05 12:45:05 kevinl Exp $\
\n\
";

	hf << "\
\n//\
\n// Derived class " << derive << " from base class " << base << ".\
\n// Generated by DiamondBase\
\n// $Id: generate.cc,v 1.45 1993/11/05 12:45:05 kevinl Exp $\
\n\
\nclass " << derive << " : public " << base << "{\
";
	genDeriveCons(hf, base, derive, fldList, consList, false);
	genDeriveCons(cf, base, derive, fldList, consList, true);

	hf << "\
\n};\
";
	return true;
}
