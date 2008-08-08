#-----------------------------------------------------------------------
# File    : makefile
# Contents: build apriori program
# Author  : Christian Borgelt
# History : ??.??.1995 file created
#           1997.10.13 macro ADDFLAGS added
#           1997.12.07 minor improvements
#           1998.01.04 table scanner management added
#           1999.11.11 vector operations module added
#           2000.11.04 modules vecops, symtab, and tabscan made external
#           2001.11.18 module tract (transaction management) added
#           2003.12.12 preprocessor definition ARCH64 added
#-----------------------------------------------------------------------
CC      = gcc
CFBASE  = -ansi -Wall -pedantic -I$(UTILDIR) -I$(MATHDIR) $(ADDFLAGS)
CFLAGS  = $(CFBASE) -DNDEBUG -O3
# CFLAGS  = $(CFBASE) -DNDEBUG -O3 -DBENCH
# CFLAGS  = $(CFBASE) -DNDEBUG -O3 -DARCH64
# CFLAGS  = $(CFBASE) -g
# CFLAGS  = $(CFBASE) -g -DARCH64
# CFLAGS  = $(CFBASE) -g -DSTORAGE $(ADDINC)
LDFLAGS =
LIBS    = -lm
# ADDINC  = -I../../misc/src
# ADDOBJ  = storage.o

UTILDIR = ../../util/src
MATHDIR = ../../math/src
HDRS    = $(UTILDIR)/vecops.h   $(UTILDIR)/symtab.h \
          $(UTILDIR)/tabscan.h  $(UTILDIR)/scan.h \
          $(MATHDIR)/gamma.h    $(MATHDIR)/chi2.h \
          tract.h istree.h
OBJS    = $(UTILDIR)/vecops.o   $(UTILDIR)/nimap.o \
          $(UTILDIR)/tabscan.o  $(UTILDIR)/scform.o \
          $(MATHDIR)/gamma.o    $(MATHDIR)/chi2.o \
          tract.o istree.o apriori.o $(ADDOBJ)

#-----------------------------------------------------------------------
# Build Program
#-----------------------------------------------------------------------
all:       apriori

apriori:   $(OBJS) makefile
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

#-----------------------------------------------------------------------
# Main Program
#-----------------------------------------------------------------------
apriori.o: tract.h istree.h $(UTILDIR)/symtab.h
apriori.o: apriori.c makefile
	$(CC) $(CFLAGS) -c apriori.c -o $@

#-----------------------------------------------------------------------
# Item and Transaction Management
#-----------------------------------------------------------------------
tract.o:   tract.h $(UTILDIR)/symtab.h
tract.o:   tract.c makefile
	$(CC) $(CFLAGS) -c tract.c -o $@

#-----------------------------------------------------------------------
# Frequent Item Set Tree Management
#-----------------------------------------------------------------------
istree.o:  istree.h tract.h $(MATHDIR)/gamma.h
istree.o:  istree.c makefile
	$(CC) $(CFLAGS) -c istree.c -o $@

#-----------------------------------------------------------------------
# External Modules
#-----------------------------------------------------------------------
$(UTILDIR)/vecops.o:
	cd $(UTILDIR); $(MAKE) vecops.o   ADDFLAGS=$(ADDFLAGS)
$(UTILDIR)/nimap.o:
	cd $(UTILDIR); $(MAKE) nimap.o    ADDFLAGS=$(ADDFLAGS)
$(UTILDIR)/tabscan.o:
	cd $(UTILDIR); $(MAKE) tabscan.o  ADDFLAGS=$(ADDFLAGS)
$(UTILDIR)/scform.o:
	cd $(UTILDIR); $(MAKE) scform.o   ADDFLAGS=$(ADDFLAGS)
$(MATHDIR)/gamma.o:
	cd $(MATHDIR); $(MAKE) gamma.o    ADDFLAGS=$(ADDFLAGS)
$(MATHDIR)/chi2.o:
	cd $(MATHDIR); $(MAKE) chi2.o     ADDFLAGS=$(ADDFLAGS)

#-----------------------------------------------------------------------
# Storage Debugging
#-----------------------------------------------------------------------
storage.o:  ../../misc/src/storage.h
storage.o:  ../../misc/src/storage.c
	$(CC) $(CFLAGS) -c ../../misc/src/storage.c -o $@

#-----------------------------------------------------------------------
# Install
#-----------------------------------------------------------------------
install:
	cp apriori $(HOME)/bin

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	rm -f *.o *~ *.flc core apriori
	cd $(UTILDIR); $(MAKE) clean
	cd $(MATHDIR); $(MAKE) clean

localclean:
	rm -f *.o *~ *.flc core apriori
