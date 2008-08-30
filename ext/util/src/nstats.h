/*----------------------------------------------------------------------
  File    : nstats.h
  Contents: management of normalization statistics
  Author  : Christian Borgelt
  History : 2003.08.12 file created
            2004.08.12 description and parse function added
----------------------------------------------------------------------*/
#ifndef __NSTATS__
#define __NSTATS__
#include <stdio.h>
#ifdef NST_PARSE
#include "parse.h"
#endif

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- numerical statistics --- */
  int    dim;                   /* dimension of data space */
  double reg;                   /* number of registered patterns */
  double *mins;                 /* minimal data values */
  double *maxs;                 /* maximal data values */
  double *sums;                 /* sums of data values */
  double *sqrs;                 /* sums of squared data values */
  double *offs;                 /* offsets for data scaling */
  double facs[1];               /* factors for data scaling */
} NSTATS;                       /* (numerical statistics) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern NSTATS* nst_create (int dim);
extern void    nst_delete (NSTATS *nst);
extern int     nst_dim    (NSTATS *nst);

extern void    nst_reg    (NSTATS *nst, const double *vec,
                           double weight);
extern void    nst_range  (NSTATS *nst, int idx,
                           double min, double max);
extern void    nst_expand (NSTATS *nst, int idx, double factor);
extern void    nst_scale  (NSTATS *nst, int idx,
                           double off, double fac);

extern double  nst_min    (NSTATS *nst, int idx);
extern double  nst_max    (NSTATS *nst, int idx);
extern double  nst_offset (NSTATS *nst, int idx);
extern double  nst_factor (NSTATS *nst, int idx);

extern void    nst_norm   (NSTATS *nst, const double *vec, double *res);
extern void    nst_inorm  (NSTATS *nst, const double *vec, double *res);
extern void    nst_center (NSTATS *nst, double *vec);
extern void    nst_spans  (NSTATS *nst, double *vec);

extern int     nst_desc   (NSTATS *nst, FILE *file,
                           const char *indent, int maxlen);
#ifdef NST_PARSE
extern NSTATS* nst_parse  (SCAN *scan, int dim);
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define nst_dim(s)        ((s)->dim)
#define nst_min(s,i)      ((s)->mins[i])
#define nst_max(s,i)      ((s)->maxs[i])
#define nst_offset(s,i)   ((s)->offs[i])
#define nst_factor(s,i)   ((s)->facs[i])

#endif
