/*----------------------------------------------------------------------
  File    : normal.c
  Contents: normal distribution functions
  Author  : Christian Borgelt
  History : 2002.11.04 file created
            2008.03.14 quantile function, main function added
            2008.03.15 cumulative distribution function added
----------------------------------------------------------------------*/
#if defined(NORMPDF_MAIN) \
 || defined(NORMCDF_MAIN) \
 || defined(NORMQTL_MAIN)
#include <stdio.h>
#include <stdlib.h>
#endif
#include <float.h>
#include <assert.h>
#include <math.h>
#include "normal.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define SQRT_2      1.41421356237309504880168872421  /* \sqrt(2) */
#define _1_SQRT_2PI 0.39894228040143267793994605993  /* 1/\sqrt(2\pi) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

double unitpdf (double x)
{                               /* --- probability density function */
  return exp(-0.5*x*x) *_1_SQRT_2PI;
}  /* unitpdf() */

/*--------------------------------------------------------------------*/

double unitcdfP (double x)
{                               /* --- cumulative distribution fn. */
  double y, z, u;               /* square, absolute value of x */

  if (x >   8.572) return 1.0;  /* if outside proper interval, */
  if (x < -37.519) return 0.0;  /* return the limiting values */
  z = fabs(x);                  /* exploit the symmetry */
  if (z < 0.5*2.2204460492503131e-16)
    return 0.5;                 /* treat 0 as a special case */
  if (z < 0.66291) {            /* if fairly close to zero */
    y = x*x;                    /* compute the square of x */
    z = ((((     0.065682337918207449113
        *y +     2.2352520354606839287)
        *y +   161.02823106855587881)
        *y +  1067.6894854603709582)
        *y + 18154.981253343561249)
      / ((((
         y +    47.20258190468824187)
        *y +   976.09855173777669322)
        *y + 10260.932208618978205)
        *y + 45507.789335026729956);
    return 0.5 +x *z;           /* compute with rational function */
  }                             
  if (z < 4*SQRT_2) {           /* if medium value */
    u = (((((((( 1.0765576773720192317e-8
        *z +     0.39894151208813466764)
        *z +     8.8831497943883759412)
        *z +    93.506656132177855979)
        *z +   597.27027639480026226)
        *z +  2494.5375852903726711)
        *z +  6848.1904505362823326)
        *z + 11602.651437647350124)
        *z +  9842.7148383839780218)
      / ((((((((
         z +    22.266688044328115691)
        *z +   235.38790178262499861)
        *z +  1519.377599407554805)
        *z +  6485.558298266760755)
        *z + 18615.571640885098091)
        *z + 34900.952721145977266)
        *z + 38912.003286093271411)
        *z + 19685.429676859990727);
    z = u *exp(-0.5*x*x); }     /* compute with rational function */
  else {                        /* if tail value */
    y = 1 /(x*x);               /* compute the inverse square of x */
    u = (((((    0.02307344176494017303
        *y +     0.21589853405795699)
        *y +     0.1274011611602473639)
        *y +     0.022235277870649807)
        *y +     0.001421619193227893466)
        *y +     2.9112874951168792e-5)
      / (((((
         y +     1.28426009614491121)
        *y +     0.468238212480865118)
        *y +     0.0659881378689285515)
        *y +     0.00378239633202758244)
        *y +     7.29751555083966205e-5);
    z = (((_1_SQRT_2PI) -y*u) /z) *exp(-0.5*x*x);
  }                             /* compute with rational function */
  return (x > 0) ? 1-z : z;     /* exploit the symmetry */
}  /* unitcdfP() */

/*----------------------------------------------------------------------
References: - W.J. Cody.
              Rational Chebyshev Approximations for the Error Function.
              Mathematics of Computation, 23(107):631-637, 1969
            - W. Fraser and J.F Hart.
              On the Computation of Rational Approximations
              to Continuous Functions.
              Communications of the ACM 5, 1962
            - W.J. Kennedy Jr. and J.E. Gentle.
              Statistical Computing.
              Marcel Dekker, 1980
----------------------------------------------------------------------*/
#if 0

double unitqtlP (double prob)
{                               /* --- quantile of normal distrib. */
  double p, x;                  /*     with mean 0 and variance 1 */

  assert((prob >= 0) && (prob <= 1));  /* check the function argument */
  if (prob >= 1.0) return  DBL_MAX; /* check for limiting values */
  if (prob <= 0.0) return -DBL_MAX; /* and return extrema */
  if (prob == 0.5) return 0;    /* treat prob = 0.5 as a special case */
  p = (prob > 0.5) ? 1-prob : prob; /* transform to left tail if nec. */
  if (p < 1e-20) return (prob < 0.5) ? -10 : 10;
  x = sqrt(log(1/(p*p)));       /* compute quotient of polynomials */
  x += ((((-0.453642210148e-4   /* (rational function approximation) */
       *x  -0.0204231210245)
       *x  -0.342242088547)
       *x  -1.0)
       *x  -0.322232431088)
     / (((( 0.0038560700634
       *x  +0.103537752850)
       *x  +0.531103462366)
       *x  +0.588581570495)
       *x  +0.0993484626060);
  return (prob < 0.5) ? -x : x; /* retransform to right tail if nec. */
}  /* unitqtlP() */

#else /*--------------------------------------------------------------*/

double unitqtlP (double prob)
{                               /* --- quantile of normal distrib. */
  double p, x;                  /*     with mean 0 and variance 1 */

  assert((prob >= 0) && (prob <= 1));  /* check the function argument */
  if (prob >= 1.0) return  DBL_MAX; /* check for limiting values */
  if (prob <= 0.0) return -DBL_MAX; /* and return extrema */
  p = prob -0.5;
  if (fabs(p) <= 0.425) {       /* if not tail */
    x = 0.180625 - p*p;         /* get argument of rational function */
    x = (((((((2509.0809287301226727
        *x +  33430.575583588128105)
        *x +  67265.770927008700853)
        *x +  45921.953931549871457)
        *x +  13731.693765509461125)
        *x +   1971.5909503065514427)
        *x +    133.14166789178437745)
        *x +      3.387132872796366608)
      / (((((((5226.495278852854561
        *x +  28729.085735721942674)
        *x +  39307.89580009271061)
        *x +  21213.794301586595867)
        *x +   5394.1960214247511077)
        *x +    687.1870074920579083)
        *x +     42.313330701600911252)
        *x +      1.0);         /* evaluate the rational function */
    return p *x;                /* and return the computed value */
  }
  p = (prob > 0.5) ? 1-prob : prob;
  x = sqrt(-log(p));            /* transform to left tail if nec. */
  if (x <= 5) {                 /* if not extreme tail */
    x -= 1.6;                   /* get argument of rational function */
    x = (((((((  7.7454501427834140764e-4
        *x +     0.0227238449892691845833)
        *x +     0.24178072517745061177)
        *x +     1.27045825245236838258)
        *x +     3.64784832476320460504)
        *x +     5.7694972214606914055)
        *x +     4.6303378461565452959)
        *x +     1.42343711074968357734)
      / (((((((  1.05075007164441684324e-9
        *x +     5.475938084995344946e-4)
        *x +     0.0151986665636164571966)
        *x +     0.14810397642748007459)
        *x +     0.68976733498510000455)
        *x +     1.6763848301838038494)
        *x +     2.05319162663775882187)
        *x +     1.0); }        /* evaluate the rational function */
  else {                        /* if extreme tail */
    x -= 5;                     /* get argument of rational function */
    x = (((((((  2.01033439929228813265e-7
        *x +     2.71155556874348757815e-5)
        *x +     0.0012426609473880784386)
        *x +     0.026532189526576123093)
        *x +     0.29656057182850489123)
        *x +     1.7848265399172913358)
        *x +     5.4637849111641143699)
        *x +     6.6579046435011037772)
      / (((((((    2.04426310338993978564e-15
        *x +     1.4215117583164458887e-7)
        *x +     1.8463183175100546818e-5)
        *x +     7.868691311456132591e-4)
        *x +     0.0148753612908506148525)
        *x +     0.13692988092273580531)
        *x +     0.59983220655588793769)
        *x +     1.0);          /* evaluate the rational function */
  }
  return (prob < 0.5) ? -x : x; /* retransform to right tail if nec. */
}  /* unitqtlP() */

#endif
/*----------------------------------------------------------------------
References: - R.E. Odeh and J.O. Evans.
              The percentage points of the normal distribution.
              Applied Statistics 22:96--97, 1974
            - J.D. Beasley and S.G. Springer.
              Algorithm AS 111:
              The percentage points of the normal distribution.
              Applied Statistics 26:118--121, 1977
            - M.J. Wichura
              Algorithm AS 241:
              The percentage points of the normal distribution.
              Applied Statistics 37:477--484, 1988
----------------------------------------------------------------------*/

double unitrand (double randfn(void))
{                               /* --- compute N(0,1) distrib. number */
  static double b;              /* buffer for random number */
  double x, y, r;               /* coordinates and radius */

  if (b != 0.0) {               /* if the buffer is full, */
    x = b; b = 0; return x; }   /* return the buffered number */
  do {                          /* pick a random point */
    x = 2.0*randfn()-1.0;       /* in the unit square [-1,1]^2 */
    y = 2.0*randfn()-1.0;       /* and check whether it lies */
    r = x*x +y*y;               /* inside the unit circle */
  } while ((r > 1) || (r == 0));
  r = sqrt(-2*log(r)/r);        /* factor for Box-Muller transform */
  b = x *r;                     /* save one of the random numbers */
  return y *r;                  /* and return the other */
}  /* unitrand() */

/*----------------------------------------------------------------------
Source for the polar method to generate normally distributed numbers:
D.E. Knuth.
The Art of Computer Programming, Vol. 2: Seminumerical Algorithms
Addison-Wesley, Reading, MA, USA 1998
pp. 122-123
----------------------------------------------------------------------*/

double normpdf (double x, double mean, double var)
{                               /* --- probability density function */
  assert(var >= 0);             /* check the function arguments */
  if (var == 1) return unitpdf(x-mean);
  return unitpdf((x-mean) /sqrt(var));
}  /* normpdf() */

/*--------------------------------------------------------------------*/

double normcdfP (double x, double mean, double var)
{                               /* --- cumulative distribution fn. */
  assert(var >= 0);             /* check the function arguments */
  if (var == 1) return unitcdf(x-mean);
  return unitcdf((x-mean) /sqrt(var));
}  /* normcdfP() */

/*--------------------------------------------------------------------*/

double normcdfQ (double x, double mean, double var)
{                               /* --- cumulative distribution fn. */
  assert(var >= 0);             /* check the function arguments */
  if (var == 1) return unitcdfP(mean-x);
  return unitcdfP((mean-x) /sqrt(var));
}  /* normcdfQ() */

/*--------------------------------------------------------------------*/

double normqtlP (double prob, double mean, double var)
{                               /* --- quantile of normal distrib. */
  assert((var  >  0)            /* check the function arguments */
      && (prob >= 0) && (prob <= 1));
  if (var == 1) return mean +unitqtl(prob);
  return mean +unitqtlP(prob) *sqrt(var);
}  /* normqtlP() */

/*--------------------------------------------------------------------*/

double normqtlQ (double prob, double mean, double var)
{                               /* --- quantile of normal distrib. */
  assert((var  >  0)            /* check the function arguments */
      && (prob >= 0) && (prob <= 1));
  if (var == 1) return mean -unitqtlP(prob);
  return mean -unitqtlP(prob) *sqrt(var);
}  /* normqtlQ() */

/*--------------------------------------------------------------------*/

double normrand (double randfn(void), double mean, double var)
{                               /* --- cumulative distribution fn. */
  assert(var >= 0);             /* check the function arguments */
  if (var == 1) return mean +unitrand(randfn);
  return mean +unitrand(randfn) *sqrt(var);
}  /* normrand() */

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/
#ifdef NORMPDF_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double mean = 0;              /* mean value */
  double var  = 1;              /* variance */
  double x;                     /* argument value */

  if ((argc < 2) || (argc > 4)){/* if wrong number of arguments */
    printf("usage: %s arg [mean variance]\n", argv[0]);
    printf("compute probability density function "
           "of the normal distribution\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  x = atof(argv[1]);            /* get the argument value */
  if (argc > 2) mean = atof(argv[2]);
  if (argc > 3) var  = atof(argv[3]);
  if (var  < 0) {               /* get the parameters */
    printf("%s: invalid variance\n", argv[0]); return -1; }
  printf("normal: f(%.16g; %.16g, %.16g) = %.16g\n",
         x, mean, var, normpdf(x, mean, var));
  return 0;                     /* compute and print density */
}  /* main() */

#endif
/*--------------------------------------------------------------------*/
#ifdef NORMCDF_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double mean = 0;              /* mean value */
  double var  = 1;              /* variance */
  double x;                     /* argument value */

  if ((argc < 2) || (argc > 4)){/* if wrong number of arguments */
    printf("usage: %s arg [mean variance]\n", argv[0]);
    printf("compute cumulative distribution function "
           "of the normal distribution\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  x = atof(argv[1]);            /* get the argument value */
  if (argc > 2) mean = atof(argv[2]);
  if (argc > 3) var  = atof(argv[3]);
  if (var  < 0) {               /* get the parameters */
    printf("%s: invalid variance\n", argv[0]); return -1; }
  printf("normal: F(% .16g; %.16g, %.16g) = %.16g\n",
         x, mean, var, normcdfP(x, mean, var));
  printf("    1 - F(% .16g; %.16g, %.16g) = %.16g\n",
         x, mean, var, normcdfQ(x, mean, var));
  return 0;                     /* compute and print probability */
}  /* main() */

#endif
/*--------------------------------------------------------------------*/
#ifdef NORMQTL_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double mean = 0;              /* mean value */
  double var  = 1;              /* variance */
  double prob;                  /* probability */

  if ((argc < 2) || (argc > 4)){/* if wrong number of arguments */
    printf("usage: %s prob [mean variance]\n", argv[0]);
    printf("compute quantile of the normal distribution\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  prob = atof(argv[1]);         /* get the probability */
  if ((prob < 0) || (prob > 1)) {
    printf("%s: invalid probability\n", argv[0]); return -1; }
  if (argc > 2) mean = atof(argv[2]);
  if (argc > 3) var  = atof(argv[3]);
  if (var  < 0) {               /* get the parameters */
    printf("%s: invalid variance\n", argv[0]); return -1; }
  printf("normal: F(% .16g; %.16g, %.16g) = %.16g\n",
         normqtlP(prob, mean, var), mean, var, prob);
  printf("    1 - F(% .16g; %.16g, %.16g) = %.16g\n",
         normqtlQ(prob, mean, var), mean, var, prob);
  return 0;                     /* compute and print quantile */
}  /* main() */

#endif
