/*----------------------------------------------------------------------
  File    : normal.h
  Contents: normal distribution functions
  Author  : Christian Borgelt
  History : 2002.11.04 file created
            2008.03.14 set of functions extended
            2008.03.15 cumulative distribution function added
----------------------------------------------------------------------*/
#ifndef __NORMAL__
#define __NORMAL__

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern double unitpdf  (double x);
extern double unitcdf  (double x);
extern double unitcdfP (double x);
extern double unitcdfQ (double x);
extern double unitqtl  (double prob);
extern double unitqtlP (double prob);
extern double unitqtlQ (double prob);
extern double unitrand (double randfn (void));

extern double normpdf  (double x,            double mu, double sigma2);
extern double normcdf  (double x,            double mu, double sigma2);
extern double normcdfP (double x,            double mu, double sigma2);
extern double normcdfQ (double x,            double mu, double sigma2);
extern double normqtl  (double prob,         double mu, double sigma2);
extern double normqtlP (double prob,         double mu, double sigma2);
extern double normqtlQ (double prob,         double mu, double sigma2);
extern double normrand (double randfn(void), double mu, double sigma2);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define unitcdf(x)     unitcdfP(x)
#define unitcdfQ(x)    unitcdfP(-(x))
#define unitqtl(p)     unitqtlP(p)
#define unitqtlQ(p)    (-unitqtlP(p))

#define normcdf(x)     normcdfP(x)
#define normqtl(p)     normqtlP(p)

#endif
