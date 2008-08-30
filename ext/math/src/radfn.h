/*----------------------------------------------------------------------
  File    : radfn.h
  Contents: radial function management
            (for clustering, radial basis function networks,
             learning vector quantization etc.)
  Author  : Christian Borgelt
  History : 15.08.2003 file created from file cluster1.c
            24.02.2004 parameterization changed
            13.08.2004 argument 'a' changed to const
            15.08.2004 functions for derivatives added
            24.01.2007 inverse function rfi_cauchy and rfi_gauss added
----------------------------------------------------------------------*/
#ifndef __RADFN__
#define __RADFN__

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef double RADFN     (double d2, const double *a);
typedef double DERIVFN   (double d2, const double *a, double f);

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern double rf_cauchy  (double d2, const double *a);
extern double rf_gauss   (double d2, const double *a);

extern double rfi_cauchy (double d2, const double *a);
extern double rfi_gauss  (double d2, const double *a);

extern double rfd_cauchy (double d2, const double *a, double f);
extern double rfd_gauss  (double d2, const double *a, double f);

#endif
