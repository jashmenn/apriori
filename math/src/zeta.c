/*----------------------------------------------------------------------
  File    : zeta.c
  Contents: compute Riemann's zeta-function
  Author  : Christian Borgelt
  History : 1998.10.20 file created
            2008.03.14 main function added
----------------------------------------------------------------------*/
#ifdef ZETA_MAIN
#include <stdio.h>
#include <stdlib.h>
#endif
#include <math.h>
#include "zeta.h"

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

double zeta (double x)
{                               /* --- compute Riemann's zeta-function*/
  double t = 1, z = 0;          /* term to add and result */
  double base = 2;              /* base for terms */

  do {                          /* compute the sum */
    z += t;                     /* zeta(x) = \sum_{n=1}^\infty n^{-x} */
    t = pow(base++, -x);        /* by successively adding terms */
  } while (z +t > z);           /* until a terms gets zero */
  return z;                     /* return the function value */
}  /* zeta() */

/*--------------------------------------------------------------------*/
#ifdef ZETA_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  double x;                     /* argument of the zeta function */

  if (argc != 2) {              /* if wrong number of arguments given */
    printf("usage: %s arg\n", argv[0]);
    printf("compute Riemann's zeta-function for arg\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  x = atof(argv[1]);            /* get argument */
  if (x <= 1) { printf("%s: x must be > 1\n", argv[0]); return -1; }
  printf("%.16g\n", zeta(x));   /* compute zeta function */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
