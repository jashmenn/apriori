/*----------------------------------------------------------------------
  File    : tract.c
  Contents: item and transaction management
  Author  : Christian Borgelt
  History : 1996.02.14 file created as apriori.c
            1996.06.24 function _get_item optimized
            1996.07.01 adapted to modified symtab module
            1998.01.04 scan functions moved to module 'tabscan'
            1998.06.09 vector enlargement modified
            1998.06.20 adapted to changed st_create function
            1998.08.07 bug in function _get_tract (is_read) fixed
            1998.08.08 item appearances added
            1998.08.17 item sorting and recoding added
            1998.09.02 several assertions added
            1999.02.05 long int changed to int
            1999.10.22 bug in item appearances reading fixed
            1999.11.11 adapted to name/identifier maps
            1999.12.01 check of item appearance added to sort function
            2000.03.15 removal of infrequent items added
            2001.07.14 adapted to modified module tabscan
            2001.12.27 item functions made a separate module
            2001.11.18 transaction functions made a separate module
            2001.12.28 first version of this module completed
            2002.01.12 empty field at end of record reported as error
            2002.02.06 item sorting reversed (ascending order)
            2002.02.19 transaction tree functions added
            2003.07.17 functions is_filter, ta_filter, tas_filter added
            2003.08.15 bug in function tat_delete fixed
            2003.08.21 parameter 'heap' added to tas_sort, tat_create
            2003.09.20 empty transactions in input made possible
            2003.12.18 padding for 64 bit architecture added
            2004.02.26 item frequency counting moved to is_read
            2004.11.20 function tat_mark added
            2005.06.20 function _nocmp added for neutral sorting
            2006.11.26 structures ISFMTR and ISEVAL added
            2007.02.13 adapted to modified tabscan module
            2008.01.25 bug in function ise_eval fixed (prefix)
            2008.06.30 support argument to ise_eval changed to double
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include "tract.h"
#include "scan.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define BLKSIZE  256            /* block size for enlarging vectors */

#define LN_2     0.69314718055994530942   /* ln(2) */

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
/* --- item appearance indicators --- */
static const char *i_body[] = { /* item to appear in bodies only */
  "i",  "in",  "a", "ante", "antecedent", "b", "body", NULL };
static const char *i_head[] = { /* item to appear in heads only */
  "o",  "out", "c", "cons", "consequent", "h", "head", NULL };
static const char *i_both[] = { /* item to appear in both */
  "io", "inout", "ac", "bh", "both",                   NULL };
static const char *i_ignore[] ={/* item to ignore */
  "n", "neither", "none", "ign", "ignore", "-",        NULL };

/*----------------------------------------------------------------------
  Auxiliary Functions
----------------------------------------------------------------------*/

static int _appcode (const char *s)
{                               /* --- get appearance indicator code */
  const char **p;               /* to traverse indicator list */

  assert(s);                    /* check the function argument */
  for (p = i_body;   *p; p++)   /* check 'body' indicators */
    if (strcmp(s, *p) == 0) return APP_BODY;
  for (p = i_head;   *p; p++)   /* check 'head' indicators */
    if (strcmp(s, *p) == 0) return APP_HEAD;
  for (p = i_both;   *p; p++)   /* check 'both' indicators */
    if (strcmp(s, *p) == 0) return APP_BOTH;
  for (p = i_ignore; *p; p++)   /* check 'ignore' indicators */
    if (strcmp(s, *p) == 0) return APP_NONE;
  return -1;                    /* if none found, return error code */
}  /* _appcode() */

/*--------------------------------------------------------------------*/

static int _get_item (ITEMSET *iset, FILE *file)
{                               /* --- read an item */
  int  d;                       /* delimiter type */
  char *buf;                    /* read buffer */
  ITEM *item;                   /* pointer to item */
  int  *vec;                    /* new item vector */
  int  size;                    /* new item vector size */

  assert(iset && file);         /* check the function arguments */
  d   = ts_next(iset->tscan, file, NULL, 0);
  buf = ts_buf(iset->tscan);    /* read the next field (item name) */
  if ((d == TS_ERR) || (buf[0] == '\0')) return d;
  fprintf(stderr, "calling _get_item\n");
  item = nim_byname(iset->nimap, buf);
  fprintf(stderr, "calling _get_item\n");
  if (!item) {                  /* look up the name in name/id map */
    if (iset->app == APP_NONE)  /* if new items are to be ignored, */
      return d;                 /* do not register the item */
    item = nim_add(iset->nimap, buf, sizeof(ITEM));
    if (!item) return E_NOMEM;  /* add the new item to the map, */
    item->frq = item->xfq = 0;  /* initialize the frequency counters */
    item->app = iset->app;      /* (occurrence and sum of t.a. sizes) */
  }                             /* and set the appearance indicator */
  size = iset->vsz;             /* get the item vector size */
  if (iset->cnt >= size) {      /* if the item vector is full */
    size += (size > BLKSIZE) ? (size >> 1) : BLKSIZE;
    vec   = (int*)realloc(iset->items, size *sizeof(int));
    if (!vec) return E_NOMEM;   /* enlarge the item vector */
    iset->items = vec; iset->vsz = size;
  }                             /* set the new vector and its size */
  iset->items[iset->cnt++] = item->id;
  return d;                     /* add the item to the transaction */
}  /* _get_item() */            /* and return the delimiter type */

/*--------------------------------------------------------------------*/

static int _nocmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  if (((const ITEM*)p1)->app == APP_NONE)
    return (((const ITEM*)p2)->app == APP_NONE) ? 0 : 1;
  if (((const ITEM*)p2)->app == APP_NONE) return -1;
  #ifdef ARCH64
  if (((const ITEM*)p1)->frq < (long)data)
    return (((const ITEM*)p2)->frq < (long)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (long)data) return -1;
  #else
  if (((const ITEM*)p1)->frq < (int)data)
    return (((const ITEM*)p2)->frq < (int)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (int)data) return -1;
  #endif
  if (((const ITEM*)p1)->id  > ((const ITEM*)p2)->id) return  1;
  if (((const ITEM*)p1)->id  < ((const ITEM*)p2)->id) return -1;
  return 0;                     /* return sign of identifier diff. */
}  /* _nocmp() */

/*--------------------------------------------------------------------*/

static int _asccmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  if (((const ITEM*)p1)->app == APP_NONE)
    return (((const ITEM*)p2)->app == APP_NONE) ? 0 : 1;
  if (((const ITEM*)p2)->app == APP_NONE) return -1;
  #ifdef ARCH64
  if (((const ITEM*)p1)->frq < (long)data)
    return (((const ITEM*)p2)->frq < (long)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (long)data) return -1;
  #else
  if (((const ITEM*)p1)->frq < (int)data)
    return (((const ITEM*)p2)->frq < (int)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (int)data) return -1;
  #endif
  if (((const ITEM*)p1)->frq > ((const ITEM*)p2)->frq) return  1;
  if (((const ITEM*)p1)->frq < ((const ITEM*)p2)->frq) return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* _asccmp() */

/*--------------------------------------------------------------------*/

static int _descmp (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  if (((const ITEM*)p1)->app == APP_NONE)
    return (((const ITEM*)p2)->app == APP_NONE) ? 0 : 1;
  if (((const ITEM*)p2)->app == APP_NONE) return -1;
  if (((const ITEM*)p1)->frq > ((const ITEM*)p2)->frq) return -1;
  if (((const ITEM*)p1)->frq < ((const ITEM*)p2)->frq) return  1;
  return 0;                     /* return sign of frequency diff. */
}  /* _descmp() */

/*--------------------------------------------------------------------*/

static int _asccmpx (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  if (((const ITEM*)p1)->app == APP_NONE)
    return (((const ITEM*)p2)->app == APP_NONE) ? 0 : 1;
  if (((const ITEM*)p2)->app == APP_NONE) return -1;
  #ifdef ARCH64
  if (((const ITEM*)p1)->frq < (long)data)
    return (((const ITEM*)p2)->frq < (long)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (long)data) return -1;
  #else
  if (((const ITEM*)p1)->frq < (int)data)
    return (((const ITEM*)p2)->frq < (int)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (int)data) return -1;
  #endif
  if (((const ITEM*)p1)->xfq > ((const ITEM*)p2)->xfq) return  1;
  if (((const ITEM*)p1)->xfq < ((const ITEM*)p2)->xfq) return -1;
  return 0;                     /* return sign of frequency diff. */
}  /* _asccmpx() */

/*--------------------------------------------------------------------*/

static int _descmpx (const void *p1, const void *p2, void *data)
{                               /* --- compare item frequencies */
  if (((const ITEM*)p1)->app == APP_NONE)
    return (((const ITEM*)p2)->app == APP_NONE) ? 0 : 1;
  if (((const ITEM*)p2)->app == APP_NONE) return -1;
  #ifdef ARCH64
  if (((const ITEM*)p1)->frq < (long)data)
    return (((const ITEM*)p2)->frq < (long)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (long)data) return -1;
  #else
  if (((const ITEM*)p1)->frq < (int)data)
    return (((const ITEM*)p2)->frq < (int)data) ? 0 : 1;
  if (((const ITEM*)p2)->frq < (int)data) return -1;
  #endif
  if (((const ITEM*)p1)->xfq > ((const ITEM*)p2)->xfq) return -1;
  if (((const ITEM*)p1)->xfq < ((const ITEM*)p2)->xfq) return  1;
  return 0;                     /* return sign of frequency diff. */
}  /* _descmpx() */

/*----------------------------------------------------------------------
  Item Set Functions
----------------------------------------------------------------------*/

ITEMSET* is_create (int cnt)
{                               /* --- create an item set */
  ITEMSET *iset;                /* created item set */

  if (cnt <= 0) cnt = BLKSIZE;  /* check and adapt number of items */
  iset = malloc(sizeof(ITEMSET));
  if (!iset) return NULL;       /* create an item set */
  iset->tscan = ts_create();    /* and its components */
  ts_chars(iset->tscan, TS_NULL, "");
  iset->nimap = nim_create(0, 0, (HASHFN*)0, (SYMFN*)0);
  iset->items = (int*)malloc(cnt *sizeof(int));
  if (!iset->tscan || !iset->nimap || !iset->items) {
    is_delete(iset); return NULL; }
  iset->tac = iset->cnt = 0;    /* initialize the other fields */
  iset->app = APP_BOTH;
  iset->vsz = cnt;
  iset->chars[0] = ' ';  iset->chars[1] = ' ';
  iset->chars[2] = '\n'; iset->chars[3] = '\0';
  return iset;                  /* return the created item set */
}  /* is_create() */

/*--------------------------------------------------------------------*/

void is_delete (ITEMSET *iset)
{                               /* --- delete an item set */
  assert(iset);                 /* check the function argument */
  if (iset->items) free(iset->items);
  if (iset->nimap) nim_delete(iset->nimap);
  if (iset->tscan) ts_delete(iset->tscan);
  free(iset);                   /* delete the components */
}  /* is_delete() */            /* and the item set body */

/*--------------------------------------------------------------------*/

void is_chars (ITEMSET *iset, const char *blanks,  const char *fldseps,
                              const char *recseps, const char *comment)
{                               /* --- set special characters */
  assert(iset);                 /* check the function argument */
  if (blanks)                   /* set blank characters */
    iset->chars[0] = ts_chars(iset->tscan, TS_BLANK,  blanks);
  if (fldseps)                  /* set field separators */
    iset->chars[1] = ts_chars(iset->tscan, TS_FLDSEP, fldseps);
  if (recseps)                  /* set record separators */
    iset->chars[2] = ts_chars(iset->tscan, TS_RECSEP, recseps);
  if (comment)                  /* set comment indicators */
    ts_chars(iset->tscan, TS_COMMENT, comment);
}  /* is_chars() */

/*--------------------------------------------------------------------*/

int is_item (ITEMSET *iset, const char *name)
{                               /* --- get an item identifier */
  ITEM *item = nim_byname(iset->nimap, name);
  return (item) ? item->id :-1; /* look up the given name */
}  /* is_item() */              /* in the name/identifier map */

/*--------------------------------------------------------------------*/

int is_readapp (ITEMSET *iset, FILE *file)
{                               /* --- read appearance indicators */
  int  d;                       /* delimiter type */
  char *buf;                    /* read buffer */
  ITEM *item;                   /* to access the item data */

  assert(iset && file);         /* check the function arguments */
  buf = ts_buf(iset->tscan);    /* read the first record (one field) */
  d   = ts_next(iset->tscan, file, NULL, 0);
  if (d == TS_ERR)      return E_FREAD;
  if (d != TS_REC)      return E_FLDCNT;
  iset->app = _appcode(buf);    /* get default appearance code */
  if (iset->app < 0)    return E_UNKAPP;
  while (1) {                   /* read item/indicator pairs */
    d = ts_next(iset->tscan, file, NULL, 0);
    if (d <= TS_EOF)            /* read the next item */
      return (d == TS_ERR) ? E_FREAD : 0;
    if (buf[0] == '\0')         /* check for end of file */
      return E_ITEMEXP;         /* and for a missing item */
    item = nim_add(iset->nimap, buf, sizeof(ITEM));
    if (item == EXISTS) return E_DUPITEM;  /* add the new item */
    if (item == NULL)   return E_NOMEM;    /* to the name/id map */
    item->frq = 0;              /* clear the frequency counters */
    item->xfq = 0;              /* (occurrence and sum of t.a. sizes) */
    if (d != TS_FLD)    return E_APPEXP;
    d = ts_next(iset->tscan, file, NULL, 0);
    if (d == TS_ERR)    return E_FREAD;
    if (d == TS_FLD)    return E_FLDCNT;
    item->app = _appcode(buf);  /* get the appearance indicator */
    if (item->app <  0) return E_UNKAPP;
  }
  return 0;                     /* return 'ok' */
}  /* is_readapp() */

/*--------------------------------------------------------------------*/

int is_read (ITEMSET *iset, FILE *file)
{                               /* --- read a transaction */
  int  i, d;                    /* loop variable, delimiter type */
  char *buf;                    /* read buffer */
  ITEM *item;                   /* pointer to item */


  assert(iset && file);         /* check the function arguments */
  iset->cnt = 0;                /* initialize the item counter */
  fprintf(stderr, "calling is_read\n");
  d   = _get_item(iset, file);  /* read the first item and */
  buf = ts_buf(iset->tscan);    /* get the read buffer */

  if ((d      == TS_EOF)        /* if at the end of the file */
  &&  (buf[0] == '\0'))         /* and no item has been read, */
    return 1;                   /* return 'end of file' */
  while ((d      == TS_FLD)     /* read the other items */
  &&     (buf[0] != '\0'))      /* of the transaction */
    d = _get_item(iset, file);  /* up to the end of the record */
  if (d == TS_ERR) return d;    /* check for a read error */
  if ((buf[0] == '\0') && (d == TS_FLD) && (iset->cnt > 0))
    return E_ITEMEXP;           /* check for an empty field */
  ta_sort(iset->items, iset->cnt); /* prepare the transaction */
  iset->cnt = ta_unique(iset->items, iset->cnt);
  for (i = iset->cnt; --i >= 0; ) {
    item = nim_byid(iset->nimap, iset->items[i]);
    item->frq += 1;             /* count the item and */
    item->xfq += iset->cnt;     /* sum the transaction sizes */
  }                             /* as an importance indicator */
  iset->tac += 1;               /* count the transaction */
  return 0;                     /* return 'ok' */
}  /* is_read() */

/*--------------------------------------------------------------------*/

int is_recode (ITEMSET *iset, int minfrq, int dir, int *map)
{                               /* --- recode items w.r.t. frequency */
  int      i, k, n, t;          /* loop variables, buffer */
  ITEM     *item;               /* to traverse the items */
  SYMCMPFN *cmp;                /* comparison function */

  assert(iset);                 /* check the function arguments */
  if      (dir >  1) cmp = _asccmpx;  /* get the appropriate */
  else if (dir >  0) cmp = _asccmp;   /* comparison function */
  else if (dir >= 0) cmp = _nocmp;    /* (ascending/descending) */
  else if (dir > -2) cmp = _descmp;   /* and sort the items */
  else               cmp = _descmpx;  /* w.r.t. their frequency */
  nim_sort(iset->nimap, cmp, (void*)minfrq, map, 1);
  for (n = nim_cnt(iset->nimap); --n >= 0; ) {
    item = (ITEM*)nim_byid(iset->nimap, n);
    if (item->frq < minfrq)     /* determine frequent items and */
      item->app = APP_NONE;     /* set all others to 'ignore' */
    else if (item->app != APP_NONE)
      break;                    /* in addition, skip all items */
  }                             /* that have been set to 'ignore' */
  if (map) {                    /* if a map vector is provided */
    for (i = k = 0; i < iset->cnt; i++) {
      t = map[iset->items[i]];  /* traverse the current transaction */
      if (t <= n) iset->items[k++] = t;
    }                           /* recode all items and */
    iset->cnt = k;              /* delete all items to ignore */
    ta_sort(iset->items, k);    /* resort the items */
  }
  return n+1;                   /* return number of frequent items */
}  /* is_recode() */

/*--------------------------------------------------------------------*/

int is_filter (ITEMSET *iset, const char *marks)
{                               /* --- filter items in transaction */
  return iset->cnt = ta_filter(iset->items, iset->cnt, marks);
}  /* is_filter() */

/*----------------------------------------------------------------------
  Item Set Evaluation Functions
----------------------------------------------------------------------*/

ISEVAL* ise_create (ITEMSET *iset, int tacnt)
{                               /* --- create an item set evaluation */
  int    i;                     /* loop variable */
  ISEVAL *eval;                 /* created item set evaluator */

  i    = is_cnt(iset);          /* get the number of items */
  eval = (ISEVAL*)malloc(sizeof(ISEVAL) +(i+i) *sizeof(double));
  if (!eval) return NULL;       /* create an evaluation object */
  eval->logfs = eval->lsums +i +1;  /* and organize the memory */
  eval->logta = log(tacnt);     /* store log of number of trans. */
  while (--i >= 0)              /* compute logarithms of item freqs. */
    eval->logfs[i] = log(is_getfrq(iset, i));
  eval->lsums[0] = 0;           /* init. first sum of logarithms */
  return eval;                  /* return created item set evaluator */
}  /* ise_create() */

/*--------------------------------------------------------------------*/

double ise_eval (ISEVAL *eval, int *ids, int cnt, int pfx, double supp)
{                               /* --- evaluate an item set */
  double sum;                   /* sum of logarithms of frequencies */

  sum = (pfx > 0)               /* if there is a prefix, */
      ? eval->lsums[pfx-1] : 0; /* get already known logarithm sum */
  for ( ; pfx < cnt; pfx++)     /* compute and add remaining terms */
    eval->lsums[pfx] = sum += eval->logfs[ids[pfx]];
  return (log(supp) -sum +(cnt-1) *eval->logta) * (1.0/LN_2);
}  /* ise_eval() */             /* compute logarithm of quotient */

/*----------------------------------------------------------------------
  Item Set Formatting Functions
----------------------------------------------------------------------*/

ISFMTR* isf_create (ITEMSET *iset, int scan)
{                               /* --- create an item set formatter */
  int        i, k, n;           /* loop variable, buffers */
  int        len, sum;          /* length of an item name and sum */
  ISFMTR     *fmt;              /* created item set formatter */
  char       buf[4*TS_SIZE+4];  /* buffer for formatting */
  const char *name;             /* to traverse the item names */
  char       *copy;             /* for copies of formatted names */

  n   = is_cnt(iset);           /* get the number of items */
  fmt = (ISFMTR*)malloc(sizeof(ISFMTR) + n    *sizeof(int)
                                       +(n-1) *sizeof(char*));
  if (!fmt) return NULL;        /* create the base structure */
  fmt->buf  = NULL;             /* and organize the memory */
  fmt->offs = (int*)(fmt->names +n);
  for (i = sum = fmt->cnt = 0; i < n; i++) {
    name = is_name(iset, i);    /* traverse the item names */
    len  = strlen(name);        /* and get their length */
    sum += k = (scan) ? sc_format(buf, name, 0) : len;
    if (k > len) {              /* if formatting was needed */
      copy = (char*)malloc((k+1) *sizeof(char));
      if (!copy) { fmt->cnt = i-1; isf_delete(fmt); return NULL; }
      name = strcpy(copy, buf); /* copy the formatted name */
    }                           /* into a newly created string */
    fmt->names[i] = name;       /* store (formatted) item name */
  }                             /* afterwards create output buffer */
  if (scan) fmt->cnt = n;       /* note the number of items */
  fmt->buf = (char*)malloc((sum +n +1) *sizeof(char));
  if (!fmt->buf) { isf_delete(fmt); return NULL; }
  fmt->offs[0] = 0;             /* init. the first prefix offset */
  return fmt;                   /* return created item set formatter */
}  /* isf_create() */

/*--------------------------------------------------------------------*/

void isf_delete (ISFMTR *fmt)
{                               /* --- delete an item set formatter */
  int i;                        /* loop variable */
  for (i = fmt->cnt; --i >= 0; )
    if ((fmt->names[i]    != NULL)
    &&  (fmt->names[i][0] == '"'))
      free((void*)fmt->names[i]);
  if (fmt->buf) free(fmt->buf); /* delete reformatted item names, */
  free(fmt);                    /* the output buffer and the base */
}  /* isf_delete() */

/*--------------------------------------------------------------------*/

const char* isf_format (ISFMTR *fmt, int *ids, int cnt, int pre)
{                               /* --- format an item set */
  char       *p;                /* to traverse the output buffer */
  const char *name;             /* to traverse the item names */

  p = fmt->buf +fmt->offs[pre]; /* get position for appending */
  while (pre < cnt) {           /* traverse the additional items */
    name = fmt->names[ids[pre]];/* copy the item name to the output */
    while (*name) *p++ = *name++;
    *p++ = ' ';                 /* add an item separator */
    fmt->offs[++pre] = (int)(p-fmt->buf);
  }                             /* record the new offset */
  *p = '\0';                    /* terminate the formatted item set */
  fmt->len = (int)(p-fmt->buf); /* note the length of the description */
  return fmt->buf;              /* return the output buffer */
}  /* isf_format() */

/*----------------------------------------------------------------------
  Transaction Functions
----------------------------------------------------------------------*/

int ta_unique (int *items, int n)
{                               /* --- remove duplicate items */
  int *s, *d;                   /* to traverse the item vector */

  assert(items && (n >= 0));    /* check the function arguments */
  if (n <= 1) return n;         /* check for 0 or 1 item */
  for (d = s = items; --n > 0;) /* traverse the sorted vector */
    if (*++s != *d) *++d = *s;  /* and remove duplicate items */ 
  return (int)(++d -items);     /* return the new number of items */
}  /* ta_unique() */

/*--------------------------------------------------------------------*/

int ta_filter (int *items, int n, const char *marks)
{                               /* --- filter items in a transaction */
  int i, k;                     /* loop variables */

  assert(items && (n >= 0));    /* check the function arguments */
  for (i = k = 0; i < n; i++)   /* remove all unmarked items */
    if (marks[items[i]]) items[k++] = items[i];
  return k;                     /* return the new number of items */
}  /* ta_filter() */

/*--------------------------------------------------------------------*/

static int ta_cmp (const void *p1, const void *p2, void *data)
{                               /* --- compare transactions */
  int       k, k1, k2;          /* loop variable, counters */
  const int *i1, *i2;           /* to traverse the item identifiers */

  assert(p1 && p2);             /* check the function arguments */
  i1 = ((const TRACT*)p1)->items;
  i2 = ((const TRACT*)p2)->items;
  k1 = ((const TRACT*)p1)->cnt; /* get the item vectors */
  k2 = ((const TRACT*)p2)->cnt; /* and the numbers of items */
  for (k  = (k1 < k2) ? k1 : k2; --k >= 0; i1++, i2++) {
    if (*i1 > *i2) return  1;   /* compare corresponding items */
    if (*i1 < *i2) return -1;   /* and abort the comparison */
  }                             /* if one of them is greater */
  if (k1 > k2) return  1;       /* if one of the transactions */
  if (k1 < k2) return -1;       /* is not empty, it is greater */
  return 0;                     /* otherwise the two trans. are equal */
}  /* ta_cmp() */

/*--------------------------------------------------------------------*/

static int ta_cmpx (const TRACT *ta, const int *items, int n)
{                               /* --- compare transactions */
  int       k, m;               /* loop variable, counter */
  const int *p;                 /* to traverse the item identifiers */

  assert(ta && items);          /* check the function arguments */
  p = ta->items; m = ta->cnt;   /* traverse the item vector */
  m = ta->cnt;
  for (k = (n < m) ? n : m; --k >= 0; p++, items++) {
    if (*p > *items) return  1; /* compare corresponding items */
    if (*p < *items) return -1; /* and abort the comparison */
  }                             /* if one of them is greater */
  if (m > n) return  1;         /* if one of the transactions */
  if (m < n) return -1;         /* is not empty, it is greater */
  return 0;                     /* otherwise the two trans. are equal */
}  /* ta_cmpx() */

/*----------------------------------------------------------------------
  Transaction Set Functions
----------------------------------------------------------------------*/

TASET* tas_create (ITEMSET *itemset)
{                               /* --- create a transaction set */
  TASET *taset;                 /* created transaction set */

  assert(itemset);              /* check the function argument */
  taset = malloc(sizeof(TASET));
  if (!taset) return NULL;      /* create a transaction set */
  taset->itemset = itemset;     /* and store the item set */
  taset->cnt     = taset->vsz = taset->max = taset->total = 0;
  taset->tracts  = NULL;        /* initialize the other fields */
  return taset;                 /* return the created t.a. set */
}  /* tas_create() */

/*--------------------------------------------------------------------*/

void tas_delete (TASET *taset, int delis)
{                               /* --- delete a transaction set */
  assert(taset);                /* check the function argument */
  if (taset->tracts) {          /* if there are loaded transactions */
    while (--taset->cnt >= 0)   /* traverse the transaction vector */
      free(taset->tracts[taset->cnt]);
    free(taset->tracts);        /* delete all transactions */
  }                             /* and the transaction vector */
  if (delis && taset->itemset) is_delete(taset->itemset);
  free(taset);                  /* delete the item set and */
}  /* tas_delete() */           /* the transaction set body */

/*--------------------------------------------------------------------*/

int tas_add (TASET *taset, const int *items, int n)
{                               /* --- add a transaction */
  TRACT *ta;                    /* new transaction */
  int   *p;                     /* to traverse the transaction */
  TRACT **vec;                  /* new transaction vector */
  int   size;                   /* new transaction vector size */

  assert(taset);                /* check the function arguments */
  size = taset->vsz;            /* get the transaction vector size */
  if (taset->cnt >= size) {     /* if the transaction vector is full */
    size += (size > BLKSIZE) ? (size >> 1) : BLKSIZE;
    vec   = (TRACT**)realloc(taset->tracts, size *sizeof(TRACT*));
    if (!vec) return -1;        /* enlarge the transaction vector */
    taset->tracts = vec; taset->vsz = size;
  }                             /* set the new vector and its size */
  if (!items) {                 /* if no transaction is given */
    items = is_tract(taset->itemset);
    n     = is_tsize(taset->itemset);
  }                             /* get it from the item set */
  ta = (TRACT*)malloc(sizeof(TRACT) +(n-1) *sizeof(int));
  if (!ta) return -1;           /* create a new transaction */
  taset->tracts[taset->cnt++]  = ta;
  if (n > taset->max)           /* store the transaction and */
    taset->max = n;             /* update maximal transaction size */
  taset->total += n;            /* sum the number of items */
  for (p = ta->items +(ta->cnt = n); --n >= 0; )
    *--p = items[n];            /* copy the items of the t.a. */
  return 0;                     /* return 'ok' */
}  /* tas_add() */

/*--------------------------------------------------------------------*/

void tas_recode (TASET *taset, int *map, int cnt)
{                               /* --- recode items */
  int   i, k, n, x;             /* loop variables, buffer */
  TRACT *t;                     /* to traverse the transactions */
  int   *p;                     /* to traverse the item identifiers */

  assert(taset && map);         /* check the function arguments */
  taset->max = taset->total = 0;/* clear the maximal size and total */
  for (n = taset->cnt; --n >= 0; ) {
    t = taset->tracts[n];       /* traverse the transactions and */
    p = t->items;               /* the items of each transaction */
    for (i = k = 0; i < t->cnt; i++) {
      x = map[p[i]];            /* recode the items and */
      if (x < cnt) p[k++] = x;  /* remove superfluous items */
    }                           /* from the transaction */
    if (k > taset->max)         /* update the max. transaction size */
      taset->max = k;           /* with the new size of the t.a. */
    taset->total += k;          /* sum the number of items */
    ta_sort(t->items, t->cnt = k);
  }                             /* resort the item identifiers */
}  /* tas_recode() */

/*--------------------------------------------------------------------*/

int tas_filter (TASET *taset, const char *marks)
{                               /* --- filter items in a trans. set */
  int   i, max = 0;             /* loop variable, max. num. of items */
  TRACT *t;                     /* to traverse the transactions */

  assert(taset && marks);       /* check the function arguments */
  taset->total = 0;             /* clear the total number of items */
  for (i = taset->cnt; --i >= 0; ) {
    t = taset->tracts[i];       /* traverse the transactions */
    t->cnt = ta_filter(t->items, t->cnt, marks);
    if (t->cnt > max) max = t->cnt;
    taset->total += t->cnt;     /* filter each transaction and */
  }                             /* update maximal size and total */
  return max;                   /* return maximum number of items */
}  /* tas_filter() */

/*--------------------------------------------------------------------*/

void tas_sort (TASET *taset, int heap)
{                               /* --- sort a transaction set */
  assert(taset);                /* check the function argument */
  if (heap) v_heapsort(taset->tracts, taset->cnt, ta_cmp, NULL);
  else      v_sort    (taset->tracts, taset->cnt, ta_cmp, NULL);
}  /* tas_sort() */

/*--------------------------------------------------------------------*/

int tas_occur (TASET *taset, const int *items, int n)
{                               /* --- count transaction occurrences */
  int l, r, m, k = taset->cnt;  /* index variables */

  assert(taset && items);       /* check the function arguments */
  for (r = m = 0; r < k; ) {    /* find right boundary */
    m = (r + k) >> 1;           /* by a binary search */
    if (ta_cmpx(taset->tracts[m], items, n) > 0) k = m;
    else                                         r = m+1;
  }
  for (l = m = 0; l < k; ) {    /* find left boundary */
    m = (l + k) >> 1;           /* by a binary search */
    if (ta_cmpx(taset->tracts[m], items, n) < 0) l = m+1;
    else                                         k = m;
  }
  return r -l;                  /* compute the number of occurrences */
}  /* tas_occur() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void tas_show (TASET *taset)
{                               /* --- show a transaction set */
  int   i, k;                   /* loop variables */
  TRACT *t;                     /* to traverse the transactions */

  assert(taset);                /* check the function argument */
  for (i = 0; i < taset->cnt; i++) {
    t = taset->tracts[i];       /* traverse the transactions */
    for (k = 0; k < t->cnt; k++) {  /* traverse the items */
      if (k > 0) putc(' ', stdout); /* print a separator */
      printf(is_name(taset->itemset, t->items[k]));
    }                           /* print the next item */
    putc('\n', stdout);         /* terminate the transaction */
  }                             /* finally print the number of t.a. */
  printf("%d transaction(s)\n", taset->cnt);
}  /* tas_show() */

#endif
/*----------------------------------------------------------------------
  Transaction Tree Functions
----------------------------------------------------------------------*/

TATREE* _create (TRACT **tracts, int cnt, int index)
{                               /* --- recursive part of tat_create() */
  int    i, k, t;               /* loop variables, buffer */
  int    item, n;               /* item and item counter */
  TATREE *tat;                  /* created transaction tree */
  TATREE **vec;                 /* vector of child pointers */

  assert(tracts                 /* check the function arguments */
     && (cnt >= 0) && (index >= 0));
  if (cnt <= 1) {               /* if only one transaction left */
    n   = (cnt > 0) ? (*tracts)->cnt -index : 0;
    tat = (TATREE*)malloc(sizeof(TATREE) +(n-1) *sizeof(int));
    if (!tat) return NULL;      /* create a transaction tree node */
    tat->cnt  = cnt;            /* and initialize its fields */
    tat->size = -n;
    tat->max  =  n;
    while (--n >= 0) tat->items[n] = (*tracts)->items[index +n];
    return tat;
  }
  for (k = cnt; (--k >= 0) && ((*tracts)->cnt <= index); )
    tracts++;                   /* skip t.a. that are too short */
  n = 0; item = -1;             /* init. item and item counter */
  for (tracts += i = ++k; --i >= 0; ) {
    t = (*--tracts)->items[index]; /* traverse the transactions */
    if (t != item) { item = t; n++; }
  }                             /* count the different items */
  #ifdef ARCH64                 /* adapt to even item number */
  i = (n & 1) ? n : (n+1);      /* so that pointer addresses are */
  #else                         /* multiples of 8 on 64 bit systems */
  i = n;                        /* on 32 bit systems, however, */
  #endif                        /* use the exact number of items */
  tat = (TATREE*)malloc(sizeof(TATREE) + (i-1) *sizeof(int)
                                       + n     *sizeof(TATREE*));
  if (!tat) return NULL;        /* create a transaction tree node */
  tat->cnt  = cnt;              /* and initialize its fields */
  tat->size = n;
  tat->max  = 0;
  if (n <= 0) return tat;       /* if t.a. are fully captured, abort */
  vec  = (TATREE**)(tat->items +i);
  item = tracts[--k]->items[index];
  for (tracts += i = k; --i >= 0; ) {
    t = (*--tracts)->items[index];     /* traverse the transactions, */
    if (t == item) continue;    /* but skip those with the same item */
    tat->items[--n] = item; item = t;
    vec[n] = _create(tracts+1, k-i, index+1);
    if (!vec[n]) break;         /* note the item identifier */
    t = vec[n]->max +1; if (t > tat->max) tat->max = t;
    k = i;                      /* recursively create subtrees */
  }                             /* and adapt the section end index */
  if (i < 0) {                  /* if child creation was successful */
    tat->items[--n] = item;     /* note the last item identifier */
    vec[n] = _create(tracts, k+1, index+1);
    if (vec[n]) {               /* create the last child */
      t = vec[n]->max +1; if (t > tat->max) tat->max = t;
      return tat;               /* return the created */
    }                           /* transaction tree */
  }                             
  for (i = tat->size; --i > n; ) tat_delete(vec[i]);
  free(tat);                    /* on error delete created subtrees */
  return NULL;                  /* and the transaction tree node */
}  /* _create() */

/*--------------------------------------------------------------------*/

TATREE* tat_create (TASET *taset, int heap)
{                               /* --- create a transactions tree */
  assert(taset);                /* check the function argument */
  if (heap) v_heapsort(taset->tracts, taset->cnt, ta_cmp, NULL);
  else      v_sort    (taset->tracts, taset->cnt, ta_cmp, NULL);
  return _create(taset->tracts, taset->cnt, 0);
}  /* tat_create() */

/*--------------------------------------------------------------------*/

void tat_delete (TATREE *tat)
{                               /* --- delete a transaction tree */
  int    i;                     /* loop variable */
  TATREE **vec;                 /* vector of child nodes */

  assert(tat);                  /* check the function argument */
  #ifdef ARCH64                 /* if 64 bit architecture */
  i = (tat->size & 1) ? tat->size : (tat->size+1);
  #else                         /* address must be a multiple of 8 */
  i = tat->size;                /* on 32 bit systems, however, */
  #endif                        /* use the number of items directly */
  vec = (TATREE**)(tat->items +i);
  for (i = tat->size; --i >= 0; )
    tat_delete(vec[i]);         /* recursively delete the subtrees */
  free(tat);                    /* and the tree node itself */
}  /* tat_delete() */

/*--------------------------------------------------------------------*/
#ifdef ARCH64

TATREE* tat_child (TATREE *tat, int index)
{                               /* --- go to a child node */
  int s;                        /* padded size of the node */

  assert(tat                    /* check the function arguments */
     && (index >= 0) && (index < tat->size));
  s = (tat->size & 1) ? tat->size : (tat->size +1);
  return ((TATREE**)(tat->items +s))[index];
}  /* tat_child */              /* return the child node/subtree */

#endif
/*--------------------------------------------------------------------*/

void tat_mark (TATREE *tat)
{                               /* --- mark end of transactions */
  int i;                        /* loop variable */

  assert(tat);                  /* check the function argument */
  if      (tat->size < 0)       /* if there is a transaction, */
    tat->items[tat->max-1] |= INT_MIN;  /* mark end of trans. */
  else {                        /* if there are subtrees */
    for (i = tat->size; --i >= 0; )
      tat_mark(tat_child(tat, i));
  }                             /* recursively mark the subtrees */
}  /* tat_mark() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

void _show (TATREE *tat, int ind)
{                               /* --- rekursive part of tat_show() */
  int    i, k;                  /* loop variables */
  TATREE **vec;                 /* vector of child nodes */

  assert(tat && (ind >= 0));    /* check the function arguments */
  if (tat->size <= 0) {         /* if this is a leaf node */
    for (i = 0; i < tat->max; i++)
      printf("%d ", tat->items[i] & ~INT_MIN);
    printf("\n"); return;       /* print the items in the */
  }                             /* (rest of) the transaction */
  vec = (TATREE**)(tat->items +tat->size);
  for (i = 0; i < tat->size; i++) {
    if (i > 0) for (k = ind; --k >= 0; ) printf("  ");
    printf("%d ", tat->items[i]);
    _show(vec[i], ind+1);       /* traverse the items, print them, */
  }                             /* and show the children recursively */
}  /* _show() */

/*--------------------------------------------------------------------*/

void tat_show (TATREE *tat)
{                               /* --- show a transaction tree */
  assert(tat);                  /* check the function argument */
  _show(tat, 0);                /* just call the recursive function */
}  /* tat_show() */

#endif
