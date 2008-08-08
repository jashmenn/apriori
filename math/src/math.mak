#-----------------------------------------------------------------------
# File    : math.mak
# Contents: build math utility modules
# Author  : Christian Borgelt
# History : 26.01.2003 file created
#           19.05.2003 intexp, choose, zeta, quantile, and normd added
#           15.08.2003 module radfn added
#-----------------------------------------------------------------------
CC      = cl.exe
LD      = link.exe
DEFS    = /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"
CFLAGS  = /nologo /W3 /EHsc /O2 $(DEFS) /FD /c /D _CRT_SECURE_NO_DEPRECATE

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:        intexp.obj choose.obj gamma.obj zeta.obj quantile.obj \
            normd.obj radfn.obj

#-----------------------------------------------------------------------
# Gamma Function
#-----------------------------------------------------------------------
intexp.obj:   intexp.h intexp.c math.mak
	$(CC) $(CFLAGS) intexp.c /Fo$@

choose.obj:   choose.h choose.c math.mak
	$(CC) $(CFLAGS) choose.c /Fo$@

gamma.obj:    gamma.h gamma.c math.mak
	$(CC) $(CFLAGS) gamma.c /Fo$@

zeta.obj:     zeta.h zeta.c math.mak
	$(CC) $(CFLAGS) zeta.c /Fo$@

quantile.obj: quantile.h quantile.c math.mak
	$(CC) $(CFLAGS) quantile.c /Fo$@

normd.obj:    normd.h normd.c math.mak
	$(CC) $(CFLAGS) normd.c /Fo$@

radfn.obj:    radfn.h radfn.c math.mak
	$(CC) $(CFLAGS) radfn.c /Fo$@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	-@erase /Q *~ *.obj *.idb *.pch
