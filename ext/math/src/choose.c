/*----------------------------------------------------------------------
  File    : choose.c
  Contents: compute n choose k
  Author  : Christian Borgelt
  History : 1996.04.29 file created
            2003.05.17 (n-i+1) replaced by n--
            2008.03.14 main function added
----------------------------------------------------------------------*/
#ifdef CHOOSE_MAIN
#include <stdio.h>
#include <stdlib.h>
#endif
#include <limits.h>
#include "choose.h"

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

unsigned int choose (unsigned int n, unsigned int k)
{                               /* --- compute n choose k */
  unsigned int i, t;            /* loop variable, buffer */
  unsigned int r = 1;           /* result */

  if (k > n) return 0;          /* check range of k */
  for (i = 1; i <= k; i++) {    /* calculation loop */
    t = n--;                    /* calculate next factor in numerator */
    if (UINT_MAX /t < r)        /* if result of multiplication */
      return 0;                 /* is out of range, abort */
    r = (r *t) /i;              /* calculate \prod_{i=1}^k (n-i+1)/i */
  }
  return r;                     /* return result */
}  /* choose() */

/*--------------------------------------------------------------------*/

double dchoose (unsigned int n, unsigned int k)
{                               /* --- compute n choose k */
  unsigned int i;               /* loop variable, buffer */
  double r = 1.0;               /* result */

  if (k > n) return 0;          /* check range of k */
  for (i = 1; i <= k; i++)      /* calculation loop */
    r = (r *n--) /i;            /* calculate \prod_{i=1}^k (n-i+1)/i */
  return r;                     /* return result */
}  /* dchoose() */

/*--------------------------------------------------------------------*/
#ifdef CHOOSE_MAIN

int main (int argc, char *argv[])
{                               /* --- main function */
  int          n, k;            /* arguments */
  unsigned int r;               /* result */

  if (argc != 3) {              /* if number of arguments is wrong */
    printf("usage: %s <n> <k>\n", argv[0]);
    printf("calculate n choose k, i.e. n!/(k!(n-k)!)\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  n = atoi(argv[1]);            /* get argument 1, i.e. n */
  k = atoi(argv[2]);            /* get argument 2, i.e. k */
  if ((n < 0) || (k < 0)) {     /* check range of n and k */
    printf("%s: value out of range\n", argv[0]); return 1; }
  r = choose(n, k);             /* calculate n choose k */
  if (r > 0) printf("%d choose %d: %d\n",  n, k, r);
  else       printf("%d choose %d: %.0f\n", n, k, dchoose(n, k));
  return 0;                     /* print result and terminate program */
}  /* main() */

#endif
