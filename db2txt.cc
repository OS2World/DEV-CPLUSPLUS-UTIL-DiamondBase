/**************************************************************************
 * Source Id :
 *
 * $Id: db2txt.cc,v 1.3 1993/10/28 07:49:01 kevinl Exp $
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
 *	Convert a dbObject to text format, outputing in index 0 order.
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: db2txt.cc,v $
// Revision 1.3  1993/10/28  07:49:01  kevinl
// Added diaGRel dbString/dbData support
//
// Revision 1.2  1993/10/18  08:03:10  kevinl
// Spelling mistook
//
// Revision 1.1  1993/10/10  23:25:28  kevinl
// Initial revision
//
 **************************************************************************/

#include <iostream.h>
#include <stdlib.h>
#include <diagrel.h>
#include <generate.h>

diamondBase theDiamondBase("");

#define OUT(data,off,typ) *((typ*)(((char*)data)+off))

void
outputField(ostream& o, memServer* ms, void* data, long off, char type, long count)
{
	if (count != 1)
		o << "[" << count << "]";
	o << " ";
	if (type == D_CHAR)
		o << ((char*)data)+off;
	else
		for (int i=0; i<count; i++) {
			switch (type) {
				case D_SHORT:
					o << OUT(data,off,short);
					break;
				case D_USHORT:
					o << OUT(data, off, unsigned short);
					break;
				case D_DATA:
				{
					dbData s;
					ms->getString(s, OUT(data, off, long));
					o << s << " @ " << OUT(data, off, long);
					break;
				}
				case D_STRING:
				{
					dbString s;
					ms->getString(s, OUT(data, off, long));
					o << s << " @ " << OUT(data, off, long);
					break;
				}
				case D_LONG:
					o << OUT(data, off, long);
					break;
				case D_ULONG:
					o << OUT(data, off, unsigned long);
					break;
				case D_DOUBLE:
					o << OUT(data, off, double);
					break;
				case D_FLOAT:
					o << OUT(data, off, float);
					break;
				case D_MONEY:
					o << OUT(data, off, moneyType);
					break;
				case D_DATE:
					o << OUT(data, off, dateType);
					break;
				case D_UNIQUE:
					o << OUT(data, off, uniqueType);
					break;
				default:
					dbErr("Unknown type");
			}
			off++;
			o << " ";
		}
}

int main(int argc, char** argv)
{
	if (argc != 2) {	
		cerr << "Syntax: " << argv[0] << " relation" << endl;
		exit (1);
	}

	diaGRel rel(argv[1]);
	if (!rel.ok()) {
		rel.perror("Can't open relation");
		exit (2);
	}

	memServer* ms = 0;
	if (rel.numStrings())
	{
		dbString strName = argv[1];
		strName += ".str";
		ms = new memServer(strName);
	}

	cout << "REL " << argv[1] << endl;
	rel.begin();
	while (rel.next() != db_eof) {
		cout << "REC " << rel.getRecNum() << endl;
		dbObjData* d = rel.getObjData();
		for (int i = 0; i < d->numFields; i++) {
			cout << "FLD " << d->fieldName[i] << " " << typeName(d->fieldType[i],true);
			outputField(cout, ms, rel.theData, d->fieldOffset[i], d->fieldType[i], d->fieldLength[i] / mySizeOf(d->fieldType[i]));
			cout << endl;
		}
	}
	rel.end();
}
