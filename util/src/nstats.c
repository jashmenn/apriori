/*----------------------------------------------------------------------
  File    : nstats.c
  Contents: management of normalization statistics
  Author  : Christian Borgelt
  History : 2003.08.12 file created
            2004.08.12 description and parse function added
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include "nstats.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define BLKSIZE   64            /* block size for parsing */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

NSTATS* nst_create (int dim)
{                               /* --- create numerical statistics */
  NSTATS *nst;                  /* created statistics structure */
  double *p;                    /* to organize the memory */

  assert(dim > 0);              /* check the function argument */
  nst = (NSTATS*)malloc(sizeof(NSTATS) +(6*dim -1) *sizeof(double));
  if (!nst) return NULL;        /* create a statistics structure */
  nst->dim  = dim;              /* and initialize the fields */
  nst->reg  = 0;
  nst->offs = p = nst->facs +dim;
  nst->mins = p += dim;
  nst->maxs = p += dim;         /* organize the vectors */
  nst->sums = p += dim;
  nst->sqrs = p += dim;
  while (--dim >= 0) {          /* traverse the vectors */
    nst->mins[dim] = DBL_MAX; nst->maxs[dim] = -DBL_MAX;
    nst->sums[dim] = nst->sqrs[dim] = nst->offs[dim] = 0;
    nst->facs[dim] = 1;         /* initialize the ranges of values */
  }                             /* and the aggregation variables */
  return nst;                   /* return created structure */
}  /* nst_create() */

/*--------------------------------------------------------------------*/

void nst_delete (NSTATS *nst)
{ free(nst); }                  /* --- delete numerical statistics */

/*--------------------------------------------------------------------*/

void nst_reg (NSTATS *nst, const double *vec, double weight)
{                               /* --- register a data vector */
  int    i;                     /* loop variable */
  double *min, *max;            /* to traverse the min./max. values */
  double *sum, *sqr;            /* to traverse the value sums */
  double *off, *fac;            /* to traverse the offsets/scales */
  double t;                     /* temporary buffer */

  assert(nst && vec);           /* check the function arguments */
  sum = nst->sums;              /* get the vectors for the sums */
  sqr = nst->sqrs;              /* and the sums of squares */
  if (!vec) {                   /* if to terminate registration */
    off = nst->offs;            /* get the offsets and */
    fac = nst->facs;            /* the scaling factors */
    if (nst->reg <= 0)          /* if no patterns are registered */
      for (i = nst->dim; --i >= 0; ) { off[i] = 0; fac[i] = 1; }
    else {                      /* if patterns have been registered */
      for (i = nst->dim; --i >= 0; ) {      /* traverse the vectors */
        off[i] = sum[i] /nst->reg;
        t      = sqr[i] -off[i] *sum[i];
        fac[i] = (t > 0) ? sqrt(nst->reg /t) : 1;
      }                         /* estimate the parameters */
    }
    if (weight < 0) {           /* if to reinitialize registration */
      for (i = nst->dim; --i >= 0; )
        sum[i] = sqr[i] = 0;    /* reinitialize the vectors */
      nst->reg = 0;             /* and the pattern counter */
    } }
  else {                        /* if to register a data vector */
    min = nst->mins;            /* get the minimal */
    max = nst->maxs;            /* and the maximal values */
    for (i = nst->dim; --i >= 0; ) {
      if (vec[i] < min[i]) min[i] = vec[i];
      if (vec[i] > max[i]) max[i] = vec[i];
      sum[i] += vec[i];         /* update the ranges of values */
      sqr[i] += vec[i] *vec[i]; /* and sum the values */
    }                           /* and their squares */
    nst->reg += weight;         /* count the pattern */
  }
}  /* nst_reg() */

/*--------------------------------------------------------------------*/

void nst_range (NSTATS *nst, int idx, double min, double max)
{                               /* --- set range of values */
  int i;                        /* loop variable */

  assert(nst && (idx < nst->dim));  /* check the arguments */
  if (idx < 0) { i = nst->dim; idx = 0; }
  else         { i = idx +1; }  /* get index range to set */
  while (--i >= idx) {          /* and traverse it */
    nst->mins[i] = min;         /* set the minimal */
    nst->maxs[i] = max;         /* and the maximal value */
  }                             /* for all dimensions in range */
}  /* nst_range() */

/*--------------------------------------------------------------------*/

void nst_expand (NSTATS *nst, int idx, double factor)
{                               /* --- expand range of values */
  int    i;                     /* loop variable */
  double t;                     /* change of minimal/maximal value */

  assert(nst                    /* check the function arguments */
     && (idx < nst->dim) && (factor >= 0));
  if (idx < 0) { i = nst->dim; idx = 0; }
  else         { i = idx +1; }  /* get index range to expand */
  while (--i >= idx) {          /* and traverse it */
    t = (nst->maxs[i] -nst->mins[i]) *(factor -1) *0.5;
    nst->mins[i] -= t;          /* adapt the minimal */
    nst->maxs[i] += t;          /* and   the maximal value */
  }                             /* for all dimensions in range */
}  /* nst_expand() */

/*--------------------------------------------------------------------*/

void nst_scale (NSTATS *nst, int idx, double off, double fac)
{                               /* --- set (linear) scaling */
  int i;                        /* loop variable */

  assert(nst && (idx < nst->dim));  /* check the arguments */
  if (idx < 0) { i = nst->dim; idx = 0; }
  else         { i = idx +1; }  /* get index range to set */
  while (--i >= idx) {          /* and traverse it */
    nst->offs[i] = off;         /* set the offset */
    nst->facs[i] = fac;         /* and the scaling factor */
  }                             /* for all dimensions in range */
}  /* nst_scale() */

/*--------------------------------------------------------------------*/

void nst_norm (NSTATS *nst, const double *vec, double *res)
{                               /* --- normalize a data vector */
  int    i;                     /* loop variable */
  double *off, *fac;            /* to traverse the scaling parameters */

  assert(nst && vec && res);    /* check the function arguments */
  off = nst->offs +(i = nst->dim);
  fac = nst->facs + i;          /* get the scaling parameters */
  res += i; vec += i;           /* and the data vectors */
  while (--i >= 0) *--res = *--fac * (*--vec - *--off);
}  /* nst_norm() */             /* scale the vector */

/*--------------------------------------------------------------------*/

void nst_inorm (NSTATS *nst, const double *vec, double *res)
{                               /* --- inverse normalize a vector */
  int    i;                     /* loop variable */
  double *off, *fac;            /* to traverse the scaling parameters */

  assert(nst && vec && res);    /* check the function arguments */
  off = nst->offs +(i = nst->dim);
  fac = nst->facs + i;          /* get the scaling parameters */
  res += i; vec += i;           /* and the data vectors */
  while (--i >= 0) *--res = *--vec / *--fac + *--off;
}  /* nst_inorm() */            /* scale the vector */

/*--------------------------------------------------------------------*/

void nst_center (NSTATS *nst, double *vec)
{                               /* --- get center of data space */
  int    i;                     /* loop variable */
  double *min, *max;            /* to traverse the ranges */

  assert(nst && vec);           /* check the function arguments */
  min = nst->mins;              /* get the range variables, */
  max = nst->maxs;              /* traverse the dimensions, */
  for (i = nst->dim; --i >= 0;) /* and compute the center vector */
    vec[i] = 0.5 *(max[i] +min[i]);
}  /* nst_center() */

/*--------------------------------------------------------------------*/

void nst_spans (NSTATS *nst, double *vec)
{                               /* --- get spans of dimensions */
  int    i;                     /* loop variable */
  double *min, *max;            /* to traverse the ranges */

  assert(nst && vec);           /* check the function arguments */
  min = nst->mins;            
  max = nst->maxs;              /* get the range variables, */ 
  for (i = nst->dim; --i >= 0;) /* traverse the dimensions, */ 
    vec[i] = max[i] -min[i];    /* and compute the spans */
}  /* nst_spans() */

/*--------------------------------------------------------------------*/

int nst_desc (NSTATS *nst, FILE *file, const char *indent, int maxlen)
{                               /* --- describe norm. statistics */
  int  i;                       /* loop variable */
  int  pos, ind;                /* position in output line */
  char buf[64];                 /* buffer for output */

  for (i = nst->dim; --i >= 0;) /* check for non-identity scaling */
    if ((nst->offs[i] != 0) || (nst->facs[i] != 1)) break;
  if (i < 0) return 0;          /* if all identity scaling, abort */

  fputs(indent,        file);   /* write the indentation and */
  fputs("scales   = ", file);   /* start the scaling parameters */
  for (ind = 0; indent[ind]; ind++);
  pos = ind +9;                 /* compute the starting position */
  for (i = 0; i < nst->dim; i++) {
    pos += sprintf(buf, "[%g, %g]", nst->offs[i], nst->facs[i]);
    if (i > 0) {                /* format the scaling parameters */
      if (pos +3 <= maxlen) { fputs(", ", file);      pos += 2;   }
      else { fprintf(file, ",\n%s         ", indent); pos  = ind; }
    }                           /* print separator and indentation */
    fputs(buf, file);           /* print formatted offset and factor */
  }
  fputs(";\n", file);           /* terminate the list */
  return ferror(file) ? -1 : 0; /* return the write status */
}  /* nst_desc() */

/*--------------------------------------------------------------------*/
#ifdef NST_PARSE

static int _parse (SCAN *scan, int dim, double **buf)
{                               /* --- parse normalization statistics */
  int    k, n = 0;              /* loop variable, counter */
  double *p;                    /* to access the statistics elements */

  assert(scan);                 /* check the function arguments */
  if ((sc_token(scan) != T_ID)  /* check whether 'scales' follows */
  ||  (strcmp(sc_value(scan), "scales") != 0))
    ERR_STR("scales");          /* if not, abort the function */
  GET_TOK();                    /* consume 'scales' */
  GET_CHR('=');                 /* consume '=' */
  for (k = 0; (dim <= 0) || (k < dim); k++) {
    if (k > 0) { GET_CHR(',');} /* if not first, consume ',' */
    if (k >= n) {               /* if the statistics vector is full */
      if (dim > 0) n  = dim;    /* compute the new vector size */
      else         n += (n > BLKSIZE) ? n >> 1 : BLKSIZE;
      p = (double*)realloc(*buf, (n+n) *sizeof(double));
      if (!p) ERROR(E_NOMEM);   /* enlarge the buffer vector */
      *buf = p;                 /* and set the new vector, */
    }                           /* then note factor and offset */
    p = *buf +k +k;             /* get the element to set */
    GET_CHR('[');               /* consume '[' */
    if (sc_token(scan) != T_NUM) ERROR(E_NUMEXP);
    p[0] = strtod(sc_value(scan), NULL);
    GET_TOK();                  /* consume the offset */
    GET_CHR(',');               /* consume '[' */
    if (sc_token(scan) != T_NUM) ERROR(E_NUMEXP);
    p[1] = strtod(sc_value(scan), NULL);
    GET_TOK();                  /* consume the factor */
    GET_CHR(']');               /* consume '[' */
    if ((dim <= 0) && (sc_token(scan) != ',')) {
      k++; break; }             /* check for more scaling params. */
  }
  GET_CHR(';');                 /* consume ';' */
  return k;                     /* return 'ok' */
}  /* _parse() */

/*--------------------------------------------------------------------*/

NSTATS* nst_parse (SCAN *scan, int dim)
{                               /* --- parse normalization statistics */
  NSTATS *nst;                  /* created normalization statistics */
  double *buf = NULL;           /* buffer for reading */

  assert(scan);                 /* check the function arguments */
  dim = _parse(scan,dim, &buf); /* parse normalization statistics */
  if (dim < 0) { if (buf) free(buf); return NULL; }
  nst = nst_create(dim);        /* create a statistics structure */
  if (!nst)    { free(buf); return NULL; }
  for (buf += dim +dim; --dim >= 0; ) {
    nst->facs[dim] = *--buf;    /* copy the buffered values */
    nst->offs[dim] = *--buf;    /* into the corresponding vectors */
  }
  free(buf);                    /* delete the read buffer */
  return nst;                   /* return the created structure */
}  /* nst_parse() */

#endif
