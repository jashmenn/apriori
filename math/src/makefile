#-----------------------------------------------------------------------
# File    : makefile    (directory: math)
# Contents: build math utility modules
# Author  : Christian Borgelt
# History : 2002.07.04 file created from makefile in util directory
#           2003.05.19 intexp, choose, zeta, quantile, and normd added
#           2003.08.15 module radfn added
#           2008.03.14 main programs added, quantile.c split/removed
#           2008.03.17 gamma distribution functions added
#-----------------------------------------------------------------------
CC      = gcc
CFBASE  = -ansi -Wall -pedantic $(ADDFLAGS)
CFLAGS  = $(CFBASE) -DNDEBUG -O3
# CFLAGS  = $(CFBASE) -g
LDFLAGS =
LIBS    = -lm

GAMMA   = gammapdf gammacdf gammaqtl
NORMAL  = normpdf  normcdf  normqtl
CHI2    = chi2pdf  chi2cdf  chi2qtl
PROGS   = $(GAMMA) $(NORMAL) $(CHI2) gamma choose zeta

#-----------------------------------------------------------------------
# Build Programs
#-----------------------------------------------------------------------
all:        $(PROGS)

gamma:      gammain.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gammain.o -o $@

choose:     chsmain.o makefile
	$(CC) $(LDFLAGS) $(LIBS) chsmain.o -o $@

zeta:       zetamain.o makefile
	$(CC) $(LDFLAGS) $(LIBS) zetamain.o -o $@

gammapdf:   gammapdf.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gammapdf.o -o $@

gammacdf:   gammacdf.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gammacdf.o -o $@

gammaqtl:   gammaqtl.o normal.o makefile
	$(CC) $(LDFLAGS) $(LIBS) normal.o gammaqtl.o -o $@

normpdf:    normpdf.o makefile
	$(CC) $(LDFLAGS) $(LIBS) normpdf.o -o $@

normcdf:    normcdf.o makefile
	$(CC) $(LDFLAGS) $(LIBS) normcdf.o -o $@

normqtl:    normqtl.o gamma.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gamma.o normqtl.o -o $@

chi2pdf:    chi2pdf.o gamma.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gamma.o chi2pdf.o -o $@

chi2cdf:    chi2cdf.o gamma.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gamma.o chi2cdf.o -o $@

chi2qtl:    chi2qtl.o gammall.o normal.o makefile
	$(CC) $(LDFLAGS) $(LIBS) gammall.o normal.o chi2qtl.o -o $@

#-----------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------
gammain.o:  gamma.h
gammain.o:  gamma.c makefile
	$(CC) $(CFLAGS) -DGAMMA_MAIN -c gamma.c -o $@

chsmain.o:  choose.h
chsmain.o:  choose.c makefile
	$(CC) $(CFLAGS) -DCHOOSE_MAIN -c choose.c -o $@

zetamain.o: zeta.h
zetamain.o: zeta.c makefile
	$(CC) $(CFLAGS) -DZETA_MAIN -c zeta.c -o $@

gammapdf.o: gamma.h
gammapdf.o: gamma.c makefile
	$(CC) $(CFLAGS) -DGAMMAPDF_MAIN -c gamma.c -o $@

gammacdf.o: gamma.h
gammacdf.o: gamma.c makefile
	$(CC) $(CFLAGS) -DGAMMACDF_MAIN -c gamma.c -o $@

gammaqtl.o: gamma.h
gammaqtl.o: gamma.c makefile
	$(CC) $(CFLAGS) -DGAMMAQTL_MAIN -c gamma.c -o $@

normpdf.o:  normal.h
normpdf.o:  normal.c makefile
	$(CC) $(CFLAGS) -DNORMPDF_MAIN -c normal.c -o $@

normcdf.o:  normal.h
normcdf.o:  normal.c makefile
	$(CC) $(CFLAGS) -DNORMCDF_MAIN -c normal.c -o $@

normqtl.o:  normal.h
normqtl.o:  normal.c makefile
	$(CC) $(CFLAGS) -DNORMQTL_MAIN -c normal.c -o $@

chi2pdf.o:  chi2.h
chi2pdf.o:  chi2.c makefile
	$(CC) $(CFLAGS) -DCHI2PDF_MAIN -c chi2.c -o $@

chi2cdf.o:  chi2.h
chi2cdf.o:  chi2.c makefile
	$(CC) $(CFLAGS) -DCHI2CDF_MAIN -c chi2.c -o $@

chi2qtl.o:  chi2.h
chi2qtl.o:  chi2.c makefile
	$(CC) $(CFLAGS) -DCHI2QTL_MAIN -c chi2.c -o $@

#-----------------------------------------------------------------------
# Mathematical Functions
#-----------------------------------------------------------------------
choose.o:   choose.h
choose.o:   choose.c makefile
	$(CC) $(CFLAGS) -c choose.c -o $@

zeta.o:     zeta.h
zeta.o:     zeta.c makefile
	$(CC) $(CFLAGS) -c zeta.c -o $@

intexp.o:   intexp.h
intexp.o:   intexp.c makefile
	$(CC) $(CFLAGS) -c intexp.c -o $@

gamma.o:    gamma.h
gamma.o:    gamma.c makefile
	$(CC) $(CFLAGS) -c gamma.c -o $@

gammall.o:  gamma.h
gammall.o:  gamma.c makefile
	$(CC) $(CFLAGS) -DGAMMAQTL -c gamma.c -o $@

normal.o:   normal.h
normal.o:   normal.c makefile
	$(CC) $(CFLAGS) -c normal.c -o $@

chi2.o:     chi2.h
chi2.o:     chi2.c makefile
	$(CC) $(CFLAGS) -c chi2.c -o $@

chi2all.o:  chi2.h gamma.h normal.h
chi2all.o:  chi2.c makefile
	$(CC) $(CFLAGS) -DCHI2QTL -c chi2.c -o $@

radfn.o:    radfn.h gamma.h
radfn.o:    radfn.c makefile
	$(CC) $(CFLAGS) -c radfn.c -o $@

#-----------------------------------------------------------------------
# Install
#-----------------------------------------------------------------------
install:
	cp $(PROGS) $(HOME)/bin

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	rm -f $(PROGS) *.o *~ *.flc core
