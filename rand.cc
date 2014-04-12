//
// Source     : rand.cc
// Author     : Darren Platt
// Version    : 1.1
// Date Begun : 10.5.91
// Last Rev   : 92/05/18
// Id String  : @(#)rand.cc	1.1
// 
// Modification log
//-----------------
// 0.0  tjp		Tom was here.
// 1.0	daz		Expanded to use a class
// 1.1	daz		Added uniformInt and put in SCCS
//
// Portable random number generator.
// Modified lahmer, generates integers in range min_long to max_long
// 1.6.91 Expanded to include a random number class.

static char *WhatString = "@(#)rand.cc	1.1";

#include "rand.h"
#include <assert.h>
#include <math.h>
#ifndef MALLOC_H_MISSING
#include <malloc.h>
#endif

unsigned long 	global_table[128];
unsigned long 	*rand_table = global_table;
unsigned long 	lahmer_seed;
long 			taus_seed;

void seed_lahmer(long seed)
{
	lahmer_seed = seed;
}
unsigned long lahmer(void)
{
	return lahmer_seed=(69069l*lahmer_seed+1);
}
void fill_table(void)
{
	for(int i=0;i<128;i++)
		rand_table[i] = lahmer();
}
void seed_taus(long seed)
{
	taus_seed = seed;
}
long taus(void)
{
	const long k_value = 123456;

	if (taus_seed >=0) return (taus_seed = (taus_seed<<1)%1000003);
	else return taus_seed = ((taus_seed<<1)^k_value)%1000003;
}
void seed_random(unsigned long seed)
{
	seed_lahmer(seed);
	seed_taus((signed long)seed);
	fill_table();
}

unsigned long my_rand(void)
{
	int choice = taus()%128;
	if (choice<0) choice = -choice;
	unsigned long ret_value = rand_table[choice];
	rand_table[choice] = lahmer();
	return ret_value;
}
//
// Method definitions for the randcl class
//

inline double randcl::quick_unit(void)
{
	return (my_rand()+0.5)/(double)max_ulong;
}
inline void randcl::ungrab(void)
{
	taus_seed = ::taus_seed;
	lahmer_seed = ::lahmer_seed;
	rand_table = ::rand_table;
}
inline void randcl::grab(void)
{
	::taus_seed = taus_seed;
	::lahmer_seed = lahmer_seed;
	::rand_table = rand_table;
}

void randcl::seed(unsigned long seed)
{
	grab();
	seed_random(seed);
	ungrab();
}
randcl::randcl(void)
{
	assert(rand_table = new unsigned long[128]);
	seed(1);
	phase = 0;
}
randcl::randcl(unsigned long seed)
{
	assert(rand_table = new unsigned long[128]);
	randcl::seed(seed);
	phase=0;
}
double randcl::neg_exp(double mean)
{
	return -mean*log(unit());
}
double randcl::normal(double mean,double sd)
{
	if (phase) {	// Already have one stored up.
		phase = 0;
		return (sqratio * q * sd)+mean;
	}
	double	p,v;
	grab();
	do {
		p = quick_unit()*2-1; q = quick_unit()*2-1;
		v = p*p + q*q;
	} while(v > 1.0 || v <0.25);
	sqratio = sqrt(-2*log(quick_unit()) / v);
	ungrab();
	phase = 1;
	return (sd * sqratio * p)+mean;
}
long randcl::uniformInt(long lower,long upper)
{
	return (ulong()%(upper-lower+1))+lower;
}

double randcl::uniform(double lower,double upper)
{
	return (upper-lower)*unit()+lower;
}
randcl::~randcl()
{
	delete rand_table;
}
double randcl::unit(void)
{
	grab();
	unsigned long	ul = my_rand();
	ungrab();
	return (ul+0.5)/(double)max_ulong;
}
unsigned long randcl::ulong(void)
{
	grab();
	unsigned long ul = my_rand();
	ungrab();
	return ul;
}

#ifdef RAND_TEST
#include <iostream.h>
main()
{
	randcl Myrand(21);
	for(int i=0;i<20;i++) {
		cout << Myrand.uniformInt(-1,1) << endl;
	}
}
#endif
