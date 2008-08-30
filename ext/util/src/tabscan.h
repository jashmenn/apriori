/*----------------------------------------------------------------------
  File    : tabscan.h
  Contents: table scanner management
  Author  : Christian Borgelt
  History : 1998.01.04 file created as tfscan.h
            1998.03.11 additional character flags enabled
            1998.08.12 function ts_copy() added
            1998.11.26 some function parameters changed to const
            1999.02.04 long int changed to int
            2001.07.14 ts_sgetc() modified, ts_buf() and ts_err() added
            2001.08.19 ts_delim() added (last delimiter type)
            2002.02.11 ts_reccnt() and ts_reset() added
            2007.02.13 renamed to tabscan, TS_NULL added
            2007.05.17 function ts_allchs() added
----------------------------------------------------------------------*/
#ifndef __TABSCAN__
#define __TABSCAN__
#include <stdio.h>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- character flags --- */
#define TS_RECSEP   0x01       /* flag for record separator */
#define TS_FLDSEP   0x02       /* flag for field separator */
#define TS_BLANK    0x04       /* flag for blank character */
#define TS_NULL     0x08       /* flag for null value characters */
#define TS_COMMENT  0x10       /* flag for comment character */
#define TS_OTHER    0x20       /* flag for other character type */

/* --- delimiter types --- */
#define TS_ERR     -2          /* error indicator */
#define TS_EOF     -1          /* end of file delimiter */
#define TS_FLD      0          /* field  delimiter */
#define TS_REC      1          /* record delimiter */

/* --- buffer size --- */
#define TS_SIZE     256        /* size of internal read buffer */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- error information --- */
  int    code;                  /* error code */
  int    rec, fld;              /* record and field number */
  int    exp;                   /* expected number of records/fields */
  char   *s;                    /* a string (e.g., field contents) */
} TSINFO;                       /* (error information) */

typedef struct {                /* --- table scanner --- */
  char   cflags[256];           /* character flags */
  int    reccnt;                /* number of records read */
  int    delim;                 /* last delimiter read */
  int    cnt;                   /* number of characters read */
  char   buf[TS_SIZE+4];        /* read buffer */
  TSINFO info;                  /* error information */
} TABSCAN;                      /* (table file scanner) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern TABSCAN* ts_create (void);
extern void     ts_delete (TABSCAN *tsc);
extern void     ts_copy   (TABSCAN *dst, const TABSCAN *src);

extern int      ts_chars  (TABSCAN *tsc, int type, const char *chars);
extern void     ts_allchs (TABSCAN *tsc, const char *recseps,
                           const char *fldseps, const char *blanks,
                           const char *nullchs, const char *comment);
extern int      ts_istype (const TABSCAN *tsc, int type, int c);
extern int      ts_type   (const TABSCAN *tsc, int c);

extern int      ts_next   (TABSCAN *tsc, FILE *file, char *buf,int len);
extern int      ts_delim  (TABSCAN *tsc);
extern int      ts_cnt    (TABSCAN *tsc);
extern char*    ts_buf    (TABSCAN *tsc);

extern int      ts_reccnt (TABSCAN *tsc);
extern void     ts_reset  (TABSCAN *tsc);

extern TSINFO*  ts_info   (TABSCAN *tsc);

extern int      ts_decode (char const **s);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ts_delete(s)     free(s)

#define ts_istype(s,t,c) ((s)->cflags[(unsigned char)(c)] & (t))
#define ts_type(s,c)     ((s)->cflags[(unsigned char)(c)])

#define ts_delim(s)      ((s)->delim)
#define ts_cnt(s)        ((s)->cnt)
#define ts_buf(s)        ((s)->buf)

#define ts_reccnt(s)     ((s)->reccnt)
#define ts_info(s)       (&(s)->info)
#endif
