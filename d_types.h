#ifndef __DIAMOND_TYPES__
#define __DIAMOND_TYPES__

// Diamond base type definitions.

#include <dbstr.h>

typedef double moneyType;
typedef long dateType;
typedef long uniqueType;

// type enumeration.

enum {D_LONG, D_ULONG, D_SHORT, D_USHORT, D_DOUBLE, D_FLOAT, D_MONEY, D_DATE, D_CHAR, D_UNIQUE, D_STRING, D_DATA, D_ICHAR};

struct d_int
{
	char data[4];

	d_int(long val=0);
	d_int(const d_int& from);

	d_int& operator = (const d_int& from);
	d_int& operator = (const long from);

	operator long() const;
};


#endif
