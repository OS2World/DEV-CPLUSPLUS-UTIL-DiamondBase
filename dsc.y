%{
#define VERSION_NUMBER "0.1a"

/**************************************************************************
 * Source Id :
 *
 * $Id: dsc.y,v 1.40 1993/11/03 10:04:46 kevinl Exp $
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
 *		Diamond Base Relation Compiler.
 *
 *			This creates the relation structure definitions and initialises
 *	the database for that relation.
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dsc.y,v $
 * Revision 1.40  1993/11/03  10:04:46	kevinl
 * Added ichar and utils.{h,cc}
 *
 * Revision 1.39  1993/11/02  03:41:35	davison
 * Fixed clear() reference.
 *
 * Revision 1.38  1993/10/25  08:07:10	kevinl
 * Let's try dbString constructors
 *
 * Revision 1.37  1993/10/24  15:35:24	kevinl
 * Reveresed command line arguments
 * Added -g
 *
 * Revision 1.36  1993/10/19  14:27:37	kevinl
 * Fixed constructors/retrievers
 *
 * Revision 1.35  1993/10/19  11:50:27	kevinl
 * Assignment/default values/constructors
 *
 * Revision 1.34  1993/10/19  08:27:23	davison
 * Added assign token
 *
 * Revision 1.33  1993/10/18  08:01:41	kevinl
 * Added constructors and fixed some probs
 *
 * Revision 1.32  1993/10/10  03:05:23	davison
 * Added grammar prototypes for relational links
 *
 * Revision 1.31  1993/10/10  01:48:16	kevinl
 * Fixed record length bug
 *
 * Revision 1.30  1993/09/26  06:40:32	kevinl
 * Added dbData support
 *
 * Revision 1.29  1993/07/11  09:42:05	kevinl
 * Changed String to dbString
 *
 * Revision 1.28  1993/06/23  05:21:22	kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.27  1993/06/20  13:43:57	kevinl
 * Added some filename length/extension limitation support
 * Added String support
 *
 * Revision 1.26  1993/05/26  00:59:16	kevinl
 * moved 'delete output' to prevent file contention
 * Fixed output of ULONG (although code is no longer used)
 *
 * Revision 1.25  1993/05/07  13:54:35	kevinl
 * Added outputFieldList so fields were in correct order for alignment
 *
 * Revision 1.24  1993/05/03  01:35:24	kevinl
 * Comparing chars to 0 now, not -1
 *
 * Revision 1.23  1993/04/28  11:31:40	kevinl
 * Fixed up .ds extension
 *
 * Revision 1.22  1993/04/27  07:06:47	kevinl
 * Removed some dead code
 * Changed a static array to a new
 * Comments
 *
 * Revision 1.21  1993/04/15  07:59:12	kevinl
 * Removed malloc include
 * Set ind btree to open, not closed
 *
 * Revision 1.20  1993/04/15  04:21:30	kevinl
 * Added an extra delete output
 * Moved malloc.h
 *
 * Revision 1.19  1993/04/01  05:39:59	kevinl
 * Fixed filename stripping
 * Fixed unused ind bTree
 *
 * Revision 1.18  1993/04/01  01:35:41	davison
 * Updated usage() reporting.
 *
 * Revision 1.17  1993/04/01  00:52:02	davison
 * A little more work on the error stuff finished.
 *
 * Revision 1.15  1993/03/30  10:04:55	davison
 * Fixed the makeSource conditions.
 *
 * Revision 1.14  1993/03/30  07:56:27	davison
 * Added -C option (generate source code only)
 *	 -D option (generate databases only)
 *	 -C -D option (parse only - no output)
 *
 * Revision 1.13  1993/03/30  06:52:48	kevinl
 * Only one unique field allowed
 *
 * Revision 1.12  1993/03/28  05:19:42	root
 * Fixed error codes.
 *
 * Revision 1.11  1993/03/26  10:01:24	davison
 * Fixed field traversal in the index generation stuff.
 *
 * Revision 1.10  1993/03/25  22:19:06	davison
 * Fixed last patch - needed a semicolon.
 * Made structure constructor do nothing.
 *
 * Revision 1.9  1993/03/25  18:17:47  davison
 * Make data members in generated structure public.ly accessible.
 *
 * Revision 1.8  1993/03/24  06:17:16  kevinl
 * Added source path
 * Adjusted struct, class and include file names
 *
 * Revision 1.7  1993/03/21  23:36:31  kevinl
 * Changed index enum to start at -1
 * Added in object class code generation
 *
 * Revision 1.6  1993/03/21  18:08:58  davison
 * Fixed RECORD_SIZE increment problems.
 *
 * Revision 1.5  1993/03/21  05:30:34  kevinl
 * Fixed linked list traversal in installIndex
 *
 * Revision 1.4  1993/03/21  01:10:39  davison
 * Now closes the relation description correctly.
 * Also fixes the "2-state file size" bug by calling the void constructor
 * for recServer.
 *
 * Revision 1.3  1993/03/20  15:02:29  davison
 * More changes to keep Daz happy...
 *
 * (i) Path specification for db files created.
 * (ii) Path specification for schema files.
 * (iii) -H flag to generate only the header files.
 *
 * Revision 1.2  1993/03/15  19:06:55  davison
 * Implemented a few nice things, like more than 2 index fields, line number
 * reporting on syntax errors, and usage reporting.
 *
 * Just for you, Daz :-)
 *
 * Revision 1.1  1993/03/15  19:02:32  davison
 * Initial revision
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#include <iostream.h>
#include <string.h>
#include <fstream.h>
#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include <idxinfo.h>
#include <d_types.h>
#include <rserv.h>
#include <btree.h>
#include <generate.h>
#include <mserv.h>

int RECORD_LENGTH = 0;
const int BUCKET_SIZE	= 512;

// Lots and lots of globals . Don't you just love this daz ! :-)

ofstream* relDesc = 0;	// Output stream for the relation structure header.
ofstream ofHead;		// Output stream for object header file.
ofstream ofCode;		// Output stream for object code file.
int	relCount = 0;		// The number of relations compiled
int fldSize=0;			// The size for the current field.
int fldType=0;			// The type of the current field.
char fldName[300];		// The name of the current field.
char** indexName=0;		// The names of all the indexes.
char** defaults=0;		// The defaults for all the fields.
dbString fldDef;		// The current field default;
fieldList* fldList;		// The list of fields.
indexList* idxList;		// The list of indicies.
indexList* consList;	// The list of constructors
int fldInfoSize=0;		// The size of the field data.
int idxInfoSize=0;		// The size of the index data.
int consListSize=0;		// The size of the constructor data.
char relName[300];		// The name of the current relation.
char fileName[100];		// The name of the current output file.
char className[300];	// The name of the object.
dbString base;			// -G strings
dbString derive;
dbString deriveName;
bool deriveDone;		// Have we generated derived classes?

int idxType;			// The type of the current index.
char idxNames[MAX_FIELDS_IN_INDEX][MAX_NAME_LENGTH];
						// The name of the current primary index.
strstream* idxStr;		// The string to be constructed for the enumeration.

int linenum=1;			// The number of the line being parsed.
int	fieldNum;			// Used to keep track of the number of fields used in
						// an index.
int indexNum=0; 		// The number of indexes.
int unique;				// Do we have a unique field?
int stringCount;		// How many string fields

int headersOnly=0;		// Should only the headers be produced ?
int	makeSource=0;
int makeDatabases=0;
char outputPath[300];	// Output pathname.
char sourcePath[300];	// Source pathname.
char inputPath[300];	// Input pathname.

//------------------------------------------------
// This creates a header for the structure headers
// which basically plugs diamond base and gives
// version number information.

void descHeader(ostream* s, char* name)
{

	*s << "//" << endl << "//   Relation Structure Description for schema file " << name << endl;
	*s << "//" << endl;
	*s <<"// This file has been automatically generated using the Schema"<<endl;
	*s << "// Compiler provided with the Diamond Base package." << endl;
	*s << "//" << endl;
	*s << "//    Diamond Base : Version " << VERSION_NUMBER << endl;
	*s << endl;
	*s << "#include <d_types.h>" << endl;
	*s << endl << endl;
}

//----------------------------------------------
// This creates an empty file and puts in the header info for the relation.
// This file will later be used by the recServer for the relation

void createEmptyRelation(char* fname, int overwrite)
{
	char theName[200];
	recServer theRecServer;

	if (outputPath[0] != 0)
		sprintf(theName,"%s%s",outputPath,fname);
	else
		sprintf(theName,"%s",fname);

	// This all makes sure we don't overwrite an old file by mistake.
	// We it truncates files, it doesn't seem to do so properly. This seems
	// to be a library fault.

#ifdef __BORLANDC__
	ofstream *output = new ofstream(theName,ios::binary|ios::noreplace);
#else
	ofstream *output = new ofstream(theName,ios::noreplace);
	delete output;
	output = new ofstream(theName,ios::noreplace);
#endif
	if(!*output)
	{
#ifdef DEBUG
		cout << "Didn't open output file first time." << endl;
#endif
		if (!overwrite)
		{
			cout <<"Error : relation file " << theName <<
		   "already exists. Use -i option to overwrite." << endl;
			exit(1);
		}
		delete output;
#ifdef __BORLANDC__
		output = new ofstream(theName,ios::binary);
#else
		output = new ofstream(theName);
#endif
		if (!*output)
		{
			cout <<"Error writing to file "<<theName <<
			       ". Aborting." << endl;
			exit(1);
		}
	}
	// Firstly figure out how long the header is for this file.
	//
	int headerLength =
			sizeof(int) +		// Length of the header
			2*sizeof(int) + 	// The field list header
			fldInfoSize +		// The field list info
			2*sizeof(int) + 	// The index list header
			idxInfoSize;		// The index list size

#ifdef DEBUG
	cout << "Header Length is " << headerLength << endl;
#endif
	output->write((char*) &headerLength,sizeof(int));
#ifdef DEBUG
	cout << "Writing field stuff." << endl;
#endif
	*output << *fldList;
#ifdef DEBUG
	cout << "Writing index stuff." << endl;
#endif
	*output << *idxList;

	// Close output here so we don't get access problems in recServer
	delete output;

	// strip the extension from the filename.

	char *idxFName = new char[strlen(theName) + 10];
	strcpy(idxFName,theName);
	char* ext;
	ext=strrchr(idxFName,'.');

	if (!ext) // If there is no extension
		ext = idxFName + strlen(idxFName); // point after the filename

#ifdef FILENAMELEN
	// We have a filename length problem (eg Minix file systems)
	if (ext - idxFName > FILENAMELEN - 4) // do we have enough space for ext
		ext = idxFName + FILENAMELEN - 4; // chop the filename off
#endif

	*ext = '\0'; // Chop off the extension

	// Ok. Now put the stuff that daz wants into the .db file.

	dbError err;
	err = theRecServer.createDb(theName, RECORD_LENGTH, headerLength);

/*	if (err != recServer::db_ok)
	{
		cerr << "Recserver failed to work." << endl;
		//exit(1);
	}
*/

	// Now let's sort out the memServer
	if (stringCount)
	{
		char* stringName = new char[strlen(idxFName) + 5];
		strcpy(stringName, idxFName);
		strcat(stringName, ".str");
		memServer m;
		m.createMem(stringName);
	}

	// and then create an index file for each index.

	indexInfo* current = idxList->indicies;

	int indNum = 0;
	while(current)
	{
		int keyLength=0;
		for(int i=0;i<MAX_FIELDS_IN_INDEX;i++)
		{
			fieldInfo* curFld=fldList->fields;
			if (current->idxFields[i] != -1)
			{
				for(int j=0;j<current->idxFields[i];j++)
					curFld=curFld->nextFld;
				keyLength+=curFld->fldSize;
			}
		}

#if defined(FIXEDEXTENSION) || defined(FILENAMELEN)
		// We don't need to always do this for FILENAMELEN but we are
		// guaranteed that there is space for these 4 characters no matter
		// what so we might as well be safe. Such arbitrary filename lengths
		// are ugly anyway. No worries making them uglier!

		sprintf(ext, ".i%s%d",(i<10)?"d":"",indNum);
#else
		sprintf(ext, ".id%d",indNum);
#endif

#ifdef DEBUG
	cout << "Filename is " << idxFName << "  Extension is " << ext << endl;
#endif
		bTree ind(idxFName,BUCKET_SIZE, keyLength, indNum++);

		// The next line is just to convince the compiler that ind is
		// really used. The reason we never access ind is all we want to do is
		// create the bTree, no more.
		ind.isOpen = true;
		current= current->nextIdx;
	}
	delete idxFName;
}

// Convert the index field names to internal structures

void installIndex(char* s1, char* s2, int idxType, char indexNames[MAX_FIELDS_IN_INDEX][MAX_NAME_LENGTH], indexList* & list, int& size)
{
	static indexInfo*	currentIdx;

#ifdef DEBUG
cout << "Installing index on " << indexNames[0] << endl;
#endif
//	int fldNum1=-1, fldNum2=-1;

	TIndicies indicies;

	for(int z=0;z<MAX_FIELDS_IN_INDEX;z++)
		indicies[z] = -1;

	// First figure out the field numbers for the required
	// index fields.

	for(int fNum=0; fNum<fieldNum; fNum++)
	{
		fieldInfo* currentField = fldList->fields;
		int i=0;

#ifdef DEBUG
		cout << "Locating " << indexNames[fNum] << endl;
#endif
		while (currentField)
		{
			if (strcmp(currentField->fldName,indexNames[fNum]))
			{
				i++;
				currentField=currentField->nextFld;
			}
			else
			{
				if (s1 && currentField->fldType == D_STRING || currentField->fldType == D_DATA)
				{
					cerr << "Variable length " << s1 << "s not supported. Field "
						 << currentField->fldName
					     << " can not be a " << s1 << " field." << endl;
					exit(1);
				}
				indicies[fNum] = i;
				break;
			}
		}
		if (indicies[fNum] == -1)
		{
			cerr << "Error parsing " << s2 << " entry : field not found in relation"
				<<  " description." << endl;
			exit(1);
		}
	}

	// Create and install the new index.

	indexInfo* temp = new indexInfo(idxType,indicies);

	if (list->indicies==0)
		list->indicies= temp;
	else
		currentIdx->nextIdx= temp;
	currentIdx = temp;
	// And increase the size of the index info.
	size+=sizeof(int)
		    +sizeof(int)*MAX_FIELDS_IN_INDEX
		    +sizeof(indexInfo*);
	list->numIndicies++;
}

void installField()
{
	static fieldInfo* currentFld;

	fieldInfo* temp = new fieldInfo(fldName, fldType, fldSize);

	if (fldList->fields == 0)
		fldList->fields = temp;
	else
		currentFld->nextFld = temp;
	currentFld = temp;
	// Increase the size of the field info section.
	fldInfoSize+= (4*sizeof(int)) + strlen(fldName) +1;

	fldList->numFields++;

	if (defaults)
		defaults = (char**)realloc(defaults, sizeof(char*) * fldList->numFields);
	else
		defaults = new char*;
	if (fldDef.len())
	{
		defaults[fldList->numFields-1] = new char[fldDef.len()+1];
		strcpy(defaults[fldList->numFields-1], fldDef);
	}
	else
		defaults[fldList->numFields-1] = 0;
}

void outputFieldList(ostream& of, bool strings)
{
	for (int ch=0; ch<=1; ch++)
	{
		fieldInfo* f = fldList->fields;

		while (f)
		{
			if (ch ^ ((f->fldType != D_CHAR && f->fldType != D_ICHAR)?1:0))
			{
				if (!strings || f->fldType == D_STRING || f->fldType == D_DATA)
				{
					of << "\t";
					if (!strings)
						of << typeName(f->fldType);
					else
						if (f->fldType == D_STRING)
							of << "dbString";
						else if (f->fldType == D_DATA)
							of << "dbData";
					of << "\t";
					if ((f->fldType == D_DATA || f->fldType == D_STRING) && !strings)
						of << "_off_";

					of << f->fldName;

					long l = f->fldSize / mySizeOf(f->fldType);
					if (l > 1)
						of << "[" << l << "]";
					of << ";" << endl;
				}
			}
			f = f->nextFld;
		}
	}
}

long indNameToNumber(char* name)
{
	for (int i=0; i<indexNum; i++)
		if (!strcmp(name, indexName[i]))
			return i;

	return -1;
}

%}

%token ASSIGN
%token NEWLINE
%token ON
%token WITH
%token WITHOUT
%token DUP
%token TCOMMENT
%token COMMA
%token INDEX
%token BOTH
%token NUM
%token SEMI
%token DIGIT
%token ALPHA
%token ALNUM
%token IDENT
%token SHORT
%token USHORT
%token LONG
%token ULONG
%token DOUBLE
%token FLOAT
%token MONEY
%token DATE
%token CHAR
%token ICHAR
%token STRING
%token DATA
%token LCBRACE
%token RCBRACE
%token LSBRACE
%token RSBRACE
%token RELATION
%token TYPE
%token FIELD
%token TYPEDEF
%token UNIQUE
%token AND
%token IS
%token CALLED
%token NUTHIN
%token ORDER
%token DOT
%token TO
%token LINK
%token USING
%token CONSTRUCT

%%

relationList: relationList element
			|
			;

element :	relation
		|	link
		;

link	:	linkT identT
			{
				// Ok. This is the start of the link and we have the name.
			}
			identT
			{
				// This is the name of the first linked relation.
			}
			dotT identT
			{
				// The name of the field to be linked in the first relation.
			}
			linkNodeName
			{
				// The name given to the node in the link structure.
			}
			toT identT
			{
				// The name of the second linked relation
			}
			dotT identT
			{
				// The name of the field to be linked in the second relation
			}
			linkNodeName
			{
				// The name given to the second link node
			}
			linkOrder
		;


linkNodeName	: is called identT
				  {
					// This is the name we want to give to the node.
				  }
				| {
					// If no name is specified, use the field name
					// prefixed by two underlines.
				  }
				;

linkOrder	: is ORDER
			  {
				// If they specify that the link is to be ordered,
				// we need to generate an extra "order" field.
			  }
			|
			;

relation	: relationT identT
			  {
				// Open the relation description.

				idxStr = new strstream;
				*idxStr << "dbIdx_seq=-1";
				strcpy(fileName,yytext);

				strcpy(relName,yytext);
				strcpy(className, yytext);

				// Ok. now create new list structures.

				idxList = new indexList;
				fldList = new fieldList;
				consList = new indexList;
				unique = 0;
				stringCount = 0;
				indexName = 0;
			  }
			  structName
			  {
				// If the relation name and class name are the same then
				// add "Str" onto the end of the name of the struct.
				if (!strcmp(relName, className))
					strcat(relName, "Str");
				*relDesc << "class "<<relName<<endl // ": public diaRel"<<endl
					 << "{" << endl
						 << "public:" << endl
					 << "//------" << endl;
				RECORD_LENGTH = 0;
			  }
	      fieldList
	      {
				*idxStr << ends;
				*relDesc << "\tenum {"
					 << idxStr->str()
					 << "};" << endl;
				*relDesc << "\t" << relName << "(){};" << endl;
				*relDesc << "};" << endl;

				if (stringCount)
				{
					*relDesc << endl << "struct _Strings_" << relName << endl
							 << "{" << endl;
					outputFieldList(*relDesc, true);
					*relDesc << "\tvirtual ~_Strings_" << relName << "() {};" << endl;
					*relDesc << "};" << endl;
				}

				relCount++;
#ifdef DEBUG
				cout << *fldList;
				cout << *idxList;
#endif

				strcat(fileName,".db");
				if (makeDatabases)
					createEmptyRelation(fileName,1);

				*relDesc << endl
					<< "//---------------------------------------------"
					<< endl << endl;

				if (makeSource)
				{
					if (!generateClass(ofHead, ofCode, RECORD_LENGTH, idxList, fldList, relName, className, stringCount, consList, defaults))
					{
						cerr << "Unable to generate class " << className << endl;
						exit(1);
					}
				}
				if (base.len() != 0)
				{
					// Make a derived class?
					if (base == className) {
						if (deriveDone) {
							cerr << "Already generated one set of derived classes." << endl;
							exit (1);
						}
						if (generateDerived(base, derive, deriveName, fldList, consList))
						{
							deriveDone = true;
						}
						else
						{
							cerr << "Unable to derive class " << derive << endl;
							exit(1);
						}
					}
				}
				delete fldList; //
				delete idxList; // Close the list structures.
				delete idxStr;
				delete consList;
				delete[] indexName;
			  }
			;
structName  : is called identT
	      {
				// An explicit name for the relation
		strcpy(relName,yytext);
	      }
	    |
	    ;

is			: IS
			|
			;
called		: CALLED
			;

fieldList	: LCBRACE fieldList1
			  {
				outputFieldList(*relDesc, false);
				indexNum = 0;
			  }
			  indexList
			  constructList
			  RCBRACE
			;

fieldList1	: fieldList1 fieldLine
			|
	    ;

fieldLine   : fieldDef semi
			;

semi		: SEMI
			| error
			  {
				yyerror(" Syntax Error. Missing semicolon.\n");
			  }
			;

fieldDef	: type identT
			  {
				// *relDesc << yytext;
				strcpy(fldName,yytext);
			  }
			  size
		      {
				// *relDesc << ";" << endl;
				RECORD_LENGTH+=fldSize;
				fldDef.clr();
			  }
			  defValue
			  {
				installField();
			  }
			;


defValue	: ASSIGN
			  {
				yyGetToSemi();
				fldDef = yytext;
			  }
			|
			;

type		: SHORT
			  {
				// All the different types are handled here

				// *relDesc << "\tshort\t";
				fldSize = sizeof(short);
				fldType = D_SHORT;
			  }
			| USHORT
			  {
				// *relDesc << "\tunsigned short\t";
				fldSize = sizeof(unsigned short);
				fldType = D_USHORT;
			  }
			| LONG
			  {
				// *relDesc << "\tlong\t";
				fldSize = sizeof(long);
				fldType = D_LONG;
			  }
			| ULONG
			  {
				// *relDesc << "\tunsigned long\t";
				fldSize = sizeof(unsigned long);
				fldType = D_ULONG;
			  }
			| DOUBLE
			  {
				// *relDesc << "\tdouble\t";
				fldSize = sizeof(double);
				fldType = D_DOUBLE;
			  }
			| FLOAT
			  {
				// *relDesc << "\tfloat\t";
				fldSize = sizeof(float);
				fldType = D_FLOAT;
			  }
			| MONEY
			  {
				// *relDesc << "\tmoneyType\t";
				fldSize = sizeof(moneyType);
				fldType = D_MONEY;
			  }
			| DATE
			  {
				// *relDesc << "\tdateType\t";
				fldSize = sizeof(dateType);
				fldType = D_DATE;
			  }
			| ICHAR
			  {
				// *relDesc << "\tchar\t";
				fldSize = sizeof(char);
				fldType = D_ICHAR;
			  }
			| CHAR
			  {
				// *relDesc << "\tchar\t";
				fldSize = sizeof(char);
				fldType = D_CHAR;
			  }
			| STRING
			  {
				fldSize = sizeof(long);
				fldType = D_STRING;
				stringCount++;
			  }
			| DATA
			  {
				fldSize = sizeof(long);
				fldType = D_DATA;
				stringCount++;
			  }
			| UNIQUE
			  {
				if (unique)
				{
					cerr << "Only one unique field allowed per relation" << endl;
					exit (1);
				}
				// *relDesc << "\tuniqueType\t";
				fldSize = sizeof(uniqueType);
				fldType = D_UNIQUE;
				unique = 1;
			  }
			| IDENT
			  {
				yyerror(" Syntax Error. Expecting a type.\n");
			  }

			;

size		: LSBRACE
				{
					if (fldType == D_STRING || fldType == D_DATA)
					{
						cerr << "We're not doing arrays of dbString or dbData yet.  Please come back later" << endl;
						exit(1);
					}
					// Handle arrays
					// *relDesc << "[";
				}
			  numT
			  {
				// *relDesc << yytext ;
				int size;
				sscanf(yytext,"%d",&size);
				fldSize*=size;
			  }
			  RSBRACE
				{
					// *relDesc << "]";
				}
			|
			;

constructList	: constructList constructSpec
			  {
				installIndex(0, "paramater", idxType, idxNames, consList, consListSize);
			  }
			|
			;

constructSpec	:	CONSTRUCT using
					{
						fieldNum=0;
						idxType = -1;
					}
					consFieldList
					consIndex
					semi
			;

consIndex	:	INDEX
				identT
				{
					int i = indNameToNumber(yytext);
					if (i == -1) {
						cerr << "Index " << yytext << " was not defined." << endl;
						exit(1);
					}
				idxType = i;
				}
			|
			;

using	: USING
		|
		;

consFieldList	:	consFieldList1
				|
				;

consFieldList1	:	consFieldList1 comma consField
				|	consField

consField		:	identT
					{
						strcpy(idxNames[fieldNum++], yytext);
					}
				;

indexList	: indexList indexSpec semi
			  {
				installIndex("key", "index", idxType, idxNames, idxList, idxInfoSize);
			  }
			|
			;


indexSpec	: INDEX identT
	      {
		fieldNum=0;	// reset the number of fields count
		*idxStr << ", dbIdx_" << yytext;
				if (indexName)
					indexName = (char**)realloc(indexName, (indexNum+1)*sizeof(char*));
				else
					indexName = new char*;
				indexName[indexNum]=new char[strlen(yytext)+2];
				strcpy(indexName[indexNum++], yytext);
	      }
	      on identT
	      {
		strcpy(idxNames[0],yytext);
		fieldNum++;
		idxType = IDX_SINGLE;
	      }
	      indexSpec2
	    ;


indexSpec2	: indexSpec2 comma identT
			  {
				strcpy(idxNames[fieldNum],yytext);
				fieldNum++;
				idxType = IDX_MULTIPLE;
			  }
			|
			;

comma		: COMMA
			;

on			: ON
			| IDENT {yyerror(" Syntax Error. Expecting \"on\".\n");}
			;

/* Removed the error below so that links can be identified */

relationT	: RELATION
			;
identT		: IDENT
			| error {yyerror(" Syntax Error. Expecting identifier.\n");}
			;
numT		: NUM
			| IDENT {yyerror(" Syntax Error. Expecting integer constant.\n");}
			;
toT			: TO
			|
			;
linkT		: LINK
			|
			;
dotT		: DOT
			| IDENT {yyerror(" Syntax Error. Expecting \".\"\n");}
			;
%%

#include "lex.yy.c"

void yyerror(char* str)
{
	cout << "Line " << linenum << ":" ;
	cout << str << endl;
	exit(1);

}

int yyparse();

void usage(void)
{
	cerr << "Usage : dsc [-C] [-D] [-I <path>] [-O <path>] [-S <path>] [-G base derived filebase] <schema name[.ds]> " << endl;
	cerr << endl;
	cerr << "\t -D    : generate the database files." << endl;
	cerr << "\t -C    : generate the source code." << endl;
	cerr << "\t -C -D : generate both." << endl;
	cerr << "\t <none>: just parse." << endl;
	cerr << endl;
	cerr << "\t -G    : generate a derived class from base in files filebase.{h,cc}" << endl;
	cerr << endl;
	cerr << "\t -I : specify the path to search for schema files." << endl;
	cerr << "\t -O : specify the path to place database files in." << endl;
	cerr << "\t -S : specify the path to place source code in." << endl;
	exit(1);
}

int getName(char* name)
{
	FILE* inFile;
	char fname[200];
#ifdef DEBUG
	cout << "Looking for " << name << endl;
#endif
	if ((inFile = fopen(name,"r")) != 0)
	{
		fclose(inFile);
		return 1;
	}
	if (inputPath[0] == 0)
		return 0;
	strcpy(fname,inputPath);
	strcat(fname,name);
#ifdef DEBUG
	cout << "Looking for " << fname << endl;
#endif
	if((inFile = fopen(fname,"r")) == 0)
		return 0;
	fclose(inFile);
	strcpy(name,fname);
	return 1;
}

main(int argc, char* argv[])
{
	char fname[100][200];
	int	 files=0;
	char bname[200];

	outputPath[0]=0;
	inputPath[0]=0;
	sourcePath[0]=0;

	if (argc == 1)
	{
		usage();
	}

	// Parse the command line args
	for(int i=1;i<argc;i++)
	{
		if (!strcasecmp(argv[i],"-G")) {
			if (argc-i < 3)
			{
				cerr << "Wrong number of arguments to -G." << endl;
				usage();
			}
			if (base.len())
			{
				cerr << "Only one -G allowed per run." << endl;
				usage();
			}
			// Usage is -d base derived file
			base = argv[i+1];
			derive = argv[i+2];
			deriveName = argv[i+3];
			deriveDone = false;
			i+=3;
		}
		else
		if (!strcasecmp(argv[i],"-C"))
		{
			makeSource=1;
			//headersOnly=1;
		}
		else
		if (!strcasecmp(argv[i],"-D"))
		{
			makeDatabases=1;
		}
		else
		if (!strcasecmp(argv[i],"-O"))
		{
			if (outputPath[0] != 0)
			{
				cerr << "Output path already specified." << endl;
				usage();
			}
			if ((argc < i+1) || (argv[i+1] == 0))
				usage();
			strcpy(outputPath,argv[i+1]);
			if(outputPath[strlen(outputPath)-1] != '/')
				strcat(outputPath,"/");
			i++;

		}
		else
		if (!strcasecmp(argv[i],"-S"))
		{
			if (sourcePath[0] != 0)
			{
				cerr << "Source path already specified." << endl;
				usage();
			}
			if ((argc < i+1) || (argv[i+1] == 0))
				usage();
			strcpy(sourcePath,argv[i+1]);
			if(outputPath[strlen(sourcePath)-1] != '/')
				strcat(sourcePath,"/");
			i++;

		}
		else
		if (!strcasecmp(argv[i],"-I"))
		{
			if (inputPath[0] != 0)
			{
				cerr << "Input path already specified." << endl;
				usage();
			}
			if ((argc < i+1) || (argv[i+1] == 0))
				usage();
			strcpy(inputPath,argv[i+1]);
			if(inputPath[strlen(inputPath)-1] != '/')
				strcat(inputPath,"/");
			i++;
		}
		else
		{
			if (argv[i][0] == '-')
			{
				cerr << "Invalid option." << endl;
				usage();
			}
			if (files !=0)
			{
				cerr << "Multiple file compilation not supported." << endl;
				usage();
			}
//			fname[files] = new char[strlen(argv[i])+1];
			strcpy(fname[files++],argv[i]);
		}
	}

#ifdef DEBUG
	if((!makeSource)&&(!makeDatabases))
		cout << "Parsing schema's only." << endl;
	if (makeSource)
		cout << "Outputting source code." << endl;
	if (makeDatabases)
		cout << "Outputing databases." << endl;
	if (outputPath[0] != 0)
		cout << "Dumping output to " << outputPath << endl;
	if (sourcePath[0] != 0)
		cout << "Creating source in " << sourcePath << endl;
	cout <<"Creating databases for :" << endl;
	for(i=0;i<files;i++)
		cout << fname[i] << endl;
#endif


	cout << "Diamond Base Schema Compiler" << endl;

	for(i=0;i<files;i++)
	{
		char* p;
		char* lastslash;
		char* lastdot;
		char* lastbslash;

		// Let's analyse this path
		lastslash = strrchr(fname[i], '/');
		lastbslash = strrchr(fname[i], '\\');
		lastdot = strrchr(fname[i], '.');

		// Find the last directory delimeter
		if (lastbslash > lastslash)
			lastslash = lastbslash;

		// Does the filename component (after the last delimeter) have a dot
		// If not add a suffix
		if (!lastdot || lastslash > lastdot)
			strcat(fname[i],".ds");

		// Find the base name, without extension
		strcpy(bname,fname[i]);
		p=strrchr(bname, '.');
		*p = 0;

		cout << "Building relations from schema file " << fname[i] << endl;


		if (!getName(fname[i]))
		{
			cerr << "Schema Description file \"" << fname[i] << "\" not found." << endl;
			exit(1);
		}

		yyin = fopen(fname[i],"r");
		if (yyin == 0)
		{
			cout << "Schema description file \""<<fname[i]<<"\" not found." << endl;
			cout << "Aborting." << endl;
			exit(1);
		}

	// Open the relation description.

		char *str = new char[strlen(sourcePath) + strlen(bname) + 10];

		if(makeSource)
		{
			if (sourcePath[0] != 0)
				sprintf(str,"%s%s",sourcePath,bname);
			else
				sprintf(str,"%s",bname);
			if (!generateStart(ofHead, ofCode, str))
			{
				cerr << "Unable to create source code files for class" << endl;
				exit(1);
			}

#ifdef FILENAMELEN
			// We have a filename length problem (eg Minix file systems)
			long extra = strlen(bname) - FILENAMELEN + 4;
			if (extra > 0) // do we have enough space
				*(str + strlen(str) - extra)=0; // chop the filename off
#endif

			strcat(str, "_s.h");
			relDesc = new ofstream(str);

			if (relDesc == 0)
			{
				cerr<<"Error opening relation description file "<<str<<endl;
				cerr<<"Aborting." << endl;
			}
		}
		else
		{
			relDesc = new ofstream("/dev/null");
		}
		descHeader(relDesc,fname[i]);
		yyparse();
		cout << relCount << " relations successfully compiled." << endl;
		relCount = 0;
		fclose(yyin);
		if(makeSource)
			generateEnd(ofHead, ofCode);
		delete relDesc;
		delete str;
		if (base.len() && !deriveDone)
			cerr << "WARNING: Derived class was not generated, " << base << "not found." << endl;
	}

	return 0;
}
