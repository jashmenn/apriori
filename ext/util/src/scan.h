/*----------------------------------------------------------------------
  File    : scan.h
  Contents: scanner (lexical analysis of a character stream)
  Author  : Christian Borgelt
  History : 1996.01.16 file created
            1996.02.21 definition of BUFSIZE made global
            1996.03.17 special tokens for keywords removed
            1998.02.08 recover and error message functions added
            1998.02.13 token T_RGT (right arrow '->') added
            1998.03.04 definitions of T_ID and T_NUM exchanged
            1998.04.17 token T_LFT (left  arrow '<-') added
            1998.05.27 token T_CMP (two char comparison operator) added
            1998.05.31 field f removed from struct TOKVAL
            2000.11.23 functions sc_len and sc_form added
            2001.07.15 scanner made an object
            2006.02.02 token T_DASH (undirected edge '--') added
----------------------------------------------------------------------*/
#ifndef __SCAN__
#define __SCAN__
#include <stdio.h>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- sizes --- */
#define SC_BUFSIZE  1024        /* size of scan buffer */

#ifdef SC_SCAN
/* --- tokens --- */
#define T_EOF        256        /* end of file */
#define T_NUM        257        /* number (floating point) */
#define T_ID         258        /* identifier or string */
#define T_RGT        259        /* right arrow '->' */
#define T_LFT        260        /* left  arrow '<-' */
#define T_DASH       261        /* dash '--' */
#define T_CMP        262        /* two char. comparison, e.g. '<=' */

/* --- error codes --- */
#define E_NONE         0        /* no error */
#define E_NOMEM      (-1)       /* not enough memory */
#define E_FOPEN      (-2)       /* file open  failed */
#define E_FREAD      (-3)       /* file read  failed */
#define E_FWRITE     (-4)       /* file write failed (unused) */
#define E_CHAR       (-5)       /* invalid character */
#define E_BUFOVF     (-6)       /* scan buffer overflow */
#define E_UNTSTR     (-7)       /* unterminated string */
#define E_UNTCOM     (-8)       /* unterminated comment */
#define E_STATE      (-9)       /* invalid scanner state */
#define E_GARBAGE   (-10)       /* garbage at end of file */

#endif
/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
#ifdef SC_SCAN
typedef struct {                 /* --- scanner --- */
  FILE       *file;              /* file to scan */
  int        start;              /* start line of comment */
  int        line,  pline;       /* input line of file */
  int        token, ptoken;      /* token */
  int        len,   plen;        /* length of token value */
  int        back;               /* flag for backward step */
  char       *value;             /* token value */
  char       buf[2][SC_BUFSIZE]; /* scan buffers */
  FILE       *errfile;           /* error output stream */
  int        lncnt;              /* error message line count */
  int        msgcnt;             /* number of add. error messages */
  const char **msgs;             /* additional error messages */
  char       fname[1];           /* name of file to scan */
} SCAN;                          /* (scanner) */
#endif

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern int         sc_fmtlen  (const char *s, int *len);
extern int         sc_format  (char *dst, const char *src, int quotes);

#ifdef SC_SCAN
extern SCAN*       sc_create  (const char *fname);
extern void        sc_delete  (SCAN *scan);
extern const char* sc_fname   (SCAN *scan);
extern int         sc_line    (SCAN *scan);

extern int         sc_next    (SCAN *scan);
extern int         sc_nexter  (SCAN *scan);
extern int         sc_back    (SCAN *scan);

extern int         sc_token   (SCAN *scan);
extern const char* sc_value   (SCAN *scan);
extern int         sc_len     (SCAN *scan);
extern int         sc_eof     (SCAN *scan);

extern int         sc_recover (SCAN *scan,
                               int stop, int beg, int end, int level);
extern void        sc_errfile (SCAN *scan, FILE *file, int lncnt);
extern void        sc_errmsgs (SCAN *scan, const char *msgs[], int cnt);
extern int         sc_error   (SCAN *scan, int code, ...);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define sc_fname(s)           ((const char*)(s)->fname)
#define sc_line(s)            ((s)->line)

#define sc_token(s)           ((s)->token)
#define sc_value(s)           ((const char*)(s)->value)
#define sc_len(s)             ((s)->len)

#endif  /* #ifdef SC_SCAN */
#endif
