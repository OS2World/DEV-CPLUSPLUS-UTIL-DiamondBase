/**************************************************************************
 * Source Id :
 *
 * $Id: generate.h,v 1.9 1993/10/25 08:37:21 kevinl Exp $
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
 *  Header file for code generator
 *
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: generate.h,v $
 * Revision 1.9  1993/10/25  08:37:21  kevinl
 * Added realData to typeName
 *
 * Revision 1.8  1993/10/24  15:35:45  kevinl
 * Added derived class generation
 *
 * Revision 1.7  1993/10/19  11:43:02  kevinl
 * Default values
 *
 * Revision 1.6  1993/10/18  08:01:41  kevinl
 * Added constructors and fixed some probs
 *
 * Revision 1.5  1993/07/11  09:42:05  kevinl
 * Changed String to dbString
 *
 * Revision 1.4  1993/06/23  05:21:22  kevinl
 * Mallocs are now in angular brackets
 *
 * Revision 1.3  1993/06/20  13:39:04  kevinl
 * New members for String support
 *
 * Revision 1.2  1993/05/07  13:54:35  kevinl
 * Added outputFieldList so fields were in correct order for alignment
 *
 * Revision 1.1  1993/03/21  23:38:35  kevinl
 * Initial revision
 *
 **************************************************************************/

#include <defs.h>

bool generateStart(ofstream& hf, ofstream& cf, char* filename);
bool generateClass(ofstream& hf, ofstream& cf, long recLen, indexList* idxList, fieldList* fldList, char* structName, char* className, int stringCount, indexList* consList, char** defaults);
bool generateEnd(ofstream& hf, ofstream& cf);
bool generateDerived(dbString base, dbString derive, dbString deriveName, fieldList* fldList, indexList* consList);
long mySizeOf(int tp);
char* typeName(long tp, bool realData=false);
