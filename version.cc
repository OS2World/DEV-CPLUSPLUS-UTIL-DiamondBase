/**************************************************************************
 * Source Id :
 *
 * $Id: version.cc,v 1.1 1993/05/11 15:14:13 kevinl Exp $
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
 *  Output version info for submission with bugs/comments etc
 *
 *  Original Author : Krazy Kev
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: version.cc,v $
// Revision 1.1  1993/05/11  15:14:13  kevinl
// Initial revision
//
 **************************************************************************/

#include "dbase.h"
#include <iostream.h>

diamondBase theDiamondBase;

int main(void)
{
	cout << theDiamondBase.version(true);
	return 0;
}
