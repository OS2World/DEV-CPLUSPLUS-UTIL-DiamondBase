/**************************************************************************
 * Source Id :
 *
 * $Id: mystring.h,v 1.23 1993/11/05 12:45:05 kevinl Exp $
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
 *  mystring.h:
 *  A the variable length string class for variable length records
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: mystring.h,v $
 * Revision 1.23  1993/11/05  12:45:05	kevinl
 * Fixed problems that CC picked up (and some multi bugs)
 *
 * Revision 1.22  1993/10/28  13:31:20	davison
 * Changed clear() method to clr().
 *
 * Revision 1.21  1993/10/24  15:36:12	kevinl
 * A spelling error and a dbString copy constructor
 *
 * Revision 1.20  1993/10/24  11:43:46	davison
 * Fixed up operator +=(char) and =operator =(char).
 * They now ensure that dummyc[1] = 0.
 *
 * Revision 1.19  1993/10/24  10:09:38	kevinl
 * Removed a const
 *
 * Revision 1.18  1993/10/24  09:14:48	kevinl
 * dummyc is now a char[2] so we can remove the aggregate initialiser
 * from the char operators
 *
 * Revision 1.17  1993/10/19  11:59:05	kevinl
 * copying (char*)0 or a totally empty dbData/dbString now truncates
 *
 * Revision 1.16  1993/10/18  09:07:23	davison
 * Killed unsigned int manipulations.
 * Added addSize & subSize.
 * Added char concatination method.
 *
 * Revision 1.15  1993/10/18  05:55:51	davison
 * Added a string comparison operator to dbString. Compiled ok. Hope it
 * works :-)
 *
 * Revision 1.14  1993/10/05  07:34:31	kevinl
 * Now handles dbData AND dbString properly
 *
 * Revision 1.13  1993/09/26  08:34:32	kevinl
 * Fixed ambiguous constructors
 *
 * Revision 1.12  1993/08/29  12:56:27	kevinl
 * Fixed problems of assigning relations to each other.
 *
 * Revision 1.11  1993/07/20  12:59:30	kevinl
 * Some unisgned int vs long problems
 *
 * Revision 1.10  1993/07/19  11:57:58	kevinl
 * Added some dummy cahrs to remove Borland warnings re temporaries
 * Added some {} to remove Borland's confusion about multiple if/elses
 *
 * Revision 1.9  1993/07/13  00:18:08  kevinl
 * We weren't setting size in cat/cpy johnb@kea.grace.cri.nz (John Burnell)
 *
 * Revision 1.8  1993/07/11  10:49:21  kevinl
 * dbString version
 *
 * Revision 1.7  1993/07/11  09:42:05  kevinl
 * Changed String to dbString
 *
 * Revision 1.6  1993/07/11  08:20:17  kevinl
 * Fixed [] operator
 * Added CHUNKSIZE
 *
 * Revision 1.5  1993/07/09  06:26:52  kevinl
 * Made some operators that take String
 *
 * Revision 1.4  1993/07/09  03:57:55  kevinl
 * Removed realloc of null pointers.
 *
 * Revision 1.3  1993/06/20  23:29:05  kevinl
 * Added -=
 *
 * Revision 1.2  1993/06/20  13:38:27  kevinl
 * Fixed multiple mallocs
 * longs now unsigned int, cat/cpy return the string
 * added fill, =,+= unsigned int and moved += operator in
 *
 * Revision 1.1  1993/06/18  12:31:14  kevinl
 * Initial revision
 *
 **************************************************************************/

#ifndef __MYSTRING_H
#define __MYSTRING_H

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif
#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <string.h>
#include <defs.h>

const CHUNKSIZE = 256;

class dbData
{
	char dummyc[2];
	unsigned int isstr; // Extra space needed on end of data
	unsigned int size;  // How much memory do we need for the char*;
public:
	char *data;  // The data being played with

	char* verStr(void) { return "$Id"; }
	dbData(unsigned int e = 0) : isstr(e) { data=0; size=0; }
	dbData(dbData& D) : isstr(D.isstr) { createDbData(D); }
	dbData(dbData& D, unsigned int e) : isstr(e) { createDbData(D); }
	void createDbData(dbData& D)
	{
		dummyc[1] = 0;
		if (D.data) {
			size=D.len()+isstr;
			data = new char[size];
			bcopy(D.data, data, size);
		} else {
			size=0;
			data=0;
		}
	}
	dbData(char* s, unsigned int e = 0) : isstr(e)
	{
		dummyc[1] = 0;
		if (s) {
			data = strdup(s);
			size = strlen(s)+1;
		} else {
			size=0;
			data=0;
		}
	}
	dbData(void* s, unsigned int len, unsigned int e = 0) : isstr(e)
	{
		dummyc[1] = 0;
		if (s) {
			data = new char[len];
			bcopy((char*)s, data, len);
			size = len;
		} else {
			size=0;
			data=0;
		}
	}
	~dbData(void) { if (data) free(data); data=0; size=0; }
	unsigned int len(void) { return (isstr)?(data?strlen(data):0):size; }
	unsigned int setSize(unsigned int newLen)
	{
		if (isstr && newLen < size) {
			// Nothing
		} else {
			if (data) {
				data = (char*)realloc(data, newLen);
			} else {
				data = new char[newLen];
				if (isstr) *data = 0;
			}
			size = newLen;
		}
		return size;
	}
	unsigned int getSize(void)
	{
		return size;
	}
	char* cat(char* s)
	{
		if (s) {
			unsigned int slen = strlen(s);
			unsigned int newsize = len()+slen+1; // always 1 'cos adding a string
			unsigned int oldsize = len();
			setSize(newsize);
			bcopy(s, data+oldsize, slen+1);
		}
		return data;
	}
	char* cat(dbData& d)
	{
		if (d.data) {
			unsigned int newsize = len()+d.len()+isstr;
			unsigned int oldsize = len();
			setSize(newsize);
			bcopy(d.data, data+oldsize, d.len()+isstr);
		}
		return data;
	}
	char* cpy(char* s)
	{
		if (s) {
			unsigned int slen = strlen(s);
			setSize(slen+1);
			bcopy(s, data, slen+1);
		} else {
			setSize(isstr);
			if (isstr)
				*data = 0;
		}
		return data;
	}
	char* cpy(dbData& d)
	{
		if (d.data) {
			setSize(d.len()+isstr);
			bcopy(d.data, data, d.len()+isstr);
		} else {
			setSize(isstr);
			if (isstr)
				*data = 0;
		}
		return data;
	}
	void fill(char c, unsigned int count = 0);
	void clr(void) {
		if (isstr) {
			if (data) {
				data[0] = 0;
			}
		} else
			dispose();
	}
	void dispose(void) { if (data) free(data); data=0; size=0; }
	operator char*() { return data; }
	operator void*() { return (void*)data; }
	bool empty(void) { return (bool)(data != 0); }
	friend dbData operator + (dbData& S, char* s);
	friend dbData operator + (dbData& S, dbData& s);
	friend ostream& operator << (ostream& o, dbData& s);
	friend istream& operator >> (istream& i, dbData& s);
	char& operator [] (unsigned int i) { return (data && i < size)?data[i]:dummyc[0]; }
	dbData& operator = (char* s)
	{
		cpy(s);
		return *this;
	}
	dbData& operator = (dbData& d)
	{
		cpy(d);
		return *this;
	}
	dbData& operator = (char c)
	{
		dummyc[0] = c;
		dummyc[1] = 0;
		cpy(dummyc);
		return *this;
	}
	dbData& operator += (char* s)
	{
		cat(s);
		return *this;
	}
	dbData& operator += (dbData& d)
	{
		cat(d);
		return *this;
	}
	dbData& operator += (char c)
	{
		dummyc[0] = c;
		dummyc[1] = 0;
		cat(dummyc);
		return *this;
	}
#if 0
	unsigned int operator = (unsigned int s)
	{
		unsigned int sz = setSize(s);
		if (isstr && s)
			data[s-1] = 0;
		return sz;
	}
#endif
//	unsigned int operator += (unsigned int s)
	unsigned int addSize(unsigned int s)
	{
		return setSize(size+s);
	}
//	unsigned int operator -= (unsigned int s)
	unsigned int subSize(unsigned int s)
	{
		if (s > size)
			s = size;
		s = size - s;
		unsigned int sz = setSize(s);
		if (isstr && s)
			data[s-1] = 0;
		return sz;
	}
};

class dbString : public dbData
{
public:
	dbString(void) : dbData(1) { }
	dbString(dbData& D) : dbData((char*)D, 1) { }
	dbString(dbString& D) : dbData(D, 1) { }
	dbString(char* s) : dbData(s, 1) { }
	dbString& operator = (char* s)
	{
		cpy(s);
		return *this;
	}
	dbString& operator = (dbData& d)
	{
		cpy(d);
		return *this;
	}
	int operator == (char* str)
	{
		return !strcmp(data, str);
	}
	int operator == (dbString& d)
	{
		return !strcmp(data, d.data);
	}
};
#endif
