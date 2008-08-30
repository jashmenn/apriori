#-----------------------------------------------------------------------
# File    : makefile    (directory: util)
# Contents: build utility modules
# Author  : Christian Borgelt
# History : 2000.11.04 file created from makefile in table directory
#           2003.06.05 module params added
#           2003.08.12 module nstats added
#           2004.12.10 module memsys added
#-----------------------------------------------------------------------
CC      = gcc
CFBASE  = -ansi -Wall -pedantic $(ADDFLAGS)
CFLAGS  = $(CFBASE) -DNDEBUG -O3
# CFLAGS  = $(CFBASE) -g
# CFLAGS  = $(CFBASE) -g -DSTORAGE $(ADDINC)
# ADDINC  = -I../../misc/src
INC      = -I. -I$(TABLEDIR)

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:        memsys.o vecops.o listops.o symtab.o nimap.o \
            tabscan.o scform.o scan.o parse.o params.o

#-----------------------------------------------------------------------
# Memory Management System for Equally Sized Objects
#-----------------------------------------------------------------------
memsys.o:   memsys.h
memsys.o:   memsys.c makefile
	$(CC) $(CFLAGS) -c memsys.c -o $@

#-----------------------------------------------------------------------
# Vector Operations
#-----------------------------------------------------------------------
vecops.o:   vecops.h
vecops.o:   vecops.c makefile
	$(CC) $(CFLAGS) -c vecops.c -o $@

#-----------------------------------------------------------------------
# List Operations
#-----------------------------------------------------------------------
listops.o:  listops.h
listops.o:  listops.c makefile
	$(CC) $(CFLAGS) -c listops.c -o $@

#-----------------------------------------------------------------------
# Symbol Table Management
#-----------------------------------------------------------------------
symtab.o:   symtab.h
symtab.o:   symtab.c makefile
	$(CC) $(CFLAGS) -c symtab.c -o $@

nimap.o:    symtab.h vecops.h
nimap.o:    symtab.c makefile
	$(CC) $(CFLAGS) -DNIMAPFN -c symtab.c -o $@

#-----------------------------------------------------------------------
# Numerical Statistics Management
#-----------------------------------------------------------------------
nstats.o:   nstats.h
nstats.o:   nstats.c makefile
	$(CC) $(CFLAGS) -c nstats.c -o $@

nst_pars.o: nstats.h
nst_pars.o: nstats.c makefile
	$(CC) $(CFLAGS) -DNST_PARSE -c nstats.c -o $@

#-----------------------------------------------------------------------
# Table Scanner Management
#-----------------------------------------------------------------------
tabscan.o:  tabscan.h
tabscan.o:  tabscan.c makefile
	$(CC) $(CFLAGS) -c tabscan.c -o $@

#-----------------------------------------------------------------------
# Scanner
#-----------------------------------------------------------------------
scform.o:   scan.h
scform.o:   scan.c makefile
	$(CC) $(CFLAGS) -c scan.c -o $@

scan.o:     scan.h
scan.o:     scan.c makefile
	$(CC) $(CFLAGS) -DSC_SCAN -c scan.c -o $@

#-----------------------------------------------------------------------
# Parser Utilities
#-----------------------------------------------------------------------
parse.o:    parse.h
parse.o:    parse.c makefile
	$(CC) $(CFLAGS) -c parse.c -o $@

#-----------------------------------------------------------------------
# Command Line Parameter Retrieval
#-----------------------------------------------------------------------
params.o:   params.h
params.o:   params.c makefile
	$(CC) $(CFLAGS) -c params.c -o $@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	rm -f *.o *~ *.flc core
