/**************************************************************************
 * Source Id :
 *
 * $Id: pathinfo.h,v 1.2 1993/03/25 22:26:13 davison Exp $
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
 *
 *		Pathinfo structure. Used to relay path information to objects.
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: pathinfo.h,v $
 * Revision 1.2  1993/03/25  22:26:13  davison
 * Path information structure .
 *
 * Revision 1.1  1993/03/20  15:07:05  davison
 * Initial revision
 *
 **************************************************************************/

#ifndef __PATHINFO_H__
#define __PATHINFO_H__

class TpathInfo
{
public:
//-----
	char* path;
	char* prefix;

	TpathInfo(const char* pth, const char* pfx)
	{
		path = new char[strlen(pth)+3];
		prefix = new char[strlen(pfx)+3];
		strcpy(path,pth);
		if(path[strlen(path)-1] != '/')
			strcat(path,"/");
		strcpy(prefix,pfx);
	}

	~TpathInfo()
	{
		delete path;
		delete prefix;
	}
};
#endif

