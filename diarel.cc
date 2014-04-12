/**************************************************************************
 * Source Id :
 *
 * $Id: diarel.cc,v 1.28 1993/10/05 07:33:35 kevinl Exp $
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
 * The diaRel is inherited into application classes to provide database
 * functionality.
 *
 *  Original Author : Daz
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: diarel.cc,v $
// Revision 1.28  1993/10/05  07:33:35  kevinl
// Now handles dbObjData
//
// Revision 1.27  1993/08/29  13:08:35  kevinl
// Problems with relation assignment
//
// Revision 1.26  1993/06/23  05:21:22  kevinl
// Mallocs are now in angular brackets
//
// Revision 1.25  1993/06/20  13:42:15  kevinl
// Fixed multiple mallocs
// Removed dbErr call in diaErr.
//
// Revision 1.24  1993/05/26  01:01:39  kevinl
// MALLOC_H_MISSING
//
// Revision 1.23  1993/05/11  14:44:50  kevinl
// Added version number output
//
// Revision 1.22  1993/05/06  04:00:19  kevinl
// SASC define for malloc.h
//
// Revision 1.21  1993/05/03  01:35:06  kevinl
// Global is called theDiamondBase now.
//
// Revision 1.20  1993/05/01  14:40:05  kevinl
// Cast to long incalls to trans
//
// Revision 1.19  1993/04/27  07:19:18  kevinl
// Added write
//
// Revision 1.18  1993/04/26  01:20:56  kevinl
// Comments
//
// Revision 1.17  1993/04/15  04:21:52  kevinl
// Moved malloc.h
//
// Revision 1.16  1993/04/14  05:49:26  kevinl
// Sees was passing ti_find by mistake
//
// Revision 1.15  1993/04/13  12:05:53  kevinl
// seekFirst & Last were not doing seeks.
//
// Revision 1.14  1993/04/11  05:49:02  kevinl
// Rationalised find/seek/peek methods etc
//
// Revision 1.13  1993/04/09  13:00:29  kevinl
// Stats can be called from diaRel now.
//
// Revision 1.12  1993/04/04  23:58:59  kevinl
// Destructor to detach
//
// Revision 1.11  1993/04/01  04:24:26  kevinl
// Added/changed transactions
// Now we have get, extract and find
//
// Revision 1.10  1993/03/30  14:38:35  kevinl
// Modified index passing to database transactions
//
// Revision 1.9  1993/03/29  23:27:37  kevinl
// Now holds it's own copy of last error
//
// Revision 1.8  1993/03/29  08:06:10  darrenp
// Removed bogus error reporting
// malloc lib
//
// Revision 1.7  1993/03/28  10:39:54  kevinl
// Modified for diaErr
//
// Revision 1.6  1993/03/28  04:53:59  root
// more error code standardization.
//
// Revision 1.5  1993/03/26  06:15:57  darrenp
// standardised error codes.
//
// Revision 1.4  1993/03/26  05:49:24  darrenp
// changed error codes.
//
// Revision 1.3  1993/03/25  22:17:07  davison
// (i) Fixed calls to "regRel". Now called attach.
// (ii) Fixed up default initialisers.
// (iii) Added a crude test for the add() primitive.
//
// Revision 1.2  1993/03/24  06:16:38  kevinl
// Changed constructor to remove pTransferArea
//
 **************************************************************************/

#if !defined(MALLOC_H_MISSING) && !defined(MALLOC_H_INCLUDED)
extern "C" {
#include <malloc.h>
}
#define MALLOC_H_INCLUDED
#endif

#include <diarel.h>

char* diaVerStr(void)
{
	return "$Id: diarel.cc,v 1.28 1993/10/05 07:33:35 kevinl Exp $";
}

// The diamondBase must be declared by the app programmer
extern diamondBase theDiamondBase;

char* diaRel::verStr(void)
{
	return diaVerStr();
}

// diaRel keeps its own status so that each relation can be queried
// separately for status.
// perror uses dbErr to print out the stored error

void diaRel::perror(char* err)
{
	dbErr(err, status);
}

// Set's the last error
dbError diaRel::diaErr(dbError err)
{
	// This was destroying any extraErr string by calling dbErr
	//return dbErr(status=err);
	return status=err;
}

// Attach to the diamondBase
// We pass in the relation name, a pointer to this diaRel and get back the
// reference id inside the diamondBase for future communication with
// diamondBase.

diaRel::diaRel(const char *name)
{
	diaErr(theDiamondBase.attach(
		name,
		this,
		refId,
		objData
	));
}

// Say goodbye to the diamondBase

diaRel::~diaRel()
{
	theDiamondBase.detach(refId);
}

// This must NOT copy refId.
// Make sure all additional fields are included here.

diaRel& diaRel::operator=(diaRel& X) {
	if (&X != this) {
		status = X.status;
	}
	return *this;
}

// The following functions simply call the diamondBase, passing in the
// reference id we were returned when we attached and the transaction type we
// are requesting.

dbError diaRel::add(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_add)); 
}

dbError diaRel::seek(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_seek)); 
}

dbError diaRel::find(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_find)); 
}

dbError diaRel::del(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_del)); 
}

dbError diaRel::write(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_write));
}

dbError diaRel::put(void) 
{ 
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_put));
}

dbError diaRel::begin(int idx) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_begin, (long)idx));
}

dbError diaRel::end(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_end));
}

dbError diaRel::extract(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_extract));
}

dbError diaRel::get(int idx) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_get,(long)idx));
}

dbError diaRel::seekFirst(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_seekFirst));
}

dbError diaRel::first(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_first));
}

dbError diaRel::seekLast(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_seekLast));
}

dbError diaRel::last(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_last));
}

dbError diaRel::next(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_next));
}

dbError diaRel::prev(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_prev));
}

dbError diaRel::peekPrev(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_peekPrev));
}

dbError diaRel::peekNext(void) 
{
	return diaErr(theDiamondBase.trans(refId,diamondBase::ti_peekNext));
}

void diaRel::stats(void)
{
	diaErr(theDiamondBase.trans(refId,diamondBase::ti_stats));
}
