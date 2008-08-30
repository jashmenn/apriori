#-----------------------------------------------------------------------
# File    : util.mak
# Contents: build utility modules
# Author  : Christian Borgelt
# History : 2003.01.26 file created
#           2003.06.05 module params added
#           2003.08.12 module nstats added
#           2006.07.20 adapted to Visual Studio 8
#-----------------------------------------------------------------------
CC      = cl.exe
LD      = link.exe
DEFS    = /D WIN32 /D NDEBUG /D _CONSOLE /D _MBCS \
          /D _CRT_SECURE_NO_DEPRECATE
CFLAGS  = /nologo /W3 /EHsc /O2 $(DEFS) /FD /c

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:        memsys.obj vecops.obj listops.obj symtab.obj nimap.obj \
            tabscan.obj scform.obj scan.obj parse.obj params.obj

#-----------------------------------------------------------------------
# Memory Management
#-----------------------------------------------------------------------
memsys.obj:   memsys.h memsys.c util.mak
	$(CC) $(CFLAGS) memsys.c /Fo$@

#-----------------------------------------------------------------------
# Vector Operations
#-----------------------------------------------------------------------
vecops.obj:   vecops.h vecops.c util.mak
	$(CC) $(CFLAGS) vecops.c /Fo$@

#-----------------------------------------------------------------------
# List Operations
#-----------------------------------------------------------------------
listops.obj:  listops.h listops.c util.mak
	$(CC) $(CFLAGS) listops.c /Fo$@

#-----------------------------------------------------------------------
# Numerical Statistics
#-----------------------------------------------------------------------
nstats.obj:   nstats.h nstats.c util.mak
	$(CC) $(CFLAGS) nstats.c /Fo$@

nst_pars.obj: nstats.h nstats.c util.mak
	$(CC) $(CFLAGS) /D "NST_PARSE" nstats.c /Fo$@

#-----------------------------------------------------------------------
# Symbol Table Management
#-----------------------------------------------------------------------
symtab.obj:   symtab.h symtab.c util.mak
	$(CC) $(CFLAGS) symtab.c /Fo$@

nimap.obj:    symtab.h vecops.h symtab.c util.mak
	$(CC) $(CFLAGS) /D "NIMAPFN" symtab.c /Fo$@

#-----------------------------------------------------------------------
# Table Scanner Management
#-----------------------------------------------------------------------
tabscan.obj:  tabscan.h tabscan.c util.mak
	$(CC) $(CFLAGS) tabscan.c /Fo$@

#-----------------------------------------------------------------------
# Scanner
#-----------------------------------------------------------------------
scform.obj:   scan.h scan.c util.mak
	$(CC) $(CFLAGS) scan.c /Fo$@

scan.obj:     scan.h scan.c util.mak
	$(CC) $(CFLAGS) /D SC_SCAN scan.c /Fo$@

#-----------------------------------------------------------------------
# Parser Utilities
#-----------------------------------------------------------------------
parse.obj:    parse.h
parse.obj:    parse.c util.mak
	$(CC) $(CFLAGS) -c parse.c /Fo $@

#-----------------------------------------------------------------------
# Command Line Parameter Retrieval
#-----------------------------------------------------------------------
params.obj:   params.h
params.obj:   params.c util.mak
	$(CC) $(CFLAGS) -c params.c /Fo $@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	-@erase /Q *~ *.obj *.idb *.pch
