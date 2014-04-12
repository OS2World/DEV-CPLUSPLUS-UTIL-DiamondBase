##########################################################
# Makefile for Diamondbase.
#
#
##########################################################
# Define your OS here.....
#    The following operating systems are supported.
#
#		"os2-bc"        - OS/2 2.0 or greater (using BCOS2).
#		"os2-emx"       - OS/2 2.x using emx/gcc 0.8h or later
#		"os2-emx-syscalls"- OS/2 2.x using emx/gcc 0.8h or later
#				    (using system calls library)
#		"ultrix"        - DEC Ultrix (using gcc). (also SG IRIX with gcc)
#		"sunos"         - SUN unix (using gcc).
#		"linux"         - LINUX (using gcc).
#		"cc"            - The (CFRONT?) CC C++ preprocessor compiler (tested
#						with IRIX)
#		"sasc"          - Amiga, Comeau C++ 3.0b (AT&T cfront 3.0), SAS/C 6.2
#	"next"          - NeXT using gcc
#		"nothing"       - Will build nothing.
#
ifndef OS
OS = os2-bc
endif
#
##########################################################
# Make options...
#    The following options may be either "on" or "off"
#
ifndef DEBUG
DEBUG = off
endif
ifndef USEMALLOCLIB
USEMALLOCLIB = off
endif
#
##########################################################
# Ok, the install directories:
#
ifndef BINDIR
BINDIR=/usr/local/bin
endif
ifndef LIBDIR
LIBDIR=/usr/local/lib
endif
ifndef INCDIR
INCDIR=/usr/local/include
endif
#
##########################################################
#
# DON'T SCREW WITH THE REST OF THIS MAKEFILE UNLESS YOU
# KNOW WHAT YOU'RE DOING.... Andy
#
# If you are running an operating system besides those above, please
# fashion a section for it below similar to the rest. You should not
# need to change anything else in the makefile. It has been designed that
# way.
#
# The following need to be defined in your OS section...
# OBJ: The object code suffix without a '.'
# CC: your C++ compiler
# CFLAGS: Compile time flags. e.g. debugging, warnings etc. You should
#   include $(MALLOC_I) if you ever plan to use the malloc library
# LOAD: The loader flag for generating object code. Usually -o<space> under
#   unix. Note that the <space> is essential otherwise compilers barf. The
#   reason this is done this way is that BC++ requires -eexecname with no
#   space.
# LDFLAGS: Link time flags. These should include $(MALLOC_L) if you plan to
#   use a the malloc library and MUST include -L. -ldiamond so that dsc can
#   can have the diamond library linked in properly. Some gcc versions require
#   a -lg++ on the end.
# DEP: Any extra dependancies, like a compiler config file (see the OS/2
#   section for an example)
# FLEXFIX: If your lex does not produce lex.yy.c, then this define should
#   be a command that generates (usually using copy or move instructions)
#   lex.yy.c form your lexical analyser's output.
# PARSERGEN: The parser that you use. e.g. yacc
# LIBRARIAN: The command and arguments to run your librarian
# RANLIB: If you need ranlib, this is where you put it. If you don't,
#   define this to equal DIR or something benign like that (eg. see the OS/2
#   section)
# LIBNAME: The name of the library
# LIBOBJ: A gnu make macro to convert the extensionless OBJS by adding a
#   suffix on eg. LIBOBJ := $(OBJS:%=%.o)
# LIBOBJS = $(LIBOBJ)
# STRIPHEAD: This should copy the yacc (or equivalent) output to dsc.cc. In
#   most cases this might read: cp y.tab.c dsc.cc. With some versions of
#   yacc, you get extern's for malloc declared at the top of your output. If
#   so, the C++ compiler will barf, so make this command a call to tail (or
#   similar) to strip off the first line. See the Sunos section for an
#   example.
# MALLOC_I and MALLOC_L should be compile time and link time options to
#   link in a malloc library if you want one. We used it to ensure there were
#   no memory leaks. Leave them blank if you want.
# RM: The program that deletes files.
# RMFLAGS: The flags to that program.
# CP: The program that copies files.
#
##########################################################

################
# Source files

ALLSRC=btree.cc bucket.cc d_types.cc dbase.cc dbobj.cc dbotest.cc \
	   diarel.cc dispr.cc generate.cc idxinfo.cc main.cc \
	   myobj.cc nserver.cc rand.cc rserv.cc rtest.cc search.cc \
	   test1.cc tester.cc dberr.cc cache.cc btdump.cc version.cc \
	   mystring.cc mserv.cc diagrel.cc db2txt.cc utils.cc

MULTISRC=gib.cc dclient.cc

MULTIHEAD=gib.h dcdefs.h dclient.h ipcproto.h

INSTALLMULTIHEAD=dclient.h dcdefs.h

DOCSRC=doc.tex dsc.tex fileform.tex mycprog.sty fancyheadings.sty dirstruc.tex multi.tex strings.tex

XTRACLEAN= doc.aux doc.dvi doc.log doc.ps

OTHERSRC=dsc.l dsc.y ns.dbc

RELATIONHEAD=btree.h d_types.h dbase.h dbobj.h defs.h diarel.h \
		idxinfo.h nserver.h object.h dbstr.h \
		rserv.h dberr.h cache.h version.h mystring.h mserv.h \
		diagrel.h ordcomp.h utils.h

ALLHEAD=$(RELATIONHEAD) generate.h myobj.h pathinfo.h rand.h

OTHERDIST=Makefile Makefile.gen BETA README Copyright Diamond.cfg CHANGES lex.yy.c

DIST=$(ALLSRC) $(DOCSRC) $(OTHERSRC) $(ALLHEAD) $(OTHERDIST) $(MULTISRC) $(MULTIHEAD)

BINARIES=dsc db2txt

DISTFILES:=$(DIST:%=diamond/%)

################
# List of object files

OBJS=btree bucket d_types dbase dbobj diarel generate idxinfo nserver \
	 rserv dberr cache mystring mserv diagrel utils

################
# Flags for building test databases.

TESTDBFILES=-O test/dbfiles
TESTSCHEMA=-I test/schema

################
# Now the OS specific stuff.

#nothing stuff
ifeq ($(OS),nothing)
error: nothing_error
all: nothing_error
clean: nothing_error
lib: nothing_error
dsc: nothing_error
depend: nothing_error

nothing_error:
	@echo You must set the OS= line in Makefile or use make OS=something
	@_____Error

endif

ifeq ($(DEBUG),on)
  DFLAGS = -DDEBUG
endif

#NeXT Defines
ifeq ($(OS),next)
  SYSDEFS = -DMALLOC_H_MISSING
  OBJ=o
  CC = gcc
  CFLAGS = -pipe -O -Wall $(SYSDEFS)
  LOAD = -o
  LDFLAGS = -L. -ldiamond -lg++
  DEP =
  FLEXFIX =
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = ranlib
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  LIBNAME = libdiamond.a
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = tail +2 y.tab.c > dsc.cc
  ifeq ($(USEMALLOCLIB),on)
    MALLOC_I = -I/usr/local/gnu/include/dbmalloc
    MALLOC_L = -ldbmalloc
  else
    MALLOC_I =
    MALLOC_L =
  endif
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#Sunos Defines
ifeq ($(OS),sunos)
  OBJ=o
  CC = gcc
  CFLAGS = -g -Wall $(MALLOC_I)
  LOAD = -o
  LDFLAGS = $(MALLOC_L) -L. -ldiamond -lg++
  DEP =
  FLEXFIX =
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = ranlib
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  LIBNAME = libdiamond.a
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = tail +2 y.tab.c > dsc.cc
  ifeq ($(USEMALLOCLIB),on)
    MALLOC_I = -I/usr/local/gnu/include/dbmalloc
    MALLOC_L = -ldbmalloc
  else
    MALLOC_I =
    MALLOC_L =
  endif
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#SASC defines
ifeq ($(OS),sasc)
  SYSDEFS = -DMALLOC_H_MISSING
  OBJ=o
  CC = rx como
  CFLAGS = $(SYSDEFS)
  LOAD = -o
  LDFLAGS = -ldiamond.lib
  DEP =
  FLEXFIX =
  PARSERGEN = byacc
  LIBRARIAN = oml
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  RANLIB = echo "No ranlib required"
  LIBNAME = diamond.lib
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = cp y.tab.c dsc.cc
  MALLOC_I =
  MALLOC_L =
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#linux Defines
ifeq ($(OS),linux)
  OBJ=o
  CC = gcc
  CFLAGS = -g -Wall $(MALLOC_I)
  LOAD = -o
  LDFLAGS = $(MALLOC_L) -L. -ldiamond -lg++
  DEP =
  FLEXFIX =
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = ranlib
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  LIBNAME = libdiamond.a
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = cp y.tab.c dsc.cc
  ifeq ($(USEMALLOCLIB),on)
    MALLOC_I = -I/usr/local/gnu/include/dbmalloc
    MALLOC_L = -ldbmalloc
  else
    MALLOC_I =
    MALLOC_L =
  endif
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#CC Defines
ifeq ($(OS),cc)
  OBJ=o
  CC = CC
  CFLAGS = -g -D__CC $(MALLOC_I)
  LOAD = -o
  LDFLAGS = $(MALLOC_L) -L. -ldiamond
  DEP =
  FLEXFIX =
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = ranlib
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  LIBNAME = libdiamond.a
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = cp y.tab.c dsc.cc
  ifeq ($(USEMALLOCLIB),on)
    MALLOC_I = -I/usr/monash/contrib/include/dbmalloc
    MALLOC_L = -L/usr/monash/contrib/lib -ldbmalloc
  else
    MALLOC_I =
    MALLOC_L =
  endif
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#Ultrix Defines
ifeq ($(OS),ultrix)
  OBJ=o
  CC = gcc
  CFLAGS = -ggdb3 -Wall $(MALLOC_I)
  LOAD = -o
  LDFLAGS = $(MALLOC_L) -L. -ldiamond -lg++
  DEP =
  FLEXFIX =
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = ranlib
  INSTALLBIN = install -m 755 -s -c
  INSTALL = install -m 644 -c
  LIBNAME = libdiamond.a
  MULTILIB = libdbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = cp y.tab.c dsc.cc
  ifeq ($(USEMALLOCLIB),on)
    MALLOC_I = -I/usr/monash/contrib/include/dbmalloc
    MALLOC_L = -L/usr/monash/contrib/lib -ldbmalloc
  else
    MALLOC_I =
    MALLOC_L =
  endif
  RM = rm
  RMFLAGS = -f
  CP = cp
  MKDIR = mkdir
endif

#OS/2 Defines
ifeq ($(OS),os2-bc)
  OBJ=obj
  CC = bcc
  CFLAGS = +Diamond.cfg
  DEP = Diamond.cfg
  LOAD = -e
  LDFLAGS = diamond.lib
  STRIPHEAD = move y.tab.c dsc.cc
  FLEXFIX = move lexyy.c lex.yy.c
  PARSERGEN = yacc
  LIBRARIAN = tlib
  RANLIB = dir
  INSTALLBIN = copy
  INSTALL = copy
  LIBNAME = diamond.lib
  LIBOBJ := $(OBJS:%=%.$(OBJ))
  LIBOBJS := $(LIBOBJ:%=+%)
  RM = del
  RMFLAGS =
  CP = copy
  MKDIR = mkdir
endif

#OS/2 emx/gcc Defines
ifeq ($(OS),os2-emx)
  OBJ=o
  CC = gcc
  CFLAGS = -O2 -s -Wall
  LOAD = -o \

  LDFLAGS = -Zexe -L. -ldiamond -lgpp -liostream -ldiamond
  DEP =
  FLEXFIX = move lexyy.c lex.yy.c
  PARSERGEN = yacc
  LIBRARIAN = ar rcv
  RANLIB = dir
  INSTALLBIN = copy
  INSTALL = copy
  LIBNAME = diamond.a
  MULTILIB = dbmulti.a
  LIBOBJ := $(OBJS:%=%.o)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = copy y.tab.c dsc.cc
  RM = del
  RMFLAGS =
  CP = copy
  MKDIR = mkdir
endif

#OS/2 emx/gcc Defines
ifeq ($(OS),os2-emx-syscalls)
  OBJ=obj
  CC = gcc
  CFLAGS = -O2 -s -Wall -Zomf -Zsys
  LOAD = -o \

  LDFLAGS = -Zexe -L. -ldiamonds -lgpp -liostream -ldiamonds
  DEP =
  FLEXFIX = move lexyy.c lex.yy.c
  PARSERGEN = yacc
  LIBRARIAN = emxomfar rcv
  RANLIB = dir
  INSTALLBIN = copy
  INSTALL = copy
  LIBNAME = diamonds.lib
  MULTILIB = dbmulti.lib
  LIBOBJ := $(OBJS:%=%.obj)
  LIBOBJS = $(LIBOBJ)
  STRIPHEAD = copy y.tab.c dsc.cc
  RM = del
  RMFLAGS =
  CP = copy
  MKDIR = mkdir
endif


.SUFFIXES:
.SUFFIXES: .$(OBJ) .y,v .l,v .y .l .cc,v .cc .h,v .h


#####################
# RCS stuff for OS/2.

ifeq ($(OS),os2)

.path.cc,v = ./RCS
.path.h,v = ./RCS
.path.l,v = ./RCS
.path.y,v = ./RCS

%.l : %.l,v
	@if not exist $&.l co $&.l

%.y : %.y,v
	@if not exist $&.y co $&.y

%.h : %.h,v
	@if not exist $&.h co $&.h

%.cc : %.cc,v
	@if not exist $&.cc co $&.cc
endif

%.$(OBJ) : %.cc
	$(CC) -I. -c $(CFLAGS) $(DFLAGS) $<

% : %.$(OBJ)
	$(CC) $< $(LOAD)$@ $(LDFLAGS)

######################################################
# Targets start here
#
# Do not change anything under this comment.
#
######################################################

all: lib $(BINARIES)

install: all
	-$(MKDIR) $(BINDIR)
	-$(MKDIR) $(LIBDIR)
	-$(MKDIR) $(INCDIR)
	$(INSTALLBIN) $(BINARIES) $(BINDIR)/dsc
	$(INSTALL) $(LIBNAME) $(LIBDIR)/$(LIBNAME)
	$(CP) $(RELATIONHEAD) $(INCDIR)

installmulti: multi
	-$(MKDIR) $(LIBDIR)
	-$(MKDIR) $(INCDIR)
	$(INSTALL) $(MULTILIB) $(LIBDIR)/$(MULTILIB)
	$(CP) $(INSTALLMULTIHEAD) $(INCDIR)

depend: $(ALLSRC) $(ALLHEAD) dsc.y
	@echo -n "Backing up Makefile, "
	@mv -f Makefile Makefile.bak
	@awk '/^\#Do not remove/{n = 1;print $0} n==0' Makefile.bak >Makefile
	@echo "now making dependencies"
	@sh -c 'for a in $(ALLSRC) $(MULTISRC); do gcc -I. -M $$a; done' | sed -e 's/\.o/.$$(OBJ)/;s| /[^ ]*||g;/^ *\\$$/d' >> Makefile

clean:
	rm -f lex.yy.c y.tab.c dsc.cc *.$(OBJ) $(MULTILIB) $(LIBNAME) dsc $(XTRACLEAN) version
	rcsclean RCS/*

trans: db2txt

db2txt: db2txt.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)db2txt db2txt.$(OBJ) $(LDFLAGS)

lib: $(LIBNAME)

multi: $(MULTILIB) jeweler

$(MULTILIB): dclient.$(OBJ)
	-$(RM) $(RMFLAGS) $(MULTILIB)
	$(LIBRARIAN) $(MULTILIB) dclient.$(OBJ)
	$(RANLIB) $(MULTILIB)

$(LIBNAME): $(LIBOBJ)
	-$(RM) $(RMFLAGS) $(LIBNAME)
	$(LIBRARIAN) $(LIBNAME) $(LIBOBJS)
	$(RANLIB) $(LIBNAME)

lex.yy.c: dsc.l
	flex dsc.l
	$(FLEXFIX)

dsc.cc: lex.yy.c dsc.y
	-$(RM) $(RMFLAGS) dsc.cc
	$(PARSERGEN) dsc.y
	$(STRIPHEAD)

dsc: lex.yy.c dsc.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)dsc dsc.$(OBJ) $(LDFLAGS)

jeweler: gib.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)jeweler gib.$(OBJ) $(LDFLAGS)

version: version.$(OBJ) $(LIBNAME) ns.dbc
	$(CC) $(CFLAGS) $(LOAD)version version.$(OBJ) $(LDFLAGS)

dispr: dispr.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)dispr dispr.$(OBJ) $(LDFLAGS)

dist: $(DIST)
	(cd ..; tar cf - $(DISTFILES) | compress > diamond.tar.Z)

######################
# Test suite....
######################

dbotest: $(LIBNAME)
	dsc $(TESTDBFILES) $(TESTSCHEMA) test
	$(CC) $(CFLAGS) $(LOAD)dbotest test.cc dbotest.cc $(LDFLAGS)

tester: tester.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)tester tester.$(OBJ) $(LDFLAGS)

btdump: btdump.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)btdump btdump.$(OBJ) $(LDFLAGS)

RTEST_O=rtest.$(OBJ) rand.$(OBJ) $(LIBNAME)

rtest: $(RTEST_O)
	$(CC) $(CFLAGS) $(LOAD)rtest $(CFLAGS) $(RTEST_O) $(LDFLAGS) -lm

test1: test1.$(OBJ) myobj.$(OBJ) $(LIBNAME)
	$(CC) $(CFLAGS) $(LOAD)test1 test1.$(OBJ) $(LDFLAGS)


#########################
# Automatic dependancies.
#########################

dsc.$(OBJ) : dsc.cc idxinfo.h d_types.h rserv.h defs.h btree.h object.h generate.h cache.h dberr.h mserv.h

#Do not remove this line or I will be very very angry (and broken).
btree.$(OBJ) : btree.cc \
  ./btree.h ./defs.h ./dberr.h \
  ./rserv.h ./cache.h ./object.h ./dbstr.h ./mystring.h
bucket.$(OBJ) : bucket.cc ./defs.h ./dberr.h \
  ./object.h ./dbstr.h ./mystring.h \

d_types.$(OBJ) : d_types.cc ./d_types.h ./dbstr.h ./mystring.h \
  ./defs.h ./dberr.h
dbase.$(OBJ) : dbase.cc \
  ./dbase.h ./nserver.h \
  ./defs.h ./dberr.h ./dbobj.h ./idxinfo.h \
  ./rserv.h ./cache.h ./btree.h ./object.h ./dbstr.h ./mystring.h ./mserv.h \
  ./diarel.h \
  ./d_types.h ./version.h
dbobj.$(OBJ) : dbobj.cc \
  ./dbobj.h ./idxinfo.h \
  ./rserv.h ./defs.h ./dberr.h ./cache.h \
  ./btree.h ./object.h ./dbstr.h ./mystring.h \
  ./mserv.h \
  ./d_types.h
diarel.$(OBJ) : diarel.cc ./diarel.h ./object.h ./defs.h ./dberr.h \
  ./dbstr.h \
  ./mystring.h \
  ./dbase.h ./nserver.h \
  ./dbobj.h ./idxinfo.h \
  ./rserv.h ./cache.h ./btree.h ./mserv.h \
  ./d_types.h
dispr.$(OBJ) : dispr.cc \
  idxinfo.h \
  d_types.h ./dbstr.h ./mystring.h \
  ./defs.h ./dberr.h
generate.$(OBJ) : generate.cc \
  ./defs.h ./dberr.h ./utils.h ./idxinfo.h ./d_types.h ./dbstr.h ./mystring.h
idxinfo.$(OBJ) : idxinfo.cc \
  ./idxinfo.h \

main.$(OBJ) : main.cc myobj.h object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \

myobj.$(OBJ) : myobj.cc myobj.h object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \

nserver.$(OBJ) : nserver.cc \
  ./nserver.h \
  ./defs.h ./dberr.h \

rand.$(OBJ) : rand.cc rand.h \

rserv.$(OBJ) : rserv.cc \
  ./rserv.h ./defs.h ./dberr.h \
  ./cache.h
rtest.$(OBJ) : rtest.cc \
  rserv.h ./defs.h ./dberr.h \
  ./cache.h rand.h
search.$(OBJ) : search.cc
test1.$(OBJ) : test1.cc myobj.h object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \
  rserv.h ./cache.h
tester.$(OBJ) : tester.cc myobj.h object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \
  rserv.h ./cache.h
dberr.$(OBJ) : dberr.cc \
  ./dberr.h
cache.$(OBJ) : cache.cc \
  ./cache.h ./defs.h ./dberr.h \

btdump.$(OBJ) : btdump.cc \
  btree.h ./defs.h ./dberr.h \
  ./rserv.h \
  ./cache.h ./object.h ./dbstr.h ./mystring.h \

version.$(OBJ) : version.cc dbase.h ./nserver.h \
  ./defs.h \
  ./dberr.h ./dbobj.h ./idxinfo.h \
  ./rserv.h ./cache.h ./btree.h ./object.h \
  ./dbstr.h ./mystring.h \
  ./mserv.h \

mystring.$(OBJ) : mystring.cc \
  ./mystring.h \
  ./defs.h ./dberr.h
mserv.$(OBJ) : mserv.cc ./mserv.h \
  ./mystring.h \
  ./defs.h ./dberr.h
diagrel.$(OBJ) : diagrel.cc \
  ./diagrel.h ./diarel.h ./object.h ./defs.h ./dberr.h ./dbstr.h \
  ./mystring.h \
  ./dbase.h ./nserver.h \
  ./dbobj.h ./idxinfo.h \
  ./rserv.h ./cache.h ./btree.h ./mserv.h \
  ./d_types.h ./ordcomp.h
db2txt.$(OBJ) : db2txt.cc \
  ./diagrel.h ./diarel.h ./object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \
  ./dbase.h ./nserver.h \
  ./dbobj.h ./idxinfo.h \
  ./rserv.h ./cache.h ./btree.h ./mserv.h \
  ./d_types.h \
  ./generate.h
utils.$(OBJ) : utils.cc \

gib.$(OBJ) : gib.cc gib.h diagrel.h ./diarel.h ./object.h ./defs.h ./dberr.h \
  ./dbstr.h ./mystring.h \
  ./dbase.h ./nserver.h \
  ./dbobj.h ./idxinfo.h ./rserv.h ./cache.h \
  ./btree.h ./mserv.h \
  ./d_types.h \
  dcdefs.h \
  ipcproto.h \

dclient.$(OBJ) : dclient.cc \
  ipcproto.h \
  dclient.h defs.h ./dberr.h \
  dcdefs.h object.h ./defs.h ./dbstr.h \
  ./mystring.h
