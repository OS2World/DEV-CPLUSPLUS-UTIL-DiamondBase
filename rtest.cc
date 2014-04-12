/**************************************************************************
 * Source Id :
 *
 * $Id: rtest.cc,v 1.7 1993/05/26 01:01:39 kevinl Exp $
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
 *  Testing rserv. Bulldozing it into the ground actually.
 *
 *  Original Author : Mad Daz.
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log
 **************************************************************************/

#include <iostream.h>
#ifdef __BORLANDC__
#include <string.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "rserv.h"
#include "rand.h"
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

void fillBuffer(long seed,char *buffer,int length)
{
	// Fill a buffer with random characters
	srand(seed);
	for(int i=0;i<length;i++) {
		buffer[i] = rand()&255;
	}
}

int checkBuffer(long seed,char *buffer,int length)
{
	// Check the contents of a buffer against a string of random characters.
	srand(seed);
	char t;
	for(int i=0;i<length;i++) {
		t = rand()&255;
		if (buffer[i] != t) return 0;
	}
	return 1;
}

int
main(int argc,char **argv)
{
	if (argc==2) {
		if (!strcmp("init",argv[1])) {
			recServer	rs;
			if (rs.createDb("testdb2",13,112)) {
				cerr << "Create failed" << endl;
			}
		} else if (!strcmp("bash",argv[1])) {
			// Do some random record creation and integrity checking.
			// we will have up to 5000 records, which can be active
			// or inactive (seed == -1) - each record has a seed associated
			// with it which can be used to check the integrity of the record.
			randcl MyRand(21);
			recServer rs("testdb2",112);

			long headerSeed = MyRand.ulong();
			long seeds[5000];
			long where[5000];
			long numActive = 0;
			long iters = 0;
			char leader[112];

			fillBuffer(headerSeed,leader,112);
			if (!rs.writeData(0,leader,112)) {
				cerr << "Failed to write leader" << endl;
				exit(1);
			}

			do {
				// Pick a random operation:
				char buffer[13];

				if (MyRand.ulong()%2 || !numActive) {
					long newRec;
					if (rs.newRec(newRec)) {
						cerr << "Error creating record when numActive==" <<
							numActive << " " << iters << endl;
						exit(1);
					}
					where[numActive] = newRec;
					seeds[numActive] = MyRand.ulong();
					fillBuffer(seeds[numActive],buffer,13);
					rs.putRec(where[numActive],buffer);
					numActive++;
				} else {
					// Check an existing record to establish it's integrity
					int which = MyRand.ulong()%numActive;
					if (rs.getRec(where[which],buffer)) {
						cerr << "Retrieval failed for record #" << where[which]
							<< " " << iters << endl;
						exit(1);
					}
					if (!checkBuffer(seeds[which],buffer,13)) {
						cerr << "Integrity check failed for record #" << 
							where[which] << " " << iters << endl;
						exit(1);
					}
				}
				iters++;
				if (!iters%10) {
					// Check header integrity
					memset(leader,0,112);
					if (!rs.readData(0,leader,112)) {
						cerr << "Error reading leader" << endl;
						exit(1);
					}
					if (!checkBuffer(headerSeed,leader,112)) {
						cerr << "Leader failed integrity check" << endl;
						exit(1);
					}
				}
			} while(numActive<5000);
			cout << "Done!" << endl;
		} else if (!strcmp("big_add",argv[1])) {
			recServer rs("testdb2",112);
			int i;
			char buffer[123];
			for(i=0;i<5000;i++) {
				long idx;
				if (rs.newRec(idx)) {
					cerr << "New failed on iteration " << i << endl;
					exit(1);
				}
				sprintf(buffer,"!%d! This is the abode of record number !%d!",i,i);
				if (rs.putRec(idx,buffer)) {
					cerr << "Put failed on iteration " << i << endl;
					exit(1);
				}
				cout << ".";
			}

			memset(buffer,' ',122);
			// Doing fetches now:
			for(i=0;i<5000;i++) {
				if (rs.getRec(i,buffer)) {
					cerr << "get failed on iteration " << i << endl;
					exit(1);
				}
				cout << "<" ;
			}
		}
	} else {
		cout << "Usage " << argv[0] << " [init|bash|big_add]" << endl;
	}
}
