/*----------------------------------------------------------------------
  File    : istree.c
  Contents: item set tree management
  Author  : Christian Borgelt
  History : 1996.01.22 file created
            1996.02.07 _child, _count, ist_addlvl, and ist_count
            1996.02.09 ist_rule programmed and debugged
            1996.02.10 empty rule bodies made optional
            1996.03.28 support made relative to number of item sets
            1996.06.25 function _count optimized
            1996.11.23 rule extraction redesigned
            1996.11.24 rule selection criteria added
            1997.08.18 normalized chi^2 measure added
                       parameter minlen added to function ist_init()
            1998.01.15 confidence comparison changed to >=
            1998.01.23 integer support computation changed (ceil)
            1998.01.26 condition added to set extension in _child
            1998.02.10 bug in computation of EM_INFO fixed
            1998.02.11 parameter 'minval' added to function ist_init()
            1998.05.14 item set tree navigation functions added
            1998.08.08 item appearances considered for rule selection
            1998.08.20 deferred child node vector allocation added
            1998.09.02 several assertions added
            1998.09.05 bug concerning node id fixed
            1998.09.07 function ist_hedge added
            1998.09.22 bug in rule extraction (item appearances) fixed
            1998.09.23 computation of chi^2 measure simplified
            1999.02.05 long int changed to int
            1999.08.25 rule extraction simplified
            1999.11.05 rule evaluation measure EM_AIMP added
            1999.11.08 parameter 'aval' added to function ist_rule
            1999.11.11 rule consequents moved to first field
            1999.12.01 bug in node reallocation fixed
            2001.04.01 functions ist_set and ist_getcntx added,
                       functions _count and _getsupp improved
            2001.12.28 sort function moved to module tract
            2002.02.07 tree clearing removed, counting improved
            2002.02.08 child creation improved (check of body support)
            2002.02.10 IST_IGNORE bugs fixed (ist_set and ist_hedge)
            2002.02.11 memory usage minimization option added
            2002.02.12 ist_first and ist_last replaced by ist_next
            2002.02.19 transaction tree functions added
            2002.10.09 bug in function ist_hedge fixed (conf. comp.)
            2003.03.12 parameter lift added to function ist_rule
            2003.07.17 check of item usage added (function ist_check)
            2003.07.18 maximally frequent item set filter added
            2003.08.11 item set filtering generalized (ist_filter)
            2003.08.15 renamed new to cur in ist_addlvl (C++ compat.)
            2003.11.14 definition of F_HDONLY changed to INT_MIN
            2003.12.02 skipping unnecessary subtrees added (_checksub)
            2003.12.03 bug in ist_check for rule mining fixed
            2003.12.12 padding for 64 bit architecture added
            2004.05.09 additional selection measure for sets added
            2004.12.09 bug in add. evaluation measure for sets fixed
            2006.11.26 support parameter changed to an absolute value
            2007.02.07 bug in function ist_addlvl / _child fixed
            2008.01.25 bug in filtering closed/maximal item sets fixed
            2008.03.13 additional rule evaluation redesigned
            2008.03.24 creation based on ITEMSET structure
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include "istree.h"
#include "chi2.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define LN_2       0.69314718055994530942   /* ln(2) */
#define EPSILON    1e-12        /* to cope with roundoff errors */
#define BLKSIZE    32           /* block size for level vector */
#define F_HDONLY   INT_MIN      /* flag for head only item in path */
#define F_SKIP     INT_MIN      /* flag for subtree skipping */
#define ID(n)      ((int)((n)->id & ~F_HDONLY))
#define HDONLY(n)  ((int)((n)->id &  F_HDONLY))
#define COUNT(n)   ((n) & ~F_SKIP)

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef double EVALFN (int set, int body, int head, int n);
/* function to compute an additional evaluation measure */

/*----------------------------------------------------------------------
  Auxiliary Functions
----------------------------------------------------------------------*/

static int _bsearch (int *vec, int n, int id)
{                               /* --- binary search for an item */
  int i, k;                     /* left and middle index */

  assert(vec && (n > 0));       /* check the function arguments */
  for (i = 0; i < n; ) {        /* while the range is not empty */
    k = (i + n) >> 1;           /* get index of middle element */
    if      (vec[k] > id) n = k;
    else if (vec[k] < id) i = k+1;
    else return k;              /* adapt range boundaries or return */
  }                             /* the index the id. was found at */
  return -1;                    /* return 'not found' */
}  /* _bsearch() */

/*--------------------------------------------------------------------*/

static void _count (ISNODE *node, int *set, int cnt, int min)
{                               /* --- count transaction recursively */
  int    i;                     /* vector index */
  int    *map, n;               /* identifier map and its size */
  ISNODE **vec;                 /* child node vector */

  assert(node                   /* check the function arguments */
      && (cnt >= 0) && (set || (cnt <= 0)));
  if (node->offset >= 0) {      /* if a pure vector is used */
    if (node->chcnt == 0) {     /* if this is a new node */
      n = node->offset;         /* get the index offset */
      while ((cnt > 0) && (*set < n)) {
        cnt--; set++; }         /* skip items before first counter */
      while (--cnt >= 0) {      /* traverse the transaction's items */
        i = *set++ -n;          /* compute counter vector index */
        if (i >= node->size) return;
        node->cnts[i]++;        /* if the counter exists, */
      } }                       /* count the transaction */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(node->cnts +node->size);
      n   = ID(vec[0]);         /* get the child node vector */
      min--;                    /* one item less to the deepest nodes */
      while ((cnt > min) && (*set < n)) {
        cnt--; set++; }         /* skip items before first child */
      while (--cnt >= min) {    /* traverse the transaction's items */
        i = *set++ -n;          /* compute child vector index */
        if (i >= node->chcnt) return;
        if (vec[i]) _count(vec[i], set, cnt, min);
      }                         /* if the child exists, */
    } }                         /* count the transaction recursively */
  else {                        /* if an identifer map is used */
    map = node->cnts +(n = node->size);
    if (node->chcnt == 0) {     /* if this is a new node */
      while (--cnt >= 0) {      /* traverse the transaction's items */
        if (*set > map[n-1]) return;  /* if beyond last item, abort */
        i = _bsearch(map, n, *set++);
        if (i >= 0) node->cnts[i]++;
      } }                       /* find index and count transaction */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(map +n); /* get id. map and child vector */
      if (node->chcnt < n)      /* if a secondary id. map exists */
        map = (int*)(vec +(n = node->chcnt));
      min--;                    /* one item less to the deepest nodes */
      while (--cnt >= min) {    /* traverse the transaction's items */
        if (*set > map[n-1]) return;  /* if beyond last item, abort */
        i = _bsearch(map, n, *set++);
        if ((i >= 0) && vec[i]) _count(vec[i], set, cnt, min);
      }                         /* search for the proper index */
    }                           /* and if the child exists, */
  }                             /* count the transaction recursively */
}  /* _count() */

/*--------------------------------------------------------------------*/

static void _countx (ISNODE *node, TATREE *tat, int min)
{                               /* --- count transa. tree recursively */
  int    i, k;                  /* vector index, loop variable */
  int    *map, n;               /* identifier map and its size */
  ISNODE **vec;                 /* child node vector */

  assert(node && tat);          /* check the function arguments */
  if (tat_max(tat) < min)       /* if the transactions are too short, */
    return;                     /* abort the recursion */
  k = tat_size(tat);            /* get the number of children */
  if (k <= 0) {                 /* if there are no children */
    if (k < 0) _count(node, tat_items(tat), -k, min);
    return;                     /* count the normal transaction */
  }                             /* and abort the function */
  while (--k >= 0)              /* count the transactions recursively */
    _countx(node, tat_child(tat, k), min);
  if (node->offset >= 0) {      /* if a pure vector is used */
    if (node->chcnt == 0) {     /* if this is a new node */
      n = node->offset;         /* get the index offset */
      for (k = tat_size(tat); --k >= 0; ) {
        i = tat_item(tat,k) -n; /* traverse the items */
        if (i < 0) return;      /* if before first item, abort */
        if (i < node->size)     /* if inside the counter range */
          node->cnts[i] += tat_cnt(tat_child(tat, k));
      } }                       /* count the transaction */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(node->cnts +node->size);
      n   = ID(vec[0]);         /* get the child node vector */
      min--;                    /* one item less to the deepest nodes */
      for (k = tat_size(tat); --k >= 0; ) {
        i = tat_item(tat,k) -n; /* traverse the items */
        if (i < 0) return;      /* if before first item, abort */
        if ((i < node->chcnt) && vec[i])
          _countx(vec[i], tat_child(tat, k), min);
      }                         /* if the child exists, */
    } }                         /* count the transaction recursively */
  else {                        /* if an identifer map is used */
    map = node->cnts +(n = node->size);
    if (node->chcnt == 0) {     /* if this is a new node */
      for (k = tat_size(tat); --k >= 0; ) {
        i = tat_item(tat, k);   /* get the next item */
        if (i < map[0]) return; /* if before first item, abort */
        i = _bsearch(map, n, i);
        if (i >= 0) node->cnts[i] += tat_cnt(tat_child(tat, k));
      } }                       /* find index and count transaction */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(map +n); /* get id. map and child vector */
      if (node->chcnt < n)      /* if a secondary id. map exists */
        map = (int*)(vec +(n = node->chcnt));
      min--;                    /* one item less to the deepest nodes */
      for (k = tat_size(tat); --k >= 0; ) {
        i = tat_item(tat, k);   /* get the next item */
        if (i < map[0]) return; /* if before first item, abort */
        i = _bsearch(map, n, i);
        if ((i >= 0) && vec[i]) _countx(vec[i], tat_child(tat, k), min);
      }                         /* search for the proper index */
    }                           /* and if the child exists, */
  }                             /* count the transaction recursively */
}  /* _countx() */

/*--------------------------------------------------------------------*/

static int _checksub (ISNODE *node)
{                               /* --- recursively check subtrees */
  int    i, r;                  /* vector index, result */
  ISNODE **vec;                 /* child node vector */

  assert(node);                 /* check the function argument */
  if (node->chcnt  == 0) return  0;  /* do not skip new leaves */
  if (node->chcnt  <  0) return -1;  /* skip marked subtrees */
  if (node->offset >= 0)        /* if a pure vector is used */
    vec = (ISNODE**)(node->cnts +node->size);
  else                          /* if an identifer map is used */
    vec = (ISNODE**)(node->cnts +node->size +node->size);
  for (r = -1, i = node->chcnt; --i >= 0; )
    if (vec[i]) r &= _checksub(vec[i]);
  if (!r) return 0;             /* recursively check all children */
  node->chcnt |= F_SKIP;        /* set the skip flag if possible */
  return -1;                    /* return 'subtree can be skipped' */
}  /* _checksub() */

/*--------------------------------------------------------------------*/

static int _checkuse (ISNODE *node, char *marks, int supp)
{                               /* --- recursively check item usage */
  int    i, r = 0;              /* vector index, result of check */
  int    *map, n;               /* identifier map and its size */
  ISNODE **vec;                 /* child node vector */

  assert(node && marks);        /* check the function arguments */
  if (node->offset >= 0) {      /* if a pure vector is used */
    if (node->chcnt == 0) {     /* if this is a new node */
      n = node->offset;         /* get the index offset */
      for (i = node->size; --i >= 0; ) {
        if (node->cnts[i] >= supp)
          marks[n+i] = r = 1;   /* mark items in set that satisfies */
      } }                       /* the minimum support criterion */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(node->cnts +node->size);
      for (i = node->chcnt; --i >= 0; )
        if (vec[i]) r |= _checkuse(vec[i], marks, supp);
    } }                         /* recursively process all children */
  else {                        /* if an identifer map is used */
    map = node->cnts +node->size;
    if (node->chcnt == 0) {     /* if this is a new node */
      for (i = node->size; --i >= 0; ) {
        if (node->cnts[i] >= supp)
          marks[map[i]] = r = 1;/* mark items in set that satisfies */
      } }                       /* the minimum support criterion */
    else if (node->chcnt > 0) { /* if there are child nodes */
      vec = (ISNODE**)(map +node->size);
      for (i = node->chcnt; --i >= 0; )
        if (vec[i]) r |= _checkuse(vec[i], marks, supp);
    }                           /* get the child vector and */
  }                             /* recursively process all children */
  if ((r != 0) && node->parent) /* if the check succeeded, mark */
    marks[ID(node)] = 1;        /* the item associated with the node */
  return r;                     /* return the check result */
}  /* _checkuse() */

/*--------------------------------------------------------------------*/

static int _getsupp (ISNODE *node, int *set, int cnt)
{                               /* --- get support of an item set */
  int    i, n, c;               /* vector index, buffers */
  int    *map;                  /* identifier map */
  ISNODE **vec;                 /* vector of child nodes */

  assert(node && set && (cnt >= 0)); /* check the function arguments */
  while (--cnt > 0) {           /* follow the set/path from the node */
    c = node->chcnt & ~F_SKIP;  /* if there are no children, */
    if (c <= 0) return -1;      /* the support is less than minsupp */
    if (node->offset >= 0) {    /* if a pure vector is used */
      vec = (ISNODE**)(node->cnts +node->size);
      i   = *set++ -ID(vec[0]); /* compute the child vector index and */
      if (i >= c) return -1; }  /* abort if the child does not exist */
    else {                      /* if an identifier map is used */
      map = node->cnts +(n = node->size);
      vec = (ISNODE**)(map +n); /* get id. map and child vector */
      if (c < n)                /* if a secondary id. map exists, */
        map = (int*)(vec +(n = c));    /* get this identifier map */
      i = _bsearch(map, n, *set++);
    }                           /* search for the proper index */
    if (i < 0) return -1;       /* abort if index is out of range */
    node = vec[i];              /* go to the corresponding child */
    if (!node) return -1;       /* if the child does not exists, */
  }                             /* the support is less than minsupp */
  if (node->offset >= 0) {      /* if a pure vector is used, */
    i = *set -node->offset;     /* compute the counter index */
    if (i >= node->size) return -1; }
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    i   = _bsearch(map, n, *set);
  }                             /* search for the proper index */
  if (i < 0) return -1;         /* abort if index is out of range */
  return node->cnts[i];         /* return the item set support */
}  /* _getsupp() */

/*--------------------------------------------------------------------*/

static void _marksupp (ISNODE *node, int *set, int cnt, int supp)
{                               /* --- mark support of an item set */
  int    i, n, c;               /* vector index, buffers */
  int    *map;                  /* identifier map */
  ISNODE **vec;                 /* vector of child nodes */

  assert(node && set && (cnt >= 0)); /* check the function arguments */
  while (--cnt > 0) {           /* follow the set/path from the node */
    if (node->offset >= 0) {    /* if a pure vector is used */
      vec = (ISNODE**)(node->cnts +node->size);
      i   = *set++ -ID(vec[0]);}/* compute the child vector index */
    else {                      /* if an identifier map is used */
      map = node->cnts +(n = node->size);
      vec = (ISNODE**)(map +n); /* get id. map, child vector and */
      c   = node->chcnt & ~F_SKIP;     /* the number of children */
      if (c < n)                /* if a secondary id. map exists, */
        map = (int*)(vec +(n = c));    /* get this identifier map */
      i = _bsearch(map, n, *set++);
    }                           /* search for the proper index */
    node = vec[i];              /* go to the corresponding child */
  }
  if (node->offset >= 0)        /* if a pure vector is used, */
    i = *set -node->offset;     /* compute the counter index */
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    i   = _bsearch(map, n, *set);
  }                             /* search for the proper index */
  if ((supp < 0)                /* if to clear unconditionally */
  ||  (node->cnts[i] == supp))  /* or the support is the same */
    node->cnts[i] |= F_SKIP;    /* mark support as cleared */
}  /* _marksupp() */

/*--------------------------------------------------------------------*/

static void _marksub (ISTREE *ist, ISNODE *node, int index, int supp)
{                               /* --- mark all n-1 subsets */
  int i;                        /* next item, loop variable */
  int *set;                     /* (partial) item set */

  if (node->offset >= 0) i = node->offset +index;
  else                   i = node->cnts[node->size +index];
  set = ist->buf +ist->vsz;     /* get and store the first two items */
  *--set = i;        _marksupp(node->parent, set, 1, supp);
  *--set = ID(node); _marksupp(node->parent, set, 1, supp);
  i = 2;                        /* mark counters in parent node */
  for (node = node->parent; node->parent; node = node->parent) {
    _marksupp(node->parent, set, i, supp);
    *--set = ID(node); i++;     /* climb up the tree and mark */
  }                             /* counters for all n-1 subsets */
}  /* _marksub() */

/*--------------------------------------------------------------------*/

static ISNODE* _child (ISTREE *ist, ISNODE *node, int index,
                       int s_min, int s_body)
{                               /* --- create child node (extend set) */
  int    i, k, n;               /* loop variables, counters */
  ISNODE *curr;                 /* to traverse the path to the root */
  int    item, cnt;             /* item identifier, number of items */
  int    *set;                  /* next (partial) item set to check */
  int    body;                  /* enough support for a rule body */
  int    hdonly;                /* whether head only item on path */
  int    app;                   /* appearance flags of an item */
  int    s_set;                 /* support of an item set */

  assert(ist && node            /* check the function arguments */
     && (index >= 0) && (index < node->size));
  if (node->offset >= 0) item = node->offset +index;
  else                   item = node->cnts[node->size +index];
  app = is_getapp(ist->set, item);   /* get item id. and app. flag */
  if ((app == IST_IGNORE)       /* do not extend an item to ignore */
  ||  ((HDONLY(node) && (app == IST_HEAD))))
    return NULL;                /* nor a set with two head only items */
  hdonly = HDONLY(node) || (app == IST_HEAD);

  /* --- initialize --- */
  s_set = node->cnts[index];    /* get support of item set to extend */
  if (s_set < s_min)            /* if set support is insufficient, */
    return NULL;                /* no child is needed, so abort */
  body = (s_set >= s_body)      /* if the set has enough support for */
       ? 1 : 0;                 /* a rule body, set the body flag */
  ist->buf[ist->vsz -2] = item; /* init. set for support checks */

  /* --- check candidates --- */
  for (n = 0, i = index; ++i < node->size; ) {
    if (node->offset >= 0) k = node->offset +i;
    else                   k = node->cnts[node->size +i];
    app = is_getapp(ist->set, k);  /* traverse the candidate items */
    if ((app == IST_IGNORE) || (hdonly && (app == IST_HEAD)))
      continue;                 /* skip sets with two head only items */
    s_set = node->cnts[i];      /* traverse the candidate items */
    if (s_set <  s_min)         /* if set support is insufficient, */
      continue;                 /* ignore the corresponding candidate */
    body &= 1;                  /* restrict body flags to the set S */
    if (s_set >= s_body)        /* if set support is sufficient for */
      body |= 2;                /* a rule body, set the body flag */ 
    set    = ist->buf +ist->vsz -(cnt = 2);
    set[1] = k;                 /* add the candidate item to the set */
    for (curr = node; curr->parent; curr = curr->parent) {
      s_set = _getsupp(curr->parent, set, cnt);
      if (s_set <  s_min)       /* get the item set support and */
        break;                  /* if it is too low, abort the loop */
      if (s_set >= s_body)      /* if some subset has enough support */
        body |= 4;              /* for a rule body, set the body flag */
      *--set = ID(curr); cnt++; /* add id of current node to the set */
    }                           /* and adapt the number of items */
    if (!curr->parent && body)  /* if subset support is high enough */
      ist->map[n++] = k;        /* for a full rule and a rule body, */
  }                             /* note the item identifier */
  if (n <= 0) return NULL;      /* if no child is needed, abort */
  #ifdef BENCH                  /* if benchmark version: */
  ist->scnec += n;              /* sum the necessary counters */
  #endif

  /* --- decide on node structure --- */
  k = ist->map[n-1] -ist->map[0] +1;
  if (!(ist->mode & IST_MEMOPT)) n = k;
  else if (3*n >= 2*k) n = k;   /* use a pure vector if it is small */
  else                 k = n+n; /* enough, otherwise use an id. map */
  #ifdef ARCH64                 /* if 64 bit architecture */
  if ((n == k) && (k & 1)) n = ++k;
  #endif                        /* pad to even number of counters */
  #ifdef BENCH                  /* if benchmark version */
  ist->sccnt += n;              /* sum the number of counters */
  ist->bytes += sizeof(ISNODE) +(k-1) *sizeof(int) +8;
  #endif                        /* determine the memory usage */

  /* --- create child --- */
  curr = (ISNODE*)malloc(sizeof(ISNODE) +(k-1) *sizeof(int));
  if (!curr) return (void*)-1;  /* create a child node */
  curr->parent = node;          /* set pointer to parent node */
  curr->succ   = NULL;          /* and clear successor pointer */
  curr->id     = item;          /* initialize the item id. and */
  if (hdonly) curr->id |= F_HDONLY;  /* set the head only flag */
  curr->chcnt  = 0;             /* there are no children yet */
  curr->size   = n;             /* set size of counter vector */
  if (n == k)                   /* if to use a pure vector, */
    curr->offset = ist->map[0]; /* note the first item as an offset */
  else {                        /* if to use an identifier map, */
    curr->offset = -1;          /* use the offset as an indicator */
    for (set = curr->cnts +n +(i = n); --i >= 0; )
      *--set = ist->map[i];     /* copy the identifier map */
  }                             /* from the buffer to the node */
  for (set = curr->cnts +(i = n); --i >= 0; )
    *--set = 0;                 /* clear all counters of the node */
  return curr;                  /* return pointer to created child */
}  /* _child() */

/*----------------------------------------------------------------------
  In the above function the set S represented by the index-th vector
element of the current node is extended only by combining it with the
sets represented by the fields that follow it in the node vector,
i.e. by the sets represented by vec[index+1] to vec[size-1]. The sets
that can be formed by combining the set S and the sets represented by
vec[0] to vec[index-1] are processed in the branches for these sets.
  In the 'check candidates' loop it is checked for each set represented
by vec[index+1] to vec[size-1] whether this set and all other subsets
of the same size, which can be formed from the union of this set and
the set S, have enough support, so that a child node is necessary.
  Note that i +offset is the identifier of the item that has to be
added to set S to form the union of the set S and the set T represented
by vec[i], since S and T have the same path with the exception of the
index in the current node. Hence we can speak of candidate items that
are added to S.
  Checking the support of the other subsets of the union of S and T
that have the same size as S and T is done with the aid of a path
variable. The items in this variable combined with the items on the
path to the current node always represent the subset currently tested.
That is, the path variable holds the path to be followed from the
current node to arrive at the support counter for the subset. The path
variable is initialized to [0]: <item>, [1]: <offset+i>, since the
support counters for S and T can be inspected directly. Then this
path is followed from the parent node of the current node, which is
equivalent to checking the subset that can be obtained by removing
from the union of S and T the item that corresponds to the parent node
(in the path to S or T, resp.).
  Iteratively making the parent node the current node, adding its
corresponding item to the path and checking the support counter at the
end of the path variable when starting from its (the new current node's)
parent node tests all other subsets.
  Another criterion is that the extended set must not contain two items
which may appear only in the head of a rule. If two such items are
contained in a set, neither can a rule be formed from its items nor can
it be the antecedent of a rule. Whether a set contains two head only
items is determined from the nodes 'hdonly' flag and the appearance
flags of the items.
----------------------------------------------------------------------*/

static void _cleanup (ISTREE *ist)
{                               /* --- clean up on error */
  ISNODE *node, *t;             /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  for (node = ist->lvls[ist->height]; node; ) {
    t = node; node = node->succ; free(t); }
  ist->lvls[ist->height] = NULL;/* delete all created nodes */
  for (node = ist->lvls[ist->height -1]; node; node = node->succ)
    node->chcnt = 0;            /* clear the child node counters */
}  /* _cleanup() */             /* of the deepest nodes in the tree */

/*----------------------------------------------------------------------
  Additional Rule Evaluation Measure Functions
----------------------------------------------------------------------*/

static double _none (int set, int body, int head, int n)
{ return 1; }                   /* --- no add. evaluation measure */

/*--------------------------------------------------------------------*/

static double _diff (int set, int body, int head, int n)
{                               /* --- absolute confidence difference */
  return fabs(head/(double)n -set/(double)body);
}  /* _diff() */

/*--------------------------------------------------------------------*/

static double _quot (int set, int body, int head, int n)
{                               /* --- diff. of conf. quotient to 1 */
  double t;                     /* temporary buffer */

  if ((head <= 0) || (body <= 0)) return 0;
  t = (set/(double)body) /(head/(double)n);
  return 1 -((t > 1) ? 1/t : t);  /* return the confidence quotient */
}  /* _quot() */

/*--------------------------------------------------------------------*/

static double _aimp (int set, int body, int head, int n)
{                               /* --- abs. diff. of improvement to 1 */
  if ((head <= 0) || (body <= 0)) return 0;
  return fabs((set/(double)body) /(head/(double)n) -1);
}  /* _aimp() */

/*--------------------------------------------------------------------*/

static double _info (int set, int body, int head, int n)
{                               /* --- information diff. to prior */
  double sum, t;                /* result, temporary buffer */

  if ((head <= 0) || (head >= n)
  ||  (body <= 0) || (body >= n))
    return 0;                   /* check for strict positivity */
  sum = 0;                      /* support of     head and     body */
  if (set > 0) sum += set *log(set /(   head  *(double)   body));
  t = body -set;                /* support of not head and     body */
  if (t   > 0) sum += t   *log(t   /((n-head) *(double)   body));
  t = head -set;                /* support of     head and not body */
  if (t   > 0) sum += t   *log(t   /(   head  *(double)(n-body)));
  t = n -head -body +set;       /* support of not head and not body */
  if (t   > 0) sum += t   *log(t   /((n-head) *(double)(n-body)));
  return (log(n) +sum/n) /LN_2; /* return information gain in bits */
}  /* _info() */

/*--------------------------------------------------------------------*/

static double _chi2 (int set, int body, int head, int n)
{                               /* --- normalized chi^2 measure */
  double t;                     /* temporary buffer */

  if ((head <= 0) || (head >= n)
  ||  (body <= 0) || (body >= n))
    return 0;                   /* check for strict positivity */
  t = head *(double)body -set *(double)n;
  return (t*t) / (((double)head) *(n-head) *body *(n-body));
}  /* _chi2() */                /* compute and return chi^2 measure */

/*--------------------------------------------------------------------*/

static double _pval (int set, int body, int head, int n)
{                               /* --- p-value from chi^2 measure */
  return chi2cdf(n*_chi2(set, body, head, n), 1);
}  /* _pval() */

/*--------------------------------------------------------------------*/

static EVALFN *_evalfns[EM_UNKNOWN] = {
  /* EM_NONE  0 */  _none,      /* no additional evaluation measure */
  /* EM_DIFF  1 */  _diff,      /* absolute confidence difference */
  /* EM_QUOT  2 */  _quot,      /* difference of conf. quotient to 1 */
  /* EM_AIMP  3 */  _aimp,      /* abs. diff. of improvement to 1 */
  /* EM_INFO  4 */  _info,      /* information difference to prior */
  /* EM_CHI2  5 */  _chi2,      /* normalized chi^2 measure */
  /* EM_PVAL  6 */  _pval,      /* p-value of chi^2 measure */
};                              /* table of evaluation functions */

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

ISTREE* ist_create (ITEMSET *set, int mode, int supp, double conf)
{                               /* --- create an item set tree */
  int    cnt, n;                /* number of items, buffer */
  ISTREE *ist;                  /* created item set tree */
  ISNODE *root;                 /* root node of the tree */

  assert(set                    /* check the function arguments */
     && (supp >= 0) && (conf >= 0) && (conf <= 1));

  /* --- allocate memory --- */ 
  cnt = is_cnt(set);            /* get the number of items */
  ist = (ISTREE*)malloc(sizeof(ISTREE));
  if (!ist) return NULL;        /* allocate the tree body */
  ist->lvls = (ISNODE**)malloc(BLKSIZE *sizeof(ISNODE*));
  if (!ist->lvls) {                  free(ist); return NULL; }
  ist->buf  = (int*)    malloc(BLKSIZE *sizeof(int));
  if (!ist->buf)  { free(ist->lvls); free(ist); return NULL; }
  ist->map  = (int*)    malloc(cnt *sizeof(int));
  if (!ist->map)  { free(ist->buf);
                    free(ist->lvls); free(ist); return NULL; }
  #ifdef ARCH64                 /* if 64 bit architecture, */
  n = cnt +(cnt & 1);           /* pad counters to even number */
  #else                         /* on 32 bit systems, however, */
  n = cnt;                      /* use the number of items directly */
  #endif
  ist->lvls[0] = ist->curr =    /* allocate a root node */
  root = (ISNODE*)calloc(1, sizeof(ISNODE) +(n-1) *sizeof(int));
  if (!root)        { free(ist->map);  free(ist->buf);
                      free(ist->lvls); free(ist); return NULL; }

  /* --- initialize structures --- */
  ist->set     = set;           /* copy parameters to the structure */
  ist->mode    = mode;
  ist->tacnt   = is_gettac(set);
  ist->vsz     = BLKSIZE;
  ist->height  = 1;
  ist->rule    = (supp > 0) ? supp : 1;
  if (mode & IST_HEAD) supp = (int)ceil(conf *supp);
  ist->supp    = (supp > 0) ? supp : 1;
  ist->conf    = conf;
  #ifdef BENCH                  /* if benchmark version */
  ist->sccnt   = ist->scnec = cnt;
  ist->cpcnt   = ist->cpnec = 0;
  ist->bytes   = sizeof(ISTREE) +cnt *sizeof(char) +8
               + BLKSIZE *sizeof(ISNODE*) +8
               + BLKSIZE *sizeof(int) +8
               + cnt     *sizeof(int) +8;
  #endif                        /* initialize the benchmark variables */
  ist_init(ist, 1, EM_NONE, 1); /* initialize rule extraction */
  root->parent = root->succ = NULL;
  root->offset = root->id   = 0;
  root->chcnt  = 0;             /* initialize the root node */
  root->size   = n;
  while (--cnt >= 0)            /* copy the item frequencies */
    root->cnts[cnt] = is_getfrq(set, cnt); 
  return ist;                   /* return created item set tree */
}  /* ist_create() */

/*--------------------------------------------------------------------*/

void ist_delete (ISTREE *ist)
{                               /* --- delete an item set tree */
  int    i;                     /* loop variables */
  ISNODE *node, *t;             /* to traverse the nodes */

  assert(ist);                  /* check the function argument */
  for (i = ist->height; --i >= 0; ) {
    for (node = ist->lvls[i]; node; ) {
      t = node; node = node->succ; free(t); }
  }                             /* delete all nodes, */
  free(ist->lvls);              /* the level vector, */
  free(ist->map);               /* the identifier map, */
  free(ist->buf);               /* the path buffer, */
  free(ist);                    /* and the tree body */
}  /* ist_delete() */

/*--------------------------------------------------------------------*/

void ist_count (ISTREE *ist, int *set, int cnt)
{                               /* --- count transaction in tree */
  assert(ist                    /* check the function arguments */
     && (cnt >= 0) && (set || (cnt <= 0)));
  if (cnt >= ist->height)       /* recursively count transaction */
    _count(ist->lvls[0], set, cnt, ist->height);
}  /* ist_count() */

/*--------------------------------------------------------------------*/

void ist_countx (ISTREE *ist, TATREE *tat)
{                               /* --- count transaction in tree */
  assert(ist && tat);           /* check the function arguments */
  _countx(ist->lvls[0], tat, ist->height);
}  /* ist_countx() */           /* recursively count the trans. tree */

/*--------------------------------------------------------------------*/

int ist_check (ISTREE *ist, char *marks)
{                               /* --- check item usage */
  int i, n;                     /* loop variable, number of items */

  assert(ist);                  /* check the function argument */
  for (i = ist->lvls[0]->size; --i >= 0; )
    marks[i] = 0;               /* clear the marker vector */
  _checkuse(ist->lvls[0], marks, ist->supp);
  for (n = 0, i = ist->lvls[0]->size; --i >= 0; )
    if (marks[i]) n++;          /* count used items */
  return n;                     /* and return this number */
}  /* ist_check() */

/*--------------------------------------------------------------------*/

int ist_addlvl (ISTREE *ist)
{                               /* --- add a level to item set tree */
  int    i, n, c;               /* loop variable, counter, buffer */
  ISNODE **ndp;                 /* to traverse the nodes */
  ISNODE *node;                 /* new (reallocated) node */
  ISNODE **end;                 /* end of new level node list */
  ISNODE *cur;                  /* current node in new level */
  ISNODE *frst;                 /* first child of current node */
  ISNODE *last;                 /* last  child of current node */
  ISNODE **vec;                 /* child node vector */
  int    *map;                  /* identifier map */
  void   *p;                    /* temporary buffer */

  assert(ist);                  /* check the function arguments */

  /* --- enlarge level vector --- */
  if (ist->height >= ist->vsz){ /* if the level vector is full */
    n = ist->vsz +BLKSIZE;      /* compute new vector size */
    p = realloc(ist->lvls, n *sizeof(ISNODE*));
    if (!p) return -1;          /* enlarge the level vector */
    ist->lvls = (ISNODE**)p;    /* and set the new vector */
    p = realloc(ist->buf,    n *sizeof(int));
    if (!p) return -1;          /* enlarge the buffer vector */
    ist->buf = (int*)p;         /* and set the new vector */
    ist->vsz = n;               /* set the new vector size */
  }                             /* (applies to buf and levels) */
  end  = ist->lvls +ist->height;
  *end = NULL;                  /* start a new tree level */

  /* --- add tree level --- */
  for (ndp = ist->lvls +ist->height -1; *ndp; ndp = &(*ndp)->succ) {
    frst = last = NULL;         /* traverse the deepest nodes */
    for (i = n = 0; i < (*ndp)->size; i++) {
      cur = _child(ist, *ndp, i, ist->supp, ist->rule);
      if (!cur) continue;       /* create a child if necessary */
      if (cur == (void*)-1) { _cleanup(ist); return -1; }
      if (!frst) frst = cur;    /* note first and last child node */
      *end = last = cur;        /* add node at the end of the list */
      end  = &cur->succ; n++;   /* that contains the new level */
    }                           /* and advance end pointer */
    if (n <= 0) {               /* if no child node was created, */
      (*ndp)->chcnt = F_SKIP; continue; }       /* skip the node */
    #ifdef BENCH                /* if benchmark version */
    ist->cpnec += n;            /* sum the number of necessary */
    #endif                      /* child pointers */
    node = *ndp;                /* decide on the node structure: */
    if (node->offset >= 0) {    /* if a pure counter vector is used, */
      n = ID(last)-ID(frst)+1;  /* always add a pure child vector */
      i = (node->size -1) *sizeof(int) +n *sizeof(ISNODE*); }
    else if (2*n > node->size){ /* if a single id. map is best, */
      n = node->size;           /* only add a child vector */
      i = (n+n-1) *sizeof(int) +n *sizeof(ISNODE*); }
    else {                      /* if two identifier maps are best, */
      i = node->size;           /* add a child vector and a map */
      i = (i+i-1) *sizeof(int) +n *(sizeof(ISNODE*) +sizeof(int));
    }                           /* get size of additional vectors */
    node = (ISNODE*)realloc(node, sizeof(ISNODE) +i);
    if (!node) { _cleanup(ist); return -1; }
    node->chcnt = n;            /* add a child vector to the node */
    #ifdef BENCH                /* if benchmark version */
    ist->cpcnt += n;            /* sum the number of child pointers */
    if ((node->offset >= 0) || (node->size == n))
         ist->bytes += n * sizeof(ISNODE*);
    else ist->bytes += n *(sizeof(ISNODE*) +sizeof(int));
    #endif                      /* determine the memory usage */
    if ((node != *ndp) && node->parent) {
      last = node->parent;      /* adapt the ref. from the parent */
      if (last->offset >= 0) {  /* if a pure vector is used */
        vec = (ISNODE**)(last->cnts +last->size);
        vec[(vec[0] != *ndp) ? ID(node) -ID(vec[0]) : 0] = node; }
      else {                    /* if an identifier map is used */
        map = last->cnts +(i = last->size);
        vec = (ISNODE**)(map+i);/* get identifier map, child vector, */
        c   = last->chcnt & ~F_SKIP;   /* and the number of children */
        if (c < i)              /* if a secondary id. map exists, */
          map = (int*)(vec +(i = c));  /* get this identifier map */
        vec[_bsearch(map, i, ID(node))] = node;
      }                         /* find the proper index and */
    }                           /* set the new child pointer */
    *ndp = node;                /* set new (reallocated) node */
    if (node->offset >= 0) {    /* if to use pure vectors */
      vec = (ISNODE**)(node->cnts +node->size);
      while (--n >= 0) vec[n] = NULL;
      i = ID(frst);             /* get item identifier of first child */
      for (cur = frst; cur; cur = cur->succ) {
        vec[ID(cur)-i] = cur;   /* set the child node pointer */
        cur->parent    = node;  /* and the parent pointer */
      } }                       /* in the new node */
    else if (n < node->size) {  /* if two identifier maps are used */
      vec = (ISNODE**)(node->cnts +node->size +node->size);
      map = (int*)(vec +n);     /* get the secondary identifier map */
      for (i = 0, cur = frst; cur; cur = cur->succ) {
        vec[i]      = cur;      /* set the child node pointer, */
        map[i++]    = ID(cur);  /* the identifier map entry, */
        cur->parent = node;     /* and the parent pointer */
      } }                       /* in the new node */
    else {                      /* if one identifier map is used */
      map = node->cnts +(i = node->size);
      vec = (ISNODE**)(map +i); /* get id. map and child vector */
      while (--n >= 0) vec[n] = NULL;
      for (cur = frst; cur; cur = cur->succ) {
        vec[_bsearch(map, i, ID(cur))] = cur;
        cur->parent = node;     /* set the child node pointer */
      }                         /* and the parent pointer */
    }                           /* in the new node */
  }
  if (!ist->lvls[ist->height])  /* if no child has been added, */
    return 1;                   /* abort the function, otherwise */
  ist->height++;                /* increment the level counter */
  _checksub(ist->lvls[0]);      /* check for unnecessary subtrees */
  return 0;                     /* return 'ok' */
}  /* ist_addlvl() */

/*--------------------------------------------------------------------*/

void ist_up (ISTREE *ist, int root)
{                               /* --- go up in item set tree */
  assert(ist && ist->curr);     /* check the function argument */
  if      (root)                /* if root flag set, */
    ist->curr = ist->lvls[0];   /* go to the root node */
  else if (ist->curr->parent)   /* if it exists, go to the parent */
    ist->curr = ist->curr->parent;
}  /* ist_up() */

/*--------------------------------------------------------------------*/

int ist_down (ISTREE *ist, int item)
{                               /* --- go down in item set tree */
  ISNODE *node;                 /* the current node */
  ISNODE **vec;                 /* child node vector of current node */
  int    *map, n;               /* identifier map and its size */
  int    c;                     /* number of children */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  c = node->chcnt & ~F_SKIP;    /* if there are no child nodes, */
  if (c <= 0) return -1;        /* abort the function */
  if (node->offset >= 0) {      /* if a pure vector is used */
    vec = (ISNODE**)(node->cnts +node->size);
    item -= ID(vec[0]);         /* compute index in child node vector */
    if (item >= c) return -1; } /* and abort if there is no child */
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    vec = (ISNODE**)(map +n);   /* get id. map and child vector */
    if (c < n)                  /* if a secondary id. map exists, */
      map = (int*)(vec +(n = c));      /* get this identifier map */
    item = _bsearch(map, n, item);
  }                             /* search for the proper index */
  if ((item < 0) || !vec[item]) /* if the index is out of range */
    return -1;                  /* or the child does not exist, abort */
  ist->curr = vec[item];        /* otherwise go to the child node */
  return 0;                     /* return 'ok' */
}  /* ist_down() */

/*--------------------------------------------------------------------*/

int ist_next (ISTREE *ist, int item)
{                               /* --- get next item with a counter */
  int    i;                     /* vector index */
  ISNODE *node;                 /* the current node */
  int    *map, n;               /* identifier map and its size */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if a pure vector is used, */
    if (item <  node->offset) return node->offset;
    if (item >= node->offset +node->size) return -1;
    return item +1; }           /* return the next item identifier */
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    if (item <  map[0])   return map[0];
    if (item >= map[n-1]) return -1;
    i = _bsearch(map, n, item); /* try to find the item directly */
    if (i >= 0) return map[i+1];/* and return the following one */
    while ((--n >= 0) && (*map > item)) map++;
    return (n >= 0) ? *map :-1; /* search iteratively for the next */
  }                             /* item identifier and return it */
}  /* ist_next() */

/*--------------------------------------------------------------------*/

void ist_setcnt (ISTREE *ist, int item, int cnt)
{                               /* --- set counter for an item */
  ISNODE *node;                 /* the current node */
  ISNODE **vec;                 /* child node vector of current node */
  int    *map, n;               /* identifier map and its size */
  int    c;                     /* number of children */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if a pure vector is used, */
    item -= node->offset;       /* get index in counter vector */
    if (item >= node->size) return; }
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    vec = (ISNODE**)(map +n);   /* get id. map and child vector */
    c = node->chcnt & ~F_SKIP;  /* and the number of children */
    if (c < n)                  /* if a secondary id. map exists, */
      map = (int*)(vec +(n = c));      /* get this identifier map */
    item = _bsearch(map, n, item);
  }                             /* search for the proper index */
  if (item >= 0) node->cnts[item] = cnt;
}  /* ist_setcnt() */           /* set the frequency counter */

/*--------------------------------------------------------------------*/

int ist_getcnt (ISTREE *ist, int item)
{                               /* --- get counter for an item */
  ISNODE *node;                 /* the current node */
  ISNODE **vec;                 /* child node vector of current node */
  int    *map, n;               /* identifier map and its size */
  int    c;                     /* number of children */

  assert(ist && ist->curr);     /* check the function argument */
  node = ist->curr;             /* get the current node */
  if (node->offset >= 0) {      /* if pure vectors are used, */
    item -= node->offset;       /* get index in counter vector */
    if (item >= node->size) return -1; }
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    vec = (ISNODE**)(map +n);   /* get id. map and child vector */
    c = node->chcnt & ~F_SKIP;  /* and the number of children */
    if (c < n)                  /* if a secondary id. map exists, */
      map = (int*)(vec +(n = c));      /* get this identifier map */
    item = _bsearch(map, n, item);
  }                             /* search for the proper index */
  if (item < 0) return -1;      /* abort if index is out of range */
  return node->cnts[item];      /* return the value of the counter */
}  /* ist_getcnt() */

/*--------------------------------------------------------------------*/

int ist_getcntx (ISTREE *ist, int *set, int cnt)
{                               /* --- get counter for an item set */
  assert(ist                    /* check the function arguments */
     && (cnt >= 0) && (set || (cnt <= 0)));
  if (cnt <= 0)                 /* if the item set is empty, */
    return ist->tacnt;          /* return the transaction count */
  return COUNT(_getsupp(ist->lvls[0], set, cnt));
}  /* ist_getcntx() */          /* return the item set support */

/*--------------------------------------------------------------------*/

void ist_filter (ISTREE *ist, int mode)
{                               /* --- filter frequent item sets */
  int    i, k;                  /* loop variables */
  ISNODE *node;                 /* to traverse the nodes */
  int    supp;                  /* support of an item set */

  assert(ist);                  /* check the function argument */
  if (mode == IST_CLEAR) {      /* if to clear all skip flags */
    for (k = 1; k < ist->height; k++)
      for (node = ist->lvls[k]; node; node = node->succ)
        for (i = 0; i < node->size; i++)
          node->cnts[i] &= ~F_SKIP;
    return;                     /* clear all skip flags */
  }                             /* and abort the function */
  supp = -1;                    /* set default support filter */
  for (k = 1; k < ist->height; k++) {
    for (node = ist->lvls[k]; node; node = node->succ) {
      for (i = 0; i < node->size; i++) {
        if (node->cnts[i] < ist->supp)
          continue;             /* skip infrequent item sets */
        if (mode == IST_CLOSED) supp = node->cnts[i];
        _marksub(ist, node, i, supp);
      }                         /* mark all n-1 subsets */
    }                           /* of the current item set */
  }                             /* that have to be cleared/marked */
}  /* ist_filter() */

/*--------------------------------------------------------------------*/

void ist_init (ISTREE *ist, int minlen, int arem, double minval)
{                               /* --- initialize (rule) extraction */
  assert(ist                    /* check the function arguments */
      && (minlen > 0) && (minval >= 0.0) && (minval <= 1.0));
  ist->item = ist->index = -1;  /* initialize rule extraction */
  ist->node = ist->lvls[minlen -1];
  ist->size = minlen;
  ist->head = NULL;
  if ((arem < EM_NONE) || (arem >= EM_UNKNOWN))
    arem = EM_NONE;             /* check, adapt, and note */
  ist->arem   = arem;           /* additional evaluation measure */
  ist->minval = minval;         /* and its minimal value */
}  /* ist_init() */

/*--------------------------------------------------------------------*/

int ist_set (ISTREE *ist, int *set, int *supp, double *aval)
{                               /* --- extract next frequent item set */
  int    i;                     /* loop variable */
  int    item;                  /* an item identifier */
  ISNODE *node, *tmp;           /* current item set node, buffer */
  int    *cnts;                 /* to access the item frequencies */
  int    s_set;                 /* support of the current set */
  double dev;                   /* deviation from indep. occurrence */

  assert(ist && set && supp);   /* check the function arguments */
  if (ist->size > ist->height)  /* if the tree is not high enough */
    return -1;                  /* for the item set size, abort */

  /* --- find frequent item set --- */
  node = ist->node;             /* get the current item set node */
  while (1) {                   /* search for a frequent item set */
    if (++ist->index >= node->size) { /* if all subsets have been */
      node = node->succ;        /* processed, go to the successor */
      if (!node) {              /* if at the end of a level, go down */
        if (++ist->size > ist->height)
          return -1;            /* if beyond the deepest level, abort */
        node = ist->lvls[ist->size -1];
      }                         /* get the 1st node of the new level */
      ist->node  = node;        /* note the new item set node */
      ist->index = 0;           /* start with the first item set */
    }                           /* of the new item set node */
    if (node->offset >= 0) item = node->offset +ist->index;
    else                   item = node->cnts[node->size +ist->index];
    if (is_getapp(ist->set, item) == IST_IGNORE)
      continue;                 /* skip items to ignore */
    s_set = node->cnts[ist->index];
    if (s_set < ist->supp)      /* if the support is not sufficient, */
      continue;                 /* go to the next item set */
    /* Note that this check automatically skips all item sets that */
    /* are marked with the flag F_SKIP, because s_set is negative  */
    /* with this flag and thus necessarily smaller than ist->supp. */
    dev = 0;                    /* init. add. evaluation measure */
    if (ist->arem == EM_DIFF) { /* if logarithm of support quotient */
      cnts = ist->lvls[0]->cnts;
      dev  = log(s_set) -log(COUNT(cnts[item]));
      for (tmp = node; tmp->parent; tmp = tmp->parent)
        dev -= log(COUNT(cnts[ID(tmp)]));
      dev = (dev +(ist->size-1) *log(ist->tacnt)) *(0.01/LN_2);
      if (dev < ist->minval)    /* if the value of the additional */
        continue;               /* eval. measure is not high enough, */
    }                           /* skip the item set */
    break;                      /* otherwise abort the search loop */
  }
  *supp = s_set;                /* store the item set support and */
  if (aval) *aval = dev;        /* the value of the add. measure */

  /* --- build frequent item set --- */
  i        = ist->size;         /* get the current item set size */
  set[--i] = item;              /* and store the first item */
  while (node->parent) {        /* while not at the root node */
    set[--i] = ID(node);        /* add item to the item set */
    node = node->parent;        /* and go to the parent node */
  }
  return ist->size;             /* return the item set size */
}  /* ist_set() */

/*--------------------------------------------------------------------*/

int ist_rule (ISTREE *ist, int *rule,
              int *supp, double *conf, double *lift, double *aval)
{                               /* --- extract next rule */
  int    i;                     /* loop variable */
  int    item;                  /* an item identifier */
  ISNODE *node;                 /* current item set node */
  ISNODE *parent;               /* parent of the item set node */
  int    *map, n;               /* identifier map and its size */
  int    s_set;                 /* support of set  (body & head) */
  int    s_body;                /* support of body (antecedent) */
  int    s_head;                /* support of head (consequent) */
  double c, v;                  /* confidence and measure value */
  int    app;                   /* appearance flag of head item */

  assert(ist && rule && supp);  /* check the function arguments */
  if (ist->size > ist->height)  /* if the tree is not high enough */
    return -1;                  /* for the rule length, abort */

  /* --- find rule --- */
  node = ist->node;             /* get the current item set node */
  while (1) {                   /* search for a rule */
    if (ist->item >= 0) {       /* --- select next item subset */
      *--ist->path = ist->item; /* add previous head to the path */
      ist->plen++;              /* and get the next head item */
      ist->item = ID(ist->head);
      ist->head = ist->head->parent;
      if (!ist->head)           /* if all subsets have been processed */
        ist->item = -1;         /* clear the head item to trigger the */
    }                           /* selection of a new item set */
    if (ist->item < 0) {        /* --- select next item set */
      if (++ist->index >= node->size){/* if all subsets have been */
        node = node->succ;      /* processed, go to the successor */
        if (!node) {            /* if at the end of a level, go down */
          if (++ist->size > ist->height)
            return -1;          /* if beyond the deepest level, abort */
          node = ist->lvls[ist->size -1];
        }                       /* get the 1st node of the new level */
        ist->node = node;       /* note the new item set node and */
        ist->index  = 0;        /* start with the first item set */
      }                         /* of the new item set node */
      if (node->offset >= 0) item = node->offset +ist->index;
      else                   item = node->cnts[node->size +ist->index];
      app = is_getapp(ist->set, item);
      if ((app == IST_IGNORE) || (HDONLY(node) && (app == IST_HEAD)))
        continue;               /* skip sets with two head only items */
      ist->item   = item;       /* set the head item identifier */
      ist->hdonly = HDONLY(node) || (app == IST_HEAD);
      ist->head   = node;       /* set the new head item node */
      ist->path   = ist->buf +ist->vsz;
      ist->plen   = 0;          /* clear the path */
    }
    app = is_getapp(ist->set, ist->item); /* get head item appearance */
    if (!(app & IST_HEAD) || (ist->hdonly && (app != IST_HEAD)))
      continue;                 /* if rule is not allowed, skip it */
    s_set = COUNT(node->cnts[ist->index]);
    if (s_set < ist->supp) {    /* get and check the item set support */
      ist->item = -1; continue; }
    parent = node->parent;      /* get the parent node */
    if (ist->plen > 0)          /* if there is a path, use it */
      s_body = COUNT(_getsupp(ist->head, ist->path, ist->plen));
    else if (!parent)           /* if there is no parent (root node), */
      s_body = ist->tacnt;      /* get the number of transactions */
    else if (parent->offset >= 0)  /* if a pure vector is used */
      s_body = COUNT(parent->cnts[ID(node) -parent->offset]);
    else {                      /* if an identifier map is used */
      map = parent->cnts +(n = parent->size);
      s_body = COUNT(parent->cnts[_bsearch(map, n, ID(node))]);
    }                           /* find vector index and get support */
    if (s_body < ist->rule)     /* if the body support is too low, */
      continue;                 /* get the next subset/next set */
    c = s_set/(double)s_body;   /* compute the rule confidence */
    if (c < ist->conf -EPSILON) /* if the confidence is too low, */
      continue;                 /* go to the next item (sub)set */
    s_head = COUNT(ist->lvls[0]->cnts[ist->item]);
    if (ist->arem == EM_NONE) { /* if no add. eval. measure given, */
      v = 0; break; }           /* abort the loop (select the rule) */
    if (ist->size < 2) {        /* if rule has an empty antecedent, */
      v = 0; break; }           /* abort the loop (select the rule) */
    v = _evalfns[ist->arem](s_set, s_body, s_head, ist->tacnt);
    if (v >= ist->minval)       /* if rule value exceeds the minimal */
      break;                    /* of the add. rule eval. measure, */
  }  /* while (1) */            /* abort the loop (select rule) */
  *supp = (ist->mode & IST_HEAD) ? s_set : s_body;
  if (lift)                     /* compute and store the lift value */
    *lift = (c *ist->tacnt)/(double)s_head;
  if (conf) *conf = c;          /* store the rule confidence and */
  if (aval) *aval = v;          /* the value of the add. measure */

  /* --- build rule --- */
  if (node->offset >= 0) item = node->offset +ist->index;
  else                   item = node->cnts[node->size +ist->index];
  i = ist->size;                /* get the current item and */
  if (item != ist->item)        /* if this item is not the head, */
    rule[--i] = item;           /* add it to the rule body */
  while (node->parent) {        /* traverse the path to the root */
    if (ID(node) != ist->item)  /* and add all items on this */
      rule[--i] = ID(node);     /* path to the rule body */
    node = node->parent;        /* (except the head of the rule) */
  }
  rule[0] = ist->item;          /* set the head of the rule, */
  return ist->size;             /* return the rule size */
}  /* ist_rule() */

/*--------------------------------------------------------------------*/

int ist_hedge (ISTREE *ist, int *hedge,
               int *supp, double *conf, double *aval)
{                               /* --- extract next hyperedge */
  int    i;                     /* loop variable */
  int    item;                  /* an item identifier */
  ISNODE *node;                 /* current item set node */
  ISNODE *head;                 /* node containing the rule head */
  int    *map, n;               /* identifier map and its size */
  int    *path, plen;           /* path in tree and its length */
  int    s_set;                 /* support of set (body & head) */
  int    s_body;                /* support of body (antecedent) */
  int    s_head;                /* support of head (consequent) */
  double c, t, v = 0;           /* confidence and measure value */

  assert(ist && hedge && supp); /* check the function arguments */
  if (ist->size > ist->height)  /* if the tree is not high enough */
    return -1;                  /* for the hyperedge size, abort */

  /* --- find hyperedge --- */
  node = ist->node;             /* get the current item set node */
  while (1) {                   /* search for a hyperedge */
    if (++ist->index >= node->size) { /* if all subsets have been */
      node = node->succ;        /* processed, go to the successor */
      if (!node) {              /* if at the end of a level, go down */
        if (++ist->size > ist->height)
          return -1;            /* if beyond the deepest level, abort */
        node = ist->lvls[ist->size -1];
      }                         /* get the 1st node of the new level */
      ist->node  = node;        /* note the new item set node and */
      ist->index = 0;           /* start with the first item set */
    }                           /* of the new item set node */
    if (node->offset >= 0) item = node->offset +ist->index;
    else                   item = node->cnts[node->size +ist->index];
    if (is_getapp(ist->set, item) == IST_IGNORE)
      continue;                 /* skip items to ignore */
    s_set = COUNT(node->cnts[ist->index]);
    if (s_set < ist->supp)      /* if the set support is too low, */
      continue;                 /* skip this item set */
    head = node->parent;        /* get subset support from parent */
    if (!head)                  /* if there is no parent (root node), */
      s_body = ist->tacnt;      /* get the total number of sets */
    else if (head->offset >= 0) /* if pure vectors are used */
      s_body = head->cnts[ID(node) -head->offset];
    else {                      /* if an identifier map is used */
      map = head->cnts +(n = head->size);
      s_body = head->cnts[_bsearch(map, n, ID(node))];
    }                           /* find index and get the support */
    if (s_body & F_SKIP) {      /* check for a valid body */
      node->cnts[ist->index] |= F_SKIP; continue; }
    s_body = COUNT(s_body);     /* get the support of body and head */
    s_head = COUNT(ist->lvls[0]->cnts[item]);
    c = s_set/(double)s_body;   /* compute confidence and add. eval. */
    v = _evalfns[ist->arem](s_set, s_body, s_head, ist->tacnt);
    item = ID(node);            /* note the next head item */
    plen = 1;                   /* and initialize the path */
    path = ist->buf +ist->vsz;  /* (store first item) */
    *--path = ist->index +node->offset;
    while (head) {              /* traverse the path up to root */
      s_body = _getsupp(head, path, plen);
      if (s_body & F_SKIP) break;
      s_body = COUNT(s_body);   /* get the support of the body */
      *--path = item; plen++;   /* store the previous head item */
      item = ID(head);          /* in the path (extend path) */
      c += s_set/(double)s_body;/* sum the rule confidences */
      s_head = COUNT(ist->lvls[0]->cnts[item]);
      t = _evalfns[ist->arem](s_set, s_body, s_head, ist->tacnt);
      if (t < v) v = t;         /* compute the add. evaluation */
      head = head->parent;      /* and go to the parent node */
    }                           /* (get the next rule head) */
    c /= ist->size;             /* average the rule confidences */
    if (!head                   /* check for a complete traversal */
    &&  (c > ist->conf         -EPSILON)
    &&  (v > fabs(ist->minval) -EPSILON))
      break;                    /* check whether hyperedge qualifies */
    if ((ist->minval < 0) && node->parent)
      node->cnts[ist->index] |= F_SKIP;
  }  /* while (1) */            /* otherwise mark it as skipped */
  *supp = s_set;                /* store the hyperedge support, */
  if (conf) *conf = c;          /* the average confidence and */
  if (aval) *aval = v;          /* the value of the add. measure */

  /* --- build hyperedge --- */
  i = ist->size -1;             /* store the first item */
  if (node->offset >= 0) hedge[i] = ist->index +node->offset;
  else                   hedge[i] = node->cnts[node->size +ist->index];
  while (node->parent) {        /* while not at the root node */
    hedge[--i] = ID(node);      /* add item to the hyperedge */
    node = node->parent;        /* and go to the parent node */
  }
  return ist->size;             /* return the hyperedge size */
}  /* ist_hedge() */

/*--------------------------------------------------------------------*/

int ist_group (ISTREE *ist, int *group, int *supp, double *aval)
{                               /* --- extract next group */
  int    i;                     /* loop variable */
  int    item;                  /* an item identifier */
  ISNODE *node;                 /* current item set node */
  ISNODE *head;                 /* node containing the rule head */
  int    *map, n;               /* identifier map and its size */
  int    *path, plen;           /* path in tree and its length */
  int    s_set;                 /* support of set (body & head) */
  int    s_body;                /* support of body (antecedent) */
  int    s_head;                /* support of head (consequent) */
  double t, v = 0;              /* additional measure value */

  assert(ist && group && supp); /* check the function arguments */
  if (ist->item <  0) {         /* if this is the first call */
    ist->size = ist->height;    /* init. the extraction variables */
    ist->node = ist->lvls[ist->size -1]; ist->item = 0;
  }
  if (ist->size <= 1)           /* if all groups are reported */
    return -1;                  /* for the hyperedge size, abort */

  /* --- find next group --- */
  node = ist->node;             /* get the current item set node */
  while (1) {                   /* search for a hyperedge */
    if (++ist->index >= node->size) { /* if all subsets have been */
      node = node->succ;        /* processed, go to the successor */
      if (!node) {              /* if at the end of a level, go down */
        if (--ist->size <= 1)   /* if all groups are reported, */
          return -1;            /* abort the extraction */
        node = ist->lvls[ist->size -1];
      }                         /* get the 1st node of the new level */
      ist->node  = node;        /* note the new item set node and */
      ist->index = 0;           /* start with the first item set */
    }                           /* of the new item set node */
    if (node->offset >= 0) item = node->offset +ist->index;
    else                   item = node->cnts[node->size +ist->index];
    if (is_getapp(ist->set, item) == IST_IGNORE)
      continue;                 /* skip items to ignore */
    s_set = node->cnts[ist->index];
    if (s_set < ist->supp) {    /* if the set support is too low */
      if (s_set & F_SKIP) _marksub(ist, node, ist->index, -1);
      continue;                 /* mark subsets if necessary */
    }                           /* and skip this item set */
    /* Note that this check automatically skips all item sets that */
    /* are marked with the flag F_SKIP, because s_set is negative  */
    /* with this flag and thus necessarily smaller than ist->supp. */
    head = node->parent;        /* get subset support from parent */
    if (!head)                  /* if there is no parent (root node), */
      s_body = ist->tacnt;      /* get the total number of sets */
    else if (head->offset >= 0) /* if pure vectors are used */
      s_body = head->cnts[ID(node) -head->offset];
    else {                      /* if an identifier map is used */
      map = head->cnts +(n = head->size);
      s_body = head->cnts[_bsearch(map, n, ID(node))];
    }                           /* find index and get the support */
    s_body = COUNT(s_body);     /* get the support of body and head */
    s_head = COUNT(ist->lvls[0]->cnts[item]);
    v = _evalfns[ist->arem](s_set, s_body, s_head, ist->tacnt);
    item = ID(node);            /* note the next head item */
    plen = 1;                   /* and initialize the path */
    path = ist->buf +ist->vsz;  /* (store first item) */
    *--path = ist->index +node->offset;
    while (head) {              /* traverse the path up to root */
      s_body  = COUNT(_getsupp(head, path, plen));
      *--path = item; plen++;   /* store the previous head item */
      item    = ID(head);       /* in the path (extend path) */
      s_head  = COUNT(ist->lvls[0]->cnts[item]);
      t = _evalfns[ist->arem](s_set, s_body, s_head, ist->tacnt);
      if (t < v) v = t;         /* compute the add. evaluation */
      head = head->parent;      /* and go to the parent node */
    }                           /* (get the next rule head) */
    if (!head                   /* check for a complete traversal */
    &&  (v > fabs(ist->minval) -EPSILON))
      break;                    /* check whether group qualifies */
  }  /* while (1) */
  *supp = s_set;                /* store the group support and */
  if (aval) *aval = v;          /* the value of the add. measure */
  _marksub(ist, node, ist->index, -1);

  /* --- build hyperedge --- */
  i = ist->size -1;             /* store the first item */
  if (node->offset >= 0) group[i] = ist->index +node->offset;
  else                   group[i] = node->cnts[node->size +ist->index];
  while (node->parent) {        /* while not at the root node */
    group[--i] = ID(node);      /* add item to the hyperedge */
    node = node->parent;        /* and go to the parent node */
  }
  return ist->size;             /* return the hyperedge size */
}  /* ist_group() */

/*--------------------------------------------------------------------*/
#ifndef NDEBUG

static void _showtree (ISNODE *node, int level)
{                               /* --- show subtree */
  int    i, k;                  /* loop variables, buffer */
  int    *map, n;               /* identifier map and its size */
  int    c;                     /* number of children */
  ISNODE **vec;                 /* vector of child nodes */

  assert(node && (level >= 0)); /* check the function arguments */
  c = node->chcnt & ~F_SKIP;    /* get the number of children */
  if      (c <= 0)              /* if there are no children, */
    vec = NULL;                 /* clear the child vector variable */
  else if (node->offset >= 0)   /* if a pure vector is used */
    vec = (ISNODE**)(node->cnts +node->size);
  else {                        /* if an identifier map is used */
    map = node->cnts +(n = node->size);
    vec = (ISNODE**)(map +n);   /* get id. map and child vector */
    if (c < n)                  /* if a secondary id. map exists, */
      map = (int*)(vec +(n = c));      /* get this identifier map */
  }                             /* get child access variables */
  for (i = 0; i < node->size; i++) {
    for (k = level; --k >= 0; ) /* indent and print */
      printf("   ");            /* item identifier and counter */
    if (node->offset >= 0) k = node->offset +i;
    else                   k = node->cnts[node->size +i];
    printf("%d: %d\n", k, COUNT(node->cnts[i]));
    if (!vec) continue;         /* check whether there are children */
    if (node->offset >= 0) k -= ID(vec[0]);
    else                   k = _bsearch(map, n, k);
    if ((k >= 0) && (k < c) && vec[k])
      _showtree(vec[k], level +1);
  }                             /* show subtree recursively */
}  /* _showtree() */

/*--------------------------------------------------------------------*/

void ist_show (ISTREE *ist)
{                               /* --- show an item set tree */
  assert(ist);                  /* check the function argument */
  _showtree(ist->lvls[0], 0);   /* show nodes recursively */
  printf("total: %d\n", ist->tacnt);
}  /* ist_show() */             /* print number of transactions */

#endif
