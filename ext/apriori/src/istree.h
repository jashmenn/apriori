/*----------------------------------------------------------------------
  File    : istree.h
  Contents: item set tree management
  Author  : Christian Borgelt
  History : 1996.01.22 file created
            1996.01.29 ISNODE.offset and ISNODE.id added
            1996.02.08 ISTREE.tacnt, ISTREE.curr, ISTREE.index,
                       ISTREE.head and ISTREE.conf added
            1996.03.28 support made relative to number of item sets
            1996.11.23 ISTREE.levels (first nodes of each level) added
            1996.11.24 ISTREE.arem (add. rule evaluation measure) added
            1997.08.18 chi^2 evaluation measure added
                       parameter 'minlen' added to function ist_init()
            1998.02.11 parameter 'minval' added to function ist_init()
            1998.05.14 item set tree navigation functions added
            1998.08.08 parameter 'apps' added to function ist_create()
            1998.08.20 structure ISNODE redesigned
            1998.09.07 function ist_hedge added
            1998.12.08 function ist_gettac added,
                       float changed to double
            1999.02.05 long int changed to int
            1999.08.26 functions ist_first and ist_last added
            1999.11.05 rule evaluation measure EM_AIMP added
            1999.11.08 parameter 'aval' added to function ist_rule
            2001.04.01 functions ist_set and ist_getcntx added
            2001.12.28 sort function moved to module tract
            2002.02.07 function ist_clear removed, ist_settac added
            2002.02.11 optional use of identifier maps in nodes added
            2002.02.12 ist_first and ist_last replaced by ist_next
            2003.03.12 parameter lift added to function ist_rule
            2003.07.17 functions ist_itemcnt and ist_check added
            2003.07.18 function ist_maxfrq added (item set filter)
            2003.08.11 item set filtering generalized (ist_filter)
            2004.05.09 parameter 'aval' added to function ist_set
            2008.03.24 creation based on ITEMSET structure
----------------------------------------------------------------------*/
#ifndef __ISTREE__
#define __ISTREE__
#include "tract.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- additional evaluation measures --- */
#define EM_NONE     0           /* no measure */
#define EM_DIFF     1           /* absolute conf. difference to prior */
#define EM_QUOT     2           /* difference of conf. quotient to 1 */
#define EM_AIMP     3           /* abs. diff. of improvement to 1 */
#define EM_INFO     4           /* information difference to prior */
#define EM_CHI2     5           /* normalized chi^2 measure */
#define EM_PVAL     6           /* p-value of chi^2 measure */
#define EM_UNKNOWN  7           /* unknown measure */

/* --- item appearances --- */
#define IST_IGNORE  0           /* ignore item */
#define IST_BODY    1           /* item may appear in rule body */
#define IST_HEAD    2           /* item may appear in rule head */
#define IST_BOTH    (IST_HEAD|IST_BODY)

/* --- search mode flags --- */
#define IST_MEMOPT  4           /* optimize memory usage */

/* --- item set filter modes --- */
#define IST_CLEAR   0           /* clear markers */
#define IST_CLOSED  1           /* closed  item sets */
#define IST_MAXFRQ  2           /* maximal item sets */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct _isnode {        /* --- item set node --- */
  struct _isnode *parent;       /* parent node */
  struct _isnode *succ;         /* successor node on same level */
  int            id;            /* identifier used in parent node */
  int            chcnt;         /* number of child nodes */
  int            size;          /* size   of counter vector */
  int            offset;        /* offset of counter vector */
  int            cnts[1];       /* counter vector */
} ISNODE;                       /* (item set node) */

typedef struct {                /* --- item set tree --- */
  ITEMSET *set;                 /* underlying item set */
  int     mode;                 /* search mode (e.g. support def.) */
  int     tacnt;                /* number of transactions */
  int     vsz;                  /* size of level vector */
  int     height;               /* tree height (number of levels) */
  ISNODE  **lvls;               /* first node of each level */
  int     rule;                 /* minimal support of an assoc. rule */
  int     supp;                 /* minimal support of an item set */
  double  conf;                 /* minimal confidence of a rule */
  int     arem;                 /* additional rule evaluation measure */
  double  minval;               /* minimal evaluation measure value */
  ISNODE  *curr;                /* current node for traversal */
  int     size;                 /* size of item set/rule/hyperedge */
  ISNODE  *node;                /* item set node for extraction */
  int     index;                /* index in item set node */
  ISNODE  *head;                /* head item node for extraction */
  int     item;                 /* head item of previous rule */
  int     *buf;                 /* buffer for paths (support check) */
  int     *path;                /* current path / (partial) item set */
  int     plen;                 /* current path length */
  int     hdonly;               /* head only item in current set */
  int     *map;                 /* to create identifier maps */
#ifdef BENCH                    /* if benchmark version */
  int     sccnt;                /* number of support counters */
  int     scnec;                /* number of necessary supp. counters */
  int     cpcnt;                /* number of child pointers */
  int     cpnec;                /* number of necessary child pointers */
  int     bytes;                /* number of bytes used */
#endif
} ISTREE;                       /* (item set tree) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern ISTREE* ist_create  (ITEMSET *set, int mode,
                            int supp, double conf);
extern void    ist_delete  (ISTREE *ist);
extern int     ist_itemcnt (ISTREE *ist);

extern void    ist_count   (ISTREE *ist, int *set, int cnt);
extern void    ist_countx  (ISTREE *ist, TATREE *tat);
extern int     ist_settac  (ISTREE *ist, int cnt);
extern int     ist_gettac  (ISTREE *ist);
extern int     ist_check   (ISTREE *ist, char *marks);
extern int     ist_addlvl  (ISTREE *ist);
extern int     ist_height  (ISTREE *ist);

extern void    ist_up      (ISTREE *ist, int root);
extern int     ist_down    (ISTREE *ist, int item);
extern int     ist_next    (ISTREE *ist, int item);
extern void    ist_setcnt  (ISTREE *ist, int item, int cnt);
extern int     ist_getcnt  (ISTREE *ist, int item);
extern int     ist_getcntx (ISTREE *ist, int *set, int cnt);

extern void    ist_filter  (ISTREE *ist, int mode);
extern void    ist_init    (ISTREE *ist, int minlen,
                            int arem, double minval);
extern int     ist_set     (ISTREE *ist, int *set,   int *supp,
                            double *aval);
extern int     ist_rule    (ISTREE *ist, int *rule,  int *supp,
                            double *conf, double *lift, double *aval);
extern int     ist_hedge   (ISTREE *ist, int *hedge, int *supp,
                            double *conf, double *aval);
extern int     ist_group   (ISTREE *ist, int *asmb,  int *supp,
                            double *aval);

#ifndef NDEBUG
extern void    ist_show    (ISTREE *ist);
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ist_itemcnt(t)     ((t)->levels[0]->size)
#define ist_settac(t,n)    ((t)->tacnt = (n))
#define ist_gettac(t)      ((t)->tacnt)
#define ist_height(t)      ((t)->height)

#endif
