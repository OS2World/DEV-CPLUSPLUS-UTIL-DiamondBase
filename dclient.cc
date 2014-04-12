/**************************************************************************
 * Source Id :
 *
 * $Id: dclient.cc,v 1.20 1993/11/05 13:46:57 kevinl Exp $
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
 *  Client code for multi-db
 *
 *  Original Author : Darren Platt
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dclient.cc,v $
// Revision 1.20  1993/11/05  13:46:57	kevinl
// Protocol and fixes
//
// Revision 1.19  1993/11/05  12:45:05	kevinl
// Fixed problems that CC picked up (and some multi bugs)
//
// Revision 1.18  1993/11/05  11:45:04	darrenp
// Fix for linux - not accounting for the # bytes sent in a message
// correctly, and linux doesn't use -1 for an error condition.
// You added len+sizeof(long) and the return checks for len bytes sent.
//
// Revision 1.17  1993/11/05  07:23:02	kevinl
// Made all clients use same channel and moved pid into message
// gib now removes the message queue nicely
// Handles missing clients that have been cleaned up and have messages outstanding
//
// Revision 1.16  1993/11/04  15:07:01	kevinl
// We now let the server remove the shared memory segment
//
// Revision 1.15  1993/11/04  13:46:44	darrenp
// Client formally detaches from its given shared memory segment
// after notifying server of its intent to die.
//
// Revision 1.14  1993/11/04  13:14:02	darrenp
// Handed over responsibility for creating the rendezvous point
// and the ahred memory segment to the server
//
// Revision 1.13  1993/11/04  11:30:38	darrenp
// Removed unlink code.
//
// Revision 1.12  1993/11/03  14:00:24	darrenp
// Cleans up diamond.pid files properly now.
//
// Revision 1.11  1993/11/03  11:29:01	davison
// Ultrix fix
//
// Revision 1.10  1993/10/31  16:25:15	darrenp
// added ipcproto include
//
// Revision 1.9  1993/10/29  15:54:37  darrenp
// bug fixes in memorary attachment.
//
// Revision 1.8  1993/10/24  14:37:00  daz
// SunOS fixes, diagnostics commented out, cmall bug fixes for
// big endian machines.
//
// Revision 1.7  1993/10/13  10:26:28  daz
// *** empty log message ***
//
// Revision 1.6  1993/07/21  14:25:42  daz
// *** empty log message ***
//
// Revision 1.5  1993/07/21  10:45:44  daz
// *** empty log message ***
//
// Revision 1.4  1993/07/04  16:08:03  daz
// *** empty log message ***
//
// Revision 1.3  1993/07/04  15:00:49  daz
// *** empty log message ***
//
// Revision 1.2  1993/07/04  13:42:19  daz
// *** empty log message ***
//
// Revision 1.1  1993/07/04  02:21:13  daz
// Initial revision
//
 **************************************************************************/

// Implement the necessary IPC calls to link this clients
// diamondBase object into the central server.

#include <iostream.h>
#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
extern "C"
{
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
}
#include "ipcproto.h"
#include <fcntl.h>
#include <errno.h>

#include "dclient.h"
#include "object.h"

// Used to keep a linked list of object pointers at this
// end to handle any requests.
struct objNode {
	object	*obj;
	long	refId;
	objNode *next;
	objNode *prev;
};

objNode *head = 0;

//
// Code for handling the object list at this end.
// We need to keep track of all the objects and their
// corresponding reference ids so that when a request
// comes from the server, the appropriate object can be called.
//

void
insertObj(objNode *f)
{
	// Insert at head of list - be lazy.
	f->prev = 0;
	f->next = head;
	head = f;
}

objNode *
findObj(long refId)
{
	// Find obj node in list.
	objNode *f;
	for(f=head;f;f=f->next) {
		if (f->refId == refId) break;
	}
	return f;
}

void
delObj(objNode *f)
{
	// Remove object from list.
	if (f->prev) f->prev->next = f->next;
	if (f->next) f->next->prev = f->prev;
	if (!f->prev) head = 0;
}
char* diamondBase::verStr(void)
{
	return "$Id: dclient.cc,v 1.20 1993/11/05 13:46:57 kevinl Exp $";
}

char* diamondBase::version(bool full)
{
	// Call across to server and ask for version string from its
	// diamondBase
	return "Yeah sure";
}

diamondBase::diamondBase(char* name)
{
	// Change the semantics of the provided name here.
	// Rather than sepcifying a relation pathname file,
	// specify the name of the server object we wish to join to.
	// In fact - that isn't even implemented - we just ignore it
	// at the moment.

	// Do an initial connection with the server and get an id for
	// this connection.
	// cout << "Multi diamondbase constructor" << endl;

	key_t key = ftok("./gibraltar",'S');

	msqId = msgget(key,0);

	// Did we attach to the message queue successfully
	if (msqId == -1) {
		cerr << "Server not found" << endl;
		perror("This was it");
		exit(1);
	}

	// We now put all requests on the same channel so they are treated equally.
	// sendChannel = BASE_REQ + getpid();

	sendChannel = BASE_REQ;
	recChannel = BASE_RESP + getpid();

	long	mess[2];

	// Send a message off to the server informing it of
	// our process id, and therefore our send and request
	// channels.

	// 0 message gets pid on it anyway
	if (!sendMess(&mess,0,ATTACH_CHANNEL)) {
		cerr << "Error connecting to server (send)" << endl;
		exit(1);
	}

	// Now wait for the reply which should consist of an error code.
	// There are potential problems if the server dies at this point -
	// it would be preferable if the client didn't hang.

	if (!recMess(&mess,sizeof(long))) {
		cerr << "Error connecting to server (receive)" << endl;
		exit(1);
	}
	// See if there was anything wrong with the connection.
	if (mess[1]) {
		// Server reported an error
		dbErr((dbError)mess[1],"Connecting to server");
		return;
	}

	// Attach to a shared memory area specifically for our id,
	// which the server will have created and attach to in order to effect
	// memory transfers.
	key_t		key2;
	char		buffer[100];
	ostrstream	rendezvous(buffer,sizeof buffer);

	// The shared memory segment will have been made to
	// rendezvous with /tmp/diamond.ourPid
	rendezvous << "/tmp/diamond." << getpid() << '\000';

	// Get the shared memory segment.
	key2 = ftok(buffer,'C');
	shmId = shmget(key2,MAX_TRANS_AREA,0);

	if (shmId == -1) {
		dbErr(db_err,"DClient: Couldn't establish server transfer area");
		return;
	}

	// Now try to attach to the shared memory segment
	// which is used for data transfer. Allocate a piece of
	// memory for us, and don't round down our supplied address (no address)
	transArea = (char*)shmat(shmId,0,0);

	if (!transArea) {
		dbErr(db_err,"Couldn't attach to server transfer area");
		return;
	}

	// Diagnostics
	// cout << "msqId = " << msqId << endl;
	// cout << "shmId = " << shmId << endl;

	// cout << "Connected ok" << endl;
	// Everything is all hooked up.
	dbErr(db_ok,"Everything is hunky dorey");
}

dbError diamondBase::detach(long refId)
{
	// Notify server of intent to disconnect
	if (msqId != -1) {
		// Transmit detach code 'D' followed by refId
		// to server and fetch return code.
		long buffer[4];
		((char *)buffer)[2*sizeof(long)] = 'D';
		buffer[3] = refId;
		if (!sendMess(buffer,sizeof(long) * 2)) return dbErr(db_comm);

		// Receive error code
		if (!recMess(buffer,sizeof(long))) return dbErr(db_comm);

		// Should remove the object's bookmark in the
		// linked list.
		objNode *o = findObj(refId);
		if (o) { // If it is in there.
			delObj(o); // Remove from list
			delete o; // Remove associated memory.
		}
		return dbErr((dbError)(buffer[1]));
	}
	return dbErr(db_ok);
}

dbError diamondBase::attach(
	const char* name,
	object* resBuffer,
	long& refId,
	dbObjData *&)
{
	// Attach to server object, assigning refId value in the process,
	// keep the object pointer somewhere - pass the name to the server
	// - will need to register the object pointers available.

	// Check if the relation is too big for our transfer area.
	// This doesn't work, apparently b/c the actual object hasn't even
	// been constructed yet - we are just cosntructing the base diaRel.
	// if (resBuffer->getDataLength() > MAX_TRANS_AREA) {
	//		return dbErr(db_toomany);
	// }

	// Send attach message, plus relation name
	long len = strlen(name) + 2 + sizeof(long);
	char *tmp = new char[len+sizeof(long)];

	// Memory allocation succeeded ?
	if (!tmp) return dbErr(db_nomem);

	// Leave first sizeof(long) bytes free for message id
	// next byte is the transaction id 'A', then rest is null
	// terminated relation name.
	tmp[2*sizeof(long)] = 'A';
	strcpy(tmp+2*sizeof(long)+1,name);

	// Transmit to server
	if (!sendMess(tmp,len)) return dbErr(db_comm);
	delete tmp;

	// Receive error code + refId if successful.
	long response[2];
	if (!recMess(response,sizeof(long))) return dbErr(db_comm);

	if (response[1]>=0) { // +ve value is refId on success
		refId = response[1];

		// We need to stash the object pointer resBuffer here
		// for future transactions that the server might require
		// us to perform.
		objNode *o = new objNode;
		if (!o) {
			detach(refId); // Say bye bye.
			return dbErr(db_nomem);
		}
		// Set up the linked list to hold it.
		o->obj = resBuffer;
		o->refId = refId;
		insertObj(o);
		return dbErr(db_ok);
	}
	// -ve value indicates error !
	refId = -1; // Flag the refId as being invalid.
	return dbErr((dbError)(response[1]));
}

void diamondBase::sendLong(long l)
{
	long	tmp[3];
	tmp[2] = l;
	sendMess(tmp,sizeof(long));
}

dbError diamondBase::trans(long refId,transId id,long idx)
{
	// Execute a transaction. This will involve stating the
	// transId and refId to the server, then waiting for any
	// messages regarding the object, and finally receiving an
	// error status message.

	if (refId == -1) {
		// They are quoting an invalid reference id - report back
		// immediately that they are in error.
		return db_refid;
	}

	long	tmp[10];

	// Three long words in this messge - the
	// reference id, the transaction we want performed and the
	// index (if necessary - send it anyway. The server will
	// know from the refId which object is involved and hence
	// which client.

	((char *)tmp)[2*sizeof(long)] = 'T';
	tmp[3] = refId;
	tmp[4] = id;
	tmp[5] = idx;

	//
	// New plan - every time we go to carry out a transaction,
	// we copy our current version of the data into the
	// shared memory area. This keeps the decision of whether
	// or not the server needs to look at the latest data, very
	// simple. It avoids the need for any messages coming back too.

	// Go through the routine of finding the object associated
	// with the refId - just so we can get its data length,
	// quite silly really - should be an easier way - server should
	// cite it at connect time, and we should store it.

	objNode *o = findObj(refId);

	assert(o);
	object *obj = o->obj;

	// Couldn't test this at attach time for some reason, so
	// do it here.

	assert(obj->getDataLength()<=MAX_TRANS_AREA);

	// cout << "Sending '" << (char *)(void *)(*obj)+8 << "'" << endl;
	memcpy(transArea,(void *)(*obj),obj->getDataLength());
	// cout << ((long *)transArea)[0] << " " << ((long *)transArea)[1] << endl;

	if (!sendMess(tmp,sizeof(long)*4)) return dbErr(db_comm);

	// Fetch message from the server.
	// Under this scheme, the only message which can come back
	// is the 'q' - quit, I'm done message. Nice and simple.

	if (!recMess(tmp,sizeof(long)*9)) {
			return dbErr(db_comm);
	}

	// Take a copy of the final data that resulted.
	memcpy((void *)(*obj),transArea,obj->getDataLength());
	// cout << "Receiving '" << (char *)(void *)(*obj)+8 << "'" << endl;
	// cout << ((long *)transArea)[0] << " " << ((long *)transArea)[1] << endl;

	return dbErr((dbError)(tmp[2])); // Pick up error code from message
}

diamondBase::~diamondBase()
{
	// Notify server of intent to disconnect.

	long tmp[3];

	((char *)tmp)[2*sizeof(long)] = 'Q'; // Quit message.
	// Transmit to server
	sendMess(tmp,sizeof(long));
	shmdt(transArea);
}

bool diamondBase::sendMess(void *data,long len,long chan)
{
	long actualChan = chan;
	// If they didn't specify a channel, then assume the
	// standard channel for communicating requests to the server.
	if (actualChan == -1) actualChan = sendChannel;

	// The start of the pointed to memory must have the
	// message type in it, so we stick it in - the caller *must*
	// leave room for it.

	((long *)data)[0] = actualChan;
	((long *)data)[1] = serverProtocol << 24 | getpid();
	// Send the data off to the server.
	long retCode = msgsnd(msqId,(msgbuf *)data,len+sizeof(long),0);
#ifdef linux
	if (retCode!=len+sizeof(long)) {
#else
	// Sun case
	if (retCode!=0) {
#endif
		perror("Transmitting message");
		return false;
	}
	// cout << "DIAG: sendmess " << ((long *)data)[0] << endl;
	return true;
}

bool diamondBase::recMess(void *data,long len)
{
	// Receive data from the server.
	long retCode = msgrcv(msqId,(msgbuf *)data,len,recChannel,0);
	if (retCode==-1) {
		perror("Receiving message");
		return false;
	}

	// The first bit of the received data should be the
	// channel number - and since we said what we wanted,
	// we should have got it - let's be paranoid.
	if (((long *)data)[0] != recChannel) {
		cerr << "Yikes - got wrong channel number!" << endl;
		return false;
	}
//	cout << "DIAG: recmess " << ((long *)data)[0] << endl;
	return true;
}

