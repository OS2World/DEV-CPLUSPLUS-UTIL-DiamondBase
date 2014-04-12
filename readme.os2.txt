		       DiamondBase v0.2 for OS/2

			 Port by Jeff M. Garzik
		      (jgarzik@pantera.atl.ga.us)


Introduction
------------
DiamondBase is a relational database system you statically link into
your OS/2 programs.  All documentation for the DiamondBase system itself
is included in the manual.txt file included in the db02-bin.zip archive.


Library Installation (db02-bin.zip)
--------------------
1.  Unzip db02-bin.zip if you have not already.

2.  Put dsc.exe (the schema compiler) and db2txt.exe (database-to-text
    conversion and debugging util) in a directory where you won't forget
    where they are.

3.  Print out the manual, manual.txt, or read it completely.

4a. If you are using emx/gcc 0.8h or later, unzip db-emx.zip.  Place the
    libraries into your default library directory (typically \emx\lib).

4b. If you are using Borland C++ for OS/2, unzip db-bc.zip.  Place the
    libraries into your default library directory (typically \bcos2\lib).

5.  You are done!  Go through the manual again and compile some test
    programs, just for fun.


Source Installation / Recompiling (db02-src.zip)
---------------------------------
1.  Unzip db02-src.zip if you have not already.

2.  Obtain and install GNU make if you have not already. It is available
    via FTP from ftp-os2.cdrom.com as /os2/2_x/unix/gnumake.zip.

3.  Load makefile into your favourite editor.

4.	    If you are: 		Set line 23 in makefile to:

	Compiling with emx		OS = os2-emx
	Compiling with emx sys calls	OS = os2-emx-syscalls
	Compining with BC++/2		OS = os2-bc

5.  Save and exit the editor.

6.  Type make.	GNU Make will rebuild the library you selected.


Files
-----
db02-bin.zip - Binary and compiled-libraries archive
	db2txt.exe - database-to-text conversion util
	dsc.exe - schema compiler and database creation util
	manual.txt - DiamondBase 0.2 manual
	db-emx.zip - emx/gcc 0.8h (or later) compiled libraries
		diamond.a - standard compiler library
		diamonds.lib - system calls compiled library
		readme.emx - short file describing emx-specific stuff
	db-bc.zip - Borland C++ for OS/2 1.0 (or later) compiled libraries
		diamond.lib - borland c++ compiled library
		readme.bc - short file describing BC/2-specific stuff
db02-src.zip - Source code archive (optional)
	readme.os2 - OS/2 readme file
	BETA - source file
	btdump.cc - source file
	btree.cc - source file
	btree.h - source file
	bucket.cc - source file
	cache.cc - source file
	cache.h - source file
	CHANGES - source file
	Copyright - source file
	db2txt.cc - source file
	dbase.cc - source file
	dbase.h - source file
	dberr.cc - source file
	dberr.h - source file
	dbobj.cc - source file
	dbobj.h - source file
	dbotest.cc - source file
	dbstr.h - source file
	dcdefs.h - source file
	dclient.cc - source file
	dclient.h - source file
	defs.h - source file
	diagrel.cc - source file
	diagrel.h - source file
	Diamond.cfg - source file
	diarel.cc - source file
	diarel.h - source file
	dispr.cc - source file
	dsc.l - source file
	dsc.y - source file
	d_types.cc - source file
	d_types.h - source file
	generate.cc - source file
	generate.h - source file
	gib.cc - source file
	gib.h - source file
	idxinfo.cc - source file
	idxinfo.h - source file
	ipcproto.h - source file
	main.cc - source file
	makefile - source file
	Makefile.gen - source file
	mserv.cc - source file
	mserv.h - source file
	myobj.cc - source file
	myobj.h - source file
	mystring.cc - source file
	mystring.h - source file
	ns.dbc - source file
	nserver.cc - source file
	nserver.h - source file
	object.h - source file
	ordcomp.h - source file
	pathinfo.h - source file
	rand.cc - source file
	rand.h - source file
	README - source file
	rserv.cc - source file
	rserv.h - source file
	rtest.cc - source file
	search.cc - source file
	test1.cc - source file
	tester.cc - source file
	utils.cc - source file
	utils.h - source file
	version.cc - source file
	version.h - source file

