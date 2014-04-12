/*-------------------------------------------------------------------------
 * Source Id :
 *
 * $Id: gib.cc,v 1.22 1993/11/05 13:46:57 kevinl Exp $
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
 *  multi-server module
 *
 *  Original Author : Darren Platt
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: gib.cc,v $
// Revision 1.22  1993/11/05  13:46:57	kevinl
// Protocol and fixes
//
// Revision 1.21  1993/11/05  13:06:52	darrenp
// Changed signal handler prototypes to differentiate linux from CC
//
// Revision 1.20  1993/11/05  12:45:05	kevinl
// Fixed problems that CC picked up (and some multi bugs)
//
// Revision 1.19  1993/11/05  07:23:02	kevinl
// Made all clients use same channel and moved pid into message
// gib now removes the message queue nicely
// Handles missing clients that have been cleaned up and have messages
// outstanding
//
// Revision 1.18  1993/11/04  23:08:37	darrenp
// Made signal callbacks typesafe (to keep linux happy)
// added time.h include (same thing - for interval timers)
//
// Revision 1.17  1993/11/04  15:10:53	kevinl
// recMess now has 3 state return
// Alarm set to clean up dead clients
// creat replaces open
// More signal handling
// We now do the shared memory creation/removal
// Removed fork
//
// Revision 1.16  1993/11/03  14:00:24	darrenp
// some prototypical logging - not being used.
//
// Revision 1.15  1993/11/03  12:41:54	darrenp
// Added sigterm shutdown facilitites.
//
// Revision 1.14  1993/11/03  11:29:11	davison
// Ultrix fix.
//
// Revision 1.13  1993/10/31  17:22:11	darrenp
// Fix to returnErr to shut up compiler.
//
// Revision 1.12  1993/10/31  16:25:15	darrenp
// added ipcproto include,
// numerous linked list fixes.
//
// Revision 1.13  1993/10/31  13:24:12	daz
// Fixed a generic bug in the delXXX functions where the head
// of the list was being deleted.
//
// Revision 1.12  1993/10/31  12:22:51	daz
// Not passing shmId correctly to shmctl for delete - fixed.
//
// Revision 1.11  1993/10/29  15:54:37	darrenp
// bug fixes in memorary attachment.
//
// Revision 1.10  1993/10/24  14:35:55	daz
// Changes to permit compilation under SunOS - debugging info
// removed, big endian bug fix.
//
// Revision 1.9  1993/10/13  10:26:28  daz
// *** empty log message ***
//
// Revision 1.8  1993/10/04  10:53:53  daz
// *** empty log message ***
//
// Revision 1.7  1993/07/21  14:25:42  daz
// *** empty log message ***
//
// Revision 1.6  1993/07/21  10:45:44  daz
// *** empty log message ***
//
// Revision 1.5  1993/07/04  16:08:03  daz
// *** empty log message ***
//
// Revision 1.4  1993/07/04  15:00:44  daz
// *** empty log message ***
//
// Revision 1.2  1993/07/04  13:42:19  daz
// *** empty log message ***
//
// Revision 1.1  1993/07/04  02:21:13  daz
// Initial revision
//
 **************************************************************************/

// This program is the front-end to the multi-user version of the database


#include "gib.h"
#include "dcdefs.h"

// Need these for inter process communication.
#include <sys/types.h>
extern "C"
{
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
}

#ifndef linux
#include "ipcproto.h"
#endif

#ifdef __EMX__
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

//
// Globals
//
diamondBase theDiamondBase("server.db");
long		msqId;				// Message queue id for the server
long		shmId;				// Shared memory id for the server
long		serverRefId = 0;	// ids we hand out to clients.
bool		shutdown = false;	// Flag to indicate shutdown request.

//
// Prototypes - local
//
void handleAttach(long m[]);	// Handle attachment.
void handleTrans(long m[]);		// Handle transaction.
#ifdef __CC
void termHandler(int,...);		// Handles SIGTERM requests
void alarmHandler(int,...);		// Handles ALARM signals
#else
void termHandler(int);			// Handles SIGTERM requests
void alarmHandler(int); 		// Handles ALARM signals
#endif
void performShutdown(void);		// Shutdown server.
void goIntoBackground(void);	// Become a proper server.

// This maintains a list of all the nodes that we are faking.
fakeNode	*firstFO = 0;
clientInfo	*firstCI = 0;
long		clientCount = 0;

//
// Setup the alarm timer
//
void
setAlarm(void)
{
	signal(SIGALRM, alarmHandler);
	struct itimerval v;
	v.it_interval.tv_sec = v.it_value.tv_sec = 60;
	v.it_interval.tv_usec = v.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &v, 0);
}

//
// Remove the message queue and exit
//
void
gibExit(int ret)
{
	msqid_ds	dummy;
	msgctl(msqId,IPC_RMID,&dummy);
	exit(ret);
}

//
// Life as a server is pretty tough - set up shop and
// wait for something interesting to do.
//
main(void)
{
	// Well, all we really need to do now is set up the inter process
	// communication and wait for database requests.

	// Create a message queue identifier
	// Use the character identifier 'S' and the rendezvous
	// point 'gibraltar' - seems a fairly solid place to setup a
	// database.

	cout << "Diamondbase Jeweler Server: Version " << serverVersion << endl;

	// Create a file which will be used as a rendezvous point for clients
	// wishing to connect to us.

	long gibraltarFD = creat("gibraltar", 0600);
	if (gibraltarFD==-1) {
		cerr << "Couldn't establish rendezvous point gibraltar" << endl;
		exit(1);
	}
	close(gibraltarFD);

	// Create a key from the file system rendezvous point.
	key_t key = ftok("gibraltar",'S');

	// Create the actual message queue
	// Must nuke existing ones which might have pending messages.
	//------------------------------------------------------------
	//
	// See if there is an existing one.

	msqId = msgget(key,0600);

	if (msqId != -1) {
		// Should have failed.
		// There is an existing queue - destroy it.
		msqid_ds	dummy;
		msgctl(msqId,IPC_RMID,&dummy);
	}

	// Now create it.
	msqId = msgget(key,IPC_CREAT | 0600);

	// Did an error occur ?
	if (msqId == -1) {
		cerr << "Couldn't establish server message queue" << endl;
		perror("Reason:");
		exit(1);
	}

	// Diagnostics
	// ---------------------------------
	// cout << "MsqId = " << msqId << endl;
	// cout << "ShmId = " << shmId << endl;

	// Insert wait loop here for incoming requests.
	// Now an incoming attach request might be one long,
	// so allocate that much space for the moment.
	// Plus space for the message type.

	long	mess[100]; // Maximum message length.

	// Set up a signal handler to permit incoming term signals
	// indicating we should shutdown.
	signal(SIGTERM,termHandler);
	signal(SIGHUP,termHandler);
	signal(SIGINT,termHandler);

	setAlarm();

	// Background this process.
	// A few things we want to do here - we set up our own
	// session id, so we aren't attached to the process group which
	// created us. We will also open a log file to record any output.
	goIntoBackground();

	// Loop around, expecting a lot of these sorts of
	// requests.
	while(1) {
		// Get the message with least number less than
		// abs(BASE_RESP - 1). That way we hear about requests
		// and attach messages, but ignore outgoing responses which
		// we have generated and which are meant for other clients.
		// see diagram below.

		// cout << "DIAG: Waiting for message with # <" << BASE_RESP -1 << endl;

		// Check for posted shutdown request.
		if (shutdown) performShutdown();

		// Get a message in our request range ( new attachments, or
		// client requests.
		switch (recMess(mess,399,-(BASE_RESP-1))) {

		case rcvERR:
			cerr << "Error reading messages" << endl;
			perror("");
			gibExit(1);

		case rcvINTR:
			continue;

		case rcvOK:
			// Nothing
			break;
		}

		// Check for posted shutdown request.
		if (shutdown) performShutdown();

		// Decide what sort of message it is.
		// Incoming messages for attaches always occur on the
		// one message channel - other transaction requests
		// come in on a channel specific to the client's pid.
		//
		// The message space is partitioned as follows:
		// 0	     <- lowest
		//
		// ATTACH_CHANNEL <- Attach requests on this channel.
		//
		// BASE_RESP  <- responses go out at here + pid
		//
		//
		// BASE_REQ <- requests come in at here + pid.
		//
		// <--------- Highest

		if (mess[0] == ATTACH_CHANNEL) {
			// It's a new request to attach.
			handleAttach(mess);

		} else if (mess[0] >=BASE_REQ && mess[0] <BASE_RESP) {
			// It's a transaction request message.
			handleTrans(mess);
		} else {
			// Hmmm - shouldn't get other kinds of messages !
			cerr << "Unexpected message type received # " << mess[0] << endl;
		}
	}
}

// Client list handling routines.
// Just a dumb list for the moment
// - should be implememented as a binary search tree
// or a hash table.

void
insertCI(clientInfo *f)
{
	// Insert at head of list - be lazy.
	f->prev = 0;
	f->next = firstCI;
	if (f->next) f->next->prev = f;
	firstCI = f;
}

clientInfo *
findCI(long pid)
{
	// Find clientInfo in list.
	clientInfo *f;
	for(f=firstCI;f;f=f->next) {
		if (f->pid == pid) break;
	}
	return f;
}

void
delCI(clientInfo *f)
{
	// Remove fake object from list.
	if (f->prev) f->prev->next = f->next;
	if (f->next) f->next->prev = f->prev;
	if (!f->prev) firstCI = f->next;
}

// Fake node list handling routines.
// Just a dumb list for the moment - should probably move to
// fake.cc - could be implememented as a binary search tree
// or a hash table.

void
insertFO(fakeNode *f)
{
	// Insert at head of list - be lazy.
	f->prev = 0;
	f->next = firstFO;
	if (f->next) f->next->prev = f;
	firstFO = f;
}

fakeNode *
findFO(long client,long refId)
{
	// Find fake node in list.
	fakeNode *f;
	for(f=firstFO;f;f=f->next) {
		if (f->client == client && f->refId == refId) break;
	}
	return f;
}

void
delFO(fakeNode *f)
{
	// Remove fake object from list.
	if (f->prev) f->prev->next = f->next;
	if (f->next) f->next->prev = f->prev;
	if (!f->prev) firstFO = f->next;
}

fakeObject::fakeObject(const char *relName) : diaGRel(relName)
{}

// Code for processing transactions

//
// The initial attach request which comes in on
// the attach channel is handled here.
// The initial attach doesn't involve much - when the
// client is ready to start some serious dialogue with the
// server, it must send a proper attach transaction. This is just to
// reserve the channel, and assign id numbers etc.
// ---------------------------------------------

void handleAttach(long m[])
{
	// Ok - the client should have sent their process id
	// as the second long of the message. The request and
	// repsonse message ids can be calculated from their pid.

	// Now attach to the shared memory transfer area for use
	// in data transfer operations between server and client.
	// Note that it has a maximum size - that's a bit of a bummer
	// really, since the relation could theoreticallly be larger.
	// They have created a shared memory segment under the name
	// /tmp/diamond.pid type 'C' - which we should be able to
	// attach to.

	char buffer[100];
	ostrstream	rendezvous(buffer,sizeof buffer);

	long protocol = (m[1] & (255 << 24)) >> 24;
	m[1] -= protocol;

	if (protocol <= serverProtocol) {
		cerr << "Attach request from pid " << m[1] << " with protocol " << protocol << " - too old." << endl;
		returnErr(-db_protocol, m[1]);
		return;
	}

	rendezvous << "/tmp/diamond." << m[1] << '\000';
	int fd;
	if ((fd=creat(buffer,0600)) == -1) {
		perror("Rendevous creation");
		returnErr(-db_err, m[1]);
		return;
	}

	close(fd);
	// cout << "Shared buffer '" << buffer << "'" << endl;

	key_t key = ftok(buffer,'C');

#if DEBUG
	cerr << "ftok(" << buffer << ") returned " << key << endl;
#endif

	shmId = shmget(key, MAX_TRANS_AREA, 0600 | IPC_CREAT);

#if DEBUG
	cerr << "shmget returned " << shmId << endl;
#endif

	if (shmId == -1) {
		cerr << "GIB: Couldn't establish server transfer area for pid"
			<< m[1] << endl;
		perror("Reason:");
		returnErr(-db_nomem,m[1]);
		return;
	}

	// Now try to attach to the shared memory segment
	// which is used for data transfer. Allocate a piece of
	// memory for us, and don't round down our supplied address (no address)
	char *transArea = (char*)shmat(shmId,0,0);

	if ((long)transArea==-1) {
		cerr << "GIB: Couldn't attach to server transfer area " << shmId << endl;
		perror("Reason:");
		returnErr(-db_nomem,m[1]);
		return;
	}
#if DEBUG
	else
		cerr << "GIB: SUCCESSFULLY attached to server transfer area " << shmId << endl;
#endif

	// Insert details of this new client onto a list,
	// the list remembers their shared memory id, their transfer
	// area and their pid, since later connections will have to
	// remember the transfer area being used for that particular
	// client.
	clientInfo	*ci = new clientInfo;
	ci->pid = m[1];
	ci->transArea = transArea;
	ci->shmId = shmId;
	insertCI(ci);
	clientCount++;

	// Diagnostics:
	// ----------------------
	cout << "+pid " << m[1] << endl;

	// Return an ok message, cite their pid to the response routine
	// so that it knows on which channel to respond.
	returnErr(-db_ok,m[1]);
}

//
// Generic reply routine.
// Caller cites the client id number (their pid)
// and a return code (one long).

void returnErr(long e,long client)
{
	// Generic routine for passing back a single long
	// value which is an error code.
	long	buffer[2];
	buffer[1] = e;

	if (!sendMess(buffer,sizeof(long),client+BASE_RESP)) {
		cerr << "Failed to send error code to client" << endl;
	}
}

// Handle the attachment of a new class from a client.
// Involves creating a new diagrel to handle the database
// requests at this end.

void relAttach(long client,const char *relName)
{
	// Diagnostics:
	// Note the request to attach.
	// -----------------------------
	// cout << "request to attach '" << relName << "' for " << client << endl;

	// We need to register a new fake object * with the
	// database engine. Build one first

	fakeObject *f;
	fakeNode *fn;
	f = new fakeObject(relName);
	fn = new fakeNode;

	if (!f || !fn) {
		// Ran out of memory in server - bad things will ensue.
		returnErr(-db_nomem,client);
		return;
	}

	// Check to see if registration with the dbEngine went
	// ok ?
	long	refId;
	if (!f->ok()) {
		// Hmm - it didn't !
		returnErr(-(f->status),client);
		delete f;
		delete fn;
	} else {
		// Purrr - we just need to tell them what
		// our choice of refId was. Ask diaGRel.

		refId = fn->refId = f->getRefId();

		//
		// Also, set up the copy of the transArea which is common to that
		// process. Get this from the client info structure.
		//
		clientInfo *ci = findCI(client);

		if (!ci) {
			delete f;
			delete fn;
			return;
		}

		f->transArea = ci->transArea;

		returnErr(refId,client);

		// We should also place this fake object onto our list.

		f->client = fn->client = client; // redundant ?
		fn->obj = f; // Might change in a non linked list imp.
		insertFO(fn);
	}
}

// Remove an existing object.
// They have requested a detach.

void relDetach(long client,long refId)
{
	// Find the client they want.
	fakeNode *f = findFO(client,refId);
	if (!f) {
		// Whinge if we couldn't find it.
		returnErr(-db_nfound,client);
		return;
	}

	// Delete the fakeObject itself. - this will cause the
	// diaGRel to detach from the engine, as required.
	delete f->obj;

	delFO(f); // Remove node from list
	delete f; // Remove node from memory

	returnErr(db_ok,client);
}

void clientDetach(long client)
{
	// A process is quitting at the other end,
	// we should go through and clean up any of its
	// remaining fakeObjects.

	// This operation could also occur on the initiative of the server
	// if it suspects a client process has died.
	fakeNode *f,*g;
	for(f=firstFO;f;f=g) {
		g=f->next;
		if (f->client == client) {
			// Free it from memory and the list.
			delete f->obj;
			delFO(f);
			delete f;
		}
	}
	// And remove its client object.
	// First nuke its shared memory segment.
	clientInfo *ci = findCI(client);
	if (!ci)
		return;
	shmid_ds dummy;
	if (shmdt(ci->transArea) == -1)
		perror("Detaching from transfer area");
	if (shmctl(ci->shmId,IPC_RMID,&dummy) == -1)
		perror("Removing shared memory segment");

	// Suck up all the client's messages

	// Diagnostics:
	// ----------------------
	clientCount--;
	char buffer[100];
	ostrstream	rendezvous(buffer,sizeof buffer);

	rendezvous << "/tmp/diamond." << client << '\000';
	unlink(rendezvous.str());
	cout << "-pid " << client << endl;

	// Now get rid of it.
	delCI(ci);
}

void processUnknown(long client, char type)
{
	// cout << "Process with id #" << client << " issued unknown "
	//	"transaction type '" << type << "'" << endl;
	returnErr(-db_err,client);
}

void processObjTrans(long client,long refId,long trans,long idx)
{
	// Carry out a specific operation on a specific object
	// using the database engine.
	// Make the db engine do the actual work - kick up our heels
	// and take life easy for a while till the database starts demanding
	// work from our fakedObject's diaGRel base class.

	fakeObject *f = findFO(client,refId)->obj;
	if (!f) return; // Not found.

	memcpy(f->theData,f->transArea,f->getDataLength());
	long d1,d2;

	d1 = ((long *)f->theData)[0];
	d2 = ((long *)f->theData)[1];
	// cout << "Incoming '" << (char *)(f->theData)+8 << "' " <<
	//	d1 << " " << d2 << endl;
	// cout << "Execute trans type " << trans << endl;

	dbError e = theDiamondBase.trans(
					refId,
					(diamondBase::transId)trans,
					idx);

	memcpy(f->transArea,f->theData,f->getDataLength());
	d1 = ((long *)f->theData)[0];
	d2 = ((long *)f->theData)[1];
	// cout << "Outgoing '" << (char *)(f->theData)+8 << "' " <<
	//	d1 << " " << d2 << endl;

	// Send back a message of type 'q' with data = retCode

	long	tmp[3]; 	// temporary place to construct message.
	tmp[1] = (long)'q'; // operation.
	tmp[2] = e;			// error code.

	// Send it back on the clients channel.
	sendMess(tmp,sizeof(long)*2,client + BASE_RESP);
}

//
// This routine dispatches the incoming transactions
// from the clients. They fall into several categories.
// Nothing more than a glorified case statement which works
// out which client we are talking to.

void handleTrans(long m[])
{
	// We now get the pid as number 1
	//long	client = m[0] - BASE_REQ;
	long protocol = (m[1] & (255 << 24)) >> 24;
	m[1] -= protocol;

	long	client = m[1];

	// This is a bit ugly - the transaction code always
	// gets put just after the first long word.
	char	type = ((char *)m)[2*sizeof(long)];

	// Dispatch different transaction types.
	switch(type) {
		case 'A': // Attach
			relAttach(client,((char *)m)+2*sizeof(long)+1);
			break;
		case 'D': // Detach
			relDetach(client,m[3]);
			break;
		case 'T': // General transaction on an object
			processObjTrans(client,m[3],m[4],m[5]);
			break;
		case 'Q': // Quit
			clientDetach(client);
			break;
		default:
			processUnknown(client,type);
	}
}

void
checkTimer()
{
	struct itimerval v;
	getitimer(ITIMER_REAL, &v);
	cerr << "TIMER: " << v.it_interval.tv_sec << "," << v.it_value.tv_sec << endl;
}

// Some IPC wrappers.
// Have fun reverse engineering them under a non-IPC
// environment.
// ---------------------------

bool sendMess(void *data,long len,long chan)
{
	// The start of the pointed to memory must have the
	// message type in it, so we stick it in - the caller *must*
	// leave room for it.

	// NB This is slightly different from the dclient.cc version which
	// also attaches the pid.

	((long *)data)[0] = chan;

	// Send the data off to the server.
	long retCode = msgsnd(msqId,(msgbuf *)data,len,0);
#ifdef linux
	if (retCode!=len) {
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

rcvRes recMess(void *data,long len,long chan)
{
	// Grab the allotted data from the server

	//checkTimer();
	long retCode = msgrcv(msqId,(msgbuf *)data,len,chan,0);
	if (retCode == -1) {
		if (errno == EINTR) {
			cerr << "EINTR" << endl;
			return rcvINTR;
		} else {
			perror("Receiving message");
			return rcvERR;
		}
	}
	// cout << "DIAG: recmess " << ((long *)data)[0] << endl;
	return rcvOK;
}

//
// Handle signal terminations by setting a flag
// which indicates we should shutdown as soon as we complete
// the current transaction.
//
void
#ifdef __CC
termHandler(int,...)
#else
termHandler(int)
#endif
{
	shutdown = true;
	cout << "Server shutdown requested" << endl;

	cout << clientCount << " clients remaining" << endl;
}

//
// Handle the periodic alarm to check for shared mem attachments
//

void
#ifdef __CC
alarmHandler(int,...)
#else
alarmHandler(int)
#endif
{
	clientInfo *c = firstCI, *n;

	struct shmid_ds buf;

	cerr << "ALARM HANDLER" << endl;

	while (c) {
		shmctl(c->shmId, IPC_STAT, &buf);
		cerr << "ALARM: shmId: " << c->shmId << "     attachments " << buf.shm_nattch << endl;

		// Store the next pointer in case clientDetach affects the current client
		n = c->next;

		// If there is 1 or fewer attachments, the client is gone.

		if (buf.shm_nattch <= 1) {
			clientDetach(c->pid);
			cerr << "     : Process Detached" << endl;
		}

		c = n;
	}
	signal(SIGALRM, alarmHandler);
}

void
performShutdown()
{
	// There is a bit of a dilemna over what to do here when
	// a shutdown is requested. Be nice and wait for all the
	// little client thingies to go home.

	if (clientCount) return;
	cout << "shutdown done." << endl;
	cout.flush();
	cerr.flush();
	gibExit(0);
}

void
goIntoBackground(void)
{
//	if (fork()) exit(0);

	// Although taken from another working program, this
	// logging code results inthe output being lost, so we
	// shall ignore it for the moment.
#if 0
	setsid();
	freopen("/dev/null","r",stdin);

	// Make sure our logfile is opened with the correct flags
	long fd = open("jeweler.log", O_APPEND | O_WRONLY | O_CREAT,0600);

	fclose(stdout);
	fclose(stderr);

	if (fd != 1) {
		if (dup2(fd,1)==-1) perror("dup2 problem");
	}
	if (fd != 2) {
		if (dup2(fd,2)==-1) perror("dup2 problem");
	}
	if (fd != 1 && fd != 2) close(fd);
#endif
}
