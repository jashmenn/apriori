/*----------------------------------------------------------------------
  File    : chi2.c
  Contents: chi^2 distribution function
  Author  : Christian Borgelt
  History : 2003.05.19 file created as quantile.c
            2008.03.14 main programs added
----------------------------------------------------------------------*/
#if defined(CHI2PDF_MAIN) \
 || defined(CHI2CDF_MAIN) \
 || defined(CHI2QTL_MAIN)
#include <stdio.h>
#include <stdlib.h>
#endif
#ifdef CHI2QTL_MAIN
#ifndef CHI2QTL
#define CHI2QTL
#endif
#ifndef GAMMAQTL
#define GAMMAQTL
#endif
#endif
#include <assert.h>
#include <math.h>
#include "gamma.h"
#ifdef CHI2QTL
#include "normal.h"
#endif
#include "chi2.h"

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

double chi2pdf (double x, double df)
{                               /* --- probability density function */
  assert(df > 0);               /* check the function arguments */
  if (x <=  0) return 0;        /* only non-zero for positive arg. */
  if (df == 2) return 0.5 *exp(-0.5*x);
  df *= 0.5;                    /* compute probability density */
  return 0.5 *exp((df-1) *log(0.5*x) -0.5*x -logGamma(df));
}  /* chi2pdf() */

/*--------------------------------------------------------------------*/

double chi2cdfP (double x, double df)
{                               /* --- cumulative distribution fn. */
  assert(df > 0);               /* check the function arguments */
  return GammaP(0.5*df, 0.5*x); /* compute regularized Gamma function */
}  /* chi2cdfP() */

/*--------------------------------------------------------------------*/

double chi2cdfQ (double x, double df)
{                               /* --- cumulative distribution fn. */
  assert(df > 0);               /* check the function arguments */
  return GammaQ(0.5*df, 0.5*x); /* compute regularized Gamma function */
}  /* chi2cdfQ() */

/*--------------------------------------------------------------------*/
#ifdef CHI2QTL

double chi2qtlP (double prob, double df)
{                               /* --- quantile of chi2 distribution */
  return GammaqtlP(prob, 0.5*df, 2);
}  /* chi2qtlP() */

/*--------------------------------------------------------------------*/

double chi2qtlQ (double prob, double df)
{                               /* --- quantile of chi2 distribution */
  return GammaqtlQ(prob, 0.5*df, 2);
}  /* chi2qtlQ() */

#endif
/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/
#ifdef CHI2PDF_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double df = 1;                /* degrees of freedom */
  double x;                     /* argument value */

  if ((argc < 2) || (argc > 3)){/* if wrong number of arguments */
    printf("usage: %s arg [df]\n", argv[0]);
    printf("compute probability density function of the\n"
           "chi^2 distribution with df degrees of freedom\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  x = atof(argv[1]);            /* get the argument value */
  if (argc > 2) df = atof(argv[2]);
  if (df <= 0) {                /* get the degrees of freedom */
    printf("%s: invalid degrees of freedom\n", argv[0]); return -1; }
  printf("chi^2: f(%.16g; %.16g) = %.16g\n", x, df, chi2pdf(x, df));
  return 0;                     /* compute and print density */
}  /* main() */

#endif
/*--------------------------------------------------------------------*/
#ifdef CHI2CDF_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double df = 1;                /* degrees of freedom */
  double x;                     /* argument value */

  if ((argc < 2) || (argc > 3)){/* if wrong number of arguments */
    printf("usage: %s arg [df]\n", argv[0]);
    printf("compute cumulative distribution function of the\n"
           "chi^2 distribution with df degrees of freedom\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  x = atof(argv[1]);            /* get the argument value */
  if (argc > 2) df = atof(argv[2]);
  if (df <= 0) {                /* get the degrees of freedom */
    printf("%s: invalid degrees of freedom\n", argv[0]); return -1; }
  printf("chi^2: F(%.16g; %.16g) = %.16g\n", x, df, chi2cdfP(x, df));
  printf("   1 - F(%.16g; %.16g) = %.16g\n", x, df, chi2cdfQ(x, df));
  return 0;                     /* compute and print probability */
}  /* main() */

#endif
/*--------------------------------------------------------------------*/
#ifdef CHI2QTL_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double df = 1;                /* degrees of freedom */
  double prob;                  /* probability */

  if ((argc < 2) || (argc > 3)){/* if wrong number of arguments */
    printf("usage: %s prob [df]\n", argv[0]);
    printf("compute quantile of the chi^2 distribution "
           "with df degrees of freedom\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  prob = atof(argv[1]);         /* get the probability */
  if ((prob < 0) || (prob > 1)) {
    printf("%s: invalid probability\n", argv[0]); return -1; }
  if (argc > 2) df = atof(argv[2]);
  if (df <= 0) {                /* get the degrees of freedom */
    printf("%s: invalid degrees of freedom\n", argv[0]); return -1; }
  printf("chi^2: F(%.16g; %.16g) = %.16g\n",
         chi2qtlP(prob, df), df, prob);
  printf("   1 - F(%.16g; %.16g) = %.16g\n",
         chi2qtlQ(prob, df), df, prob);
  return 0;                     /* compute and print quantile */
}  /* main() */

#endif
