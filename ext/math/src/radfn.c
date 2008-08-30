/*----------------------------------------------------------------------
  File    : radfn.c
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
#include <math.h>
#include <assert.h>
#include "gamma.h"
#include "radfn.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define	M_PI         3.14159265358979323846  /* \pi */
#define MINDENOM     1e-12      /* minimal value of denominator */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

double rf_cauchy (double d2, const double *a)
{                               /* --- (generalized) Cauchy function */
  double ma;                    /* temporary buffer for m/a */

  assert(a);                    /* check the function arguments */
  if (d2 < 0) {                 /* if to the get normalization factor */
    if ((a[0] <= -d2) || (a[1] <= 0))
      return 0;                 /* check whether the integral exists */
    ma = -d2 /a[0]; d2 *= -0.5; /* m/a and m/2 (m = number of dims.) */
    return (a[0] *Gamma(d2) *sin(ma *M_PI))
         / (2 *pow(M_PI, d2+1) *pow(a[1], ma-1));
  }                             /* return the normalization factor */
  if (a[0] != 2)                /* raise distance to the given power */
    d2 = pow(d2, 0.5*a[0]);     /* (note that d2 is already squared) */
  d2 += a[1];                   /* add offset to distance */
  return (d2 > MINDENOM) ? 1/d2 : 1/MINDENOM;
}  /* rf_cauchy() */            /* compute Cauchy function */

/*----------------------------------------------------------------------
f_cauchy(d) = 1/(d^{a_0} +a_1), default: 1/d^2
For retrieving the normalization factor, it must be
d2 = -(number of dimensions of the data space).
----------------------------------------------------------------------*/

double rf_gauss (double d2, const double *a)
{                               /* --- (general.) Gaussian function */
  double ma;                    /* temporary buffer for m/a */

  if (d2 < 0) {                 /* if to the get normalization factor */
    if (a[0] <= 0) return 0;    /* check whether the integral exists */
    if (a[0] == 2)              /* use simplified formula for a = 2 */
      return pow(2*M_PI, 0.5*d2);
    ma = -d2 /a[0]; d2 *= -0.5; /* m/a and m/2 (m = number of dims.) */
    return (a[0] *Gamma(d2)) / (pow(2, ma+1) *pow(M_PI, d2) *Gamma(ma));
  }                             /* return the normalization factor */
  if (a[0] != 2)                /* raise distance to the given power */
    d2 = pow(d2, 0.5*a[0]);     /* (note that d2 is already squared) */
  return exp(-0.5 *d2);         /* compute Gaussian function */
}  /* rf_gauss() */

/*----------------------------------------------------------------------
f_gauss(d) = exp(-0.5 *d^{a_0}), default: exp(-0.5 *d^2)
For retrieving the normalization factor, it must be
d2 = -(number of dimensions of the data space).
----------------------------------------------------------------------*/

double rfi_cauchy (double d2, const double *a)
{                               /* --- inverse Cauchy function */
  assert(a);                    /* check the function arguments */
  if (a[0] != 2)                /* raise distance to the given power */
    d2 = pow(d2, 0.5*a[0]);     /* (note that d2 is already squared) */
  return d2 +a[1];              /* add offset to distance */
}  /* rfi_cauchy() */

/*--------------------------------------------------------------------*/

double rfi_gauss (double d2, const double *a)
{                               /* --- (general.) Gaussian function */
  if (a[0] != 2)                /* raise distance to the given power */
    d2 = pow(d2, 0.5*a[0]);     /* (note that d2 is already squared) */
  return exp(0.5 *d2);          /* compute Gaussian function */
}  /* rfi_gauss() */

/*--------------------------------------------------------------------*/

double rfd_cauchy (double d2, const double *a, double f)
{                               /* --- derivative of Cauchy function */
  assert(a && (d2 >= 0));       /* check the function arguments */
  if (d2   <= 0) return 0;      /* check for a positive distance */
  if (f    <  0) f = rf_cauchy(d2, a);  /* get the function value */
  if (a[0] == 2) return -f*f;           /* and check special cases */
  if (a[0] == 4) return -2 *f*f *sqrt(d2);
  return -0.5 *a[0] *f*f *pow(d2, 0.25 *a[0] -0.5);
}  /* rfd_cauchy() */            /* return the derivative */

/*--------------------------------------------------------------------*/

double rfd_gauss (double d2, const double *a, double f)
{                               /* --- derivative of Gaussian func. */
  assert(a && (d2 >= 0));       /* check the function arguments */
  if (d2   <= 0) return 0;      /* check for a positive distance */
  if (f    <  0) f = rf_gauss(d2, a);   /* get the function value */
  if (a[0] == 2) return -0.5 *f;        /* and check special cases */
  if (a[0] == 4) return -f *sqrt(d2);
  return -0.25 *a[0] *f *pow(d2, 0.25 *a[0] -0.5);
}  /* rfd_gauss() */            /* return the derivative */
