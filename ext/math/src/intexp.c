/*----------------------------------------------------------------------
  File    : intexp.c
  Contents: raise x to the power of n, n integer
  Author  : Christian Borgelt
  History : 17.05.2003 file created
----------------------------------------------------------------------*/
#include "intexp.h"

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
#if 0

double intexp (double x, int n)
{                               /* --- raise x to the power of n */
  if (n <= 0) return 0;         /* check for immediate cases, otherw. */
  if (n <= 1) return x;         /* apply the recursive formula */
  if (n &  1) return intexp(x*x, n >> 1) *x;
  else        return intexp(x*x, n >> 1);
}  /* intexp() */

#else /*--------------------------------------------------------------*/

double intexp (double x, int n)
{                               /* --- raise x to the power of n */
  double r = (n & 1) ? x : 1;   /* result */

  for (n >>= 1; n > 0; n >>= 1) {
    x *= x;                     /* traverse the powers of 2 and */
    if (n & 1) r *= x;          /* multiply them into the result */
  }                             /* if the corr. exponent bit is set */
  return r;                     /* return the result */
}  /* intexp() */

#endif
