#-----------------------------------------------------------------------
# File    : apriori.mak
# Contents: build apriori program
# Author  : Christian Borgelt
# History : 26.01.2003 file created
#           20.07.2006 adapted to Visual Studio 8
#-----------------------------------------------------------------------
CC      = cl.exe
LD      = link.exe
DEFS    = /D WIN32 /D NDEBUG /D _CONSOLE /D _MBCS \
          /D _CRT_SECURE_NO_DEPRECATE
CFLAGS  = /nologo /W3 /EHsc /O2 /I $(UTILDIR) /I $(MATHDIR) $(DEFS) /FD /c
LDFLAGS = /nologo /subsystem:console /incremental:no /machine:X86

THISDIR = ..\..\apriori\src
UTILDIR = ..\..\util\src
MATHDIR = ..\..\math\src
HDRS    = $(UTILDIR)\vecops.h    $(UTILDIR)\symtab.h \
          $(UTILDIR)\tabscan.h   $(UTILDIR)\scan.h \
          $(MATHDIR)\gamma.h     $(MATHDIR)\chi2.o \
          tract.h istree.h
OBJS    = $(UTILDIR)\vecops.obj  $(UTILDIR)\nimap.obj \
          $(UTILDIR)\tabscan.obj $(UTILDIR)\scan.obj \
          $(MATHDIR)\gamma.obj   $(MATHDIR)\chi2.obj \
          tract.obj istree.obj apriori.obj

#-----------------------------------------------------------------------
# Build Program
#-----------------------------------------------------------------------
all:         apriori.exe

apriori.exe: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) /out:$@

#-----------------------------------------------------------------------
# Item and Transaction Management
#-----------------------------------------------------------------------
tract.obj:   $(UTILDIR)\symtab.h tract.h tract.c apriori.mak
	$(CC) $(CFLAGS) tract.c /Fo$@

#-----------------------------------------------------------------------
# Frequent Item Set Tree Management
#-----------------------------------------------------------------------
istree.obj:  $(MATHDIR)\gamma.h tract.h istree.h istree.c apriori.mak
	$(CC) $(CFLAGS) istree.c /Fo$@

#-----------------------------------------------------------------------
# Main Program
#-----------------------------------------------------------------------
apriori.obj: $(UTILDIR)\symtab.h tract.h istree.h apriori.c apriori.mak
	$(CC) $(CFLAGS) /D NIMAPFN apriori.c /Fo$@

#-----------------------------------------------------------------------
# External Modules
#-----------------------------------------------------------------------
$(UTILDIR)\vecops.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak vecops.obj
	cd $(THISDIR)
$(UTILDIR)\nimap.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak nimap.obj
	cd $(THISDIR)
$(UTILDIR)\tabscan.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak tabscan.obj
	cd $(THISDIR)
$(UTILDIR)\scan.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak scan.obj
	cd $(THISDIR)
$(MATHDIR)\gamma.obj:
	cd $(MATHDIR)
	$(MAKE) /f math.mak gamma.obj
	cd $(THISDIR)
$(MATHDIR)\chi2.obj:
	cd $(MATHDIR)
	$(MAKE) /f math.mak chi2.obj
	cd $(THISDIR)

#-----------------------------------------------------------------------
# Install
#-----------------------------------------------------------------------
install:
	-@copy apriori.exe c:\home\bin

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	$(MAKE) /f apriori.mak localclean
	cd $(UTILDIR)
	$(MAKE) /f util.mak clean
	cd $(MATHDIR)
	$(MAKE) /f math.mak clean
	cd $(THISDIR)

localclean:
	-@erase /Q *~ *.obj *.idb *.pch apriori.exe
