/**************************************************************************
 * Source Id :
 *
 * $Id: mystring.cc,v 1.8 1993/10/28 13:31:20 davison Exp $
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
 *  mystring.cc:
 *  Operators for the variable length string class.
 *
 *  Original Author : Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: mystring.cc,v $
// Revision 1.8  1993/10/28  13:31:20  davison
// Changed clear() method to clr().
//
// Revision 1.7  1993/10/05  07:34:31  kevinl
// Now handles dbData AND dbString properly
//
// Revision 1.6  1993/08/29  12:56:27  kevinl
// Fixed problems of assigning relations to each other.
//
// Revision 1.5  1993/07/11  09:42:05  kevinl
// Changed String to dbString
//
// Revision 1.4  1993/07/11  08:20:42  kevinl
// Fixed >> operator. Now gets enough memory for itself.
//
// Revision 1.3  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.2  1993/06/20  13:40:01  kevinl
// Fixed multiple mallocs
// += removed and put in header
//
// Revision 1.1  1993/06/18  12:31:14  kevinl
// Initial revision
//
 **************************************************************************/

#include <ctype.h>
#include <mystring.h>

void
dbData::fill(char c, unsigned int count)
{
	if (count == 0) {
		if (size == 0)
			return;
		else
			count = size-isstr;
	} else
		setSize(count+isstr);
	memset(data, c, count);
	if (isstr)
		data[count] = 0;
}

dbData operator + (dbData& S, char* s) 
{
	dbData res(S);
	res.cat(s);
	return res;
}
dbData operator + (dbData& S, dbData& s) 
{
	dbData res(S);
	res.cat(s);
	return res;
}
ostream& operator << (ostream& o, dbData& s)
{
	if (s.isstr)
		o << (char*)s;
	else
		o.write((char*)s, s.getSize());
	return o;
}
istream& operator >> (istream& i, dbData& s)
{
	s.clr();
	s.setSize(CHUNKSIZE);
	long len = 0; // Speeds things up
	s[len] = 0;
	char c;
	if (s.isstr) {
		do {
			c = i.get();
			if (c == EOF || !i.good())
				return i;
		} while (isspace(c));
		i.putback(c);
	}
	while (i.good()) {
		c = i.get();
		if (c == EOF || !i.good())
			break;
		else if (s.isstr)
			if (isspace(c)) {
				i.putback(c);
				break;
			}
		if (len >= s.getSize()-1)
			s+=CHUNKSIZE;
		s[len++] = c;
	}
	if (s.isstr)
		s[len] = 0;
	s.setSize(len);
	return i;
}
