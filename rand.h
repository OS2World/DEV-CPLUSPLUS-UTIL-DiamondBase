//
// Source     : rand.h
// Author     : Darren Platt
// Version    : 1.1
// Date Begun : 10.5.91
// Last Rev   : 92/05/18
// Id String  : @(#)rand.h	1.1
// 
// Modification log
//-----------------
// 0.0  tjp		Tom was here.
// 1.1	daz		Added to SCCS

#ifndef _RANDOM_
#define _RANDOM_
void seed_random(unsigned long);
unsigned long my_rand(void);

const unsigned long max_ulong = 0xffffffff;

// The random number generator class contains a number of methods for
// generating random numbers with different distributions. It also has
// its own seeds making it good for use in simulations.
class randcl {
		unsigned long 	*rand_table;
		unsigned long 	lahmer_seed;
		int				phase;
		double			sqratio,q;
		long 			taus_seed;
		inline	void	grab(void);
		inline	void	ungrab(void);
		inline	double	quick_unit(void);
	public:
					randcl(unsigned long);	// seed
					randcl(void);
					~randcl();
	unsigned long	ulong(void);
		void		seed(unsigned long);
		double		unit(void);
		double		neg_exp(double);		// mean
		double		uniform(double,double);	// lower,upper
		long		uniformInt(long,long);	// lower,upper
		double		normal(double,double);	// mean,sd
};
#endif
