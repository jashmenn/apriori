/*----------------------------------------------------------------------
  File    : tabscan.c
  Contents: table scanner management
  Author  : Christian Borgelt
  History : 1998.01.04 file created
            1998.03.11 additional character flags enabled
            1998.08.12 function ts_copy() added
            1998.09.01 several assertions added
            1998.09.27 function ts_getfld() improved
            1998.10.21 bug in ts_sgetc() removed
            1998.11.26 some function parameters changed to const
            1999.02.04 long int changed to int
            1999.11.16 number of characters cleared for an empty field
            2000.12.01 '\r' made a default blank character
            2001.07.14 ts_sgetc() modified, ts_buf() and ts_err() added
            2001.08.19 last delimiter stored in TABSCAN structure
            2002.02.11 ts_reccnt() and ts_reset() added
            2006.10.06 result value policy of ts_getfld() improved
            2007.02.13 renamed to tabscan, redesigned, TS_NULL added
            2007.05.17 function ts_allchs() added
            2007.09.02 made '*' a null value character by default
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tabscan.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- convenience functions --- */
#define isrecsep(c)   ts_istype(tsc, TS_RECSEP,  c)
#define isfldsep(c)   ts_istype(tsc, TS_FLDSEP,  c)
#define issep(c)      ts_istype(tsc, TS_FLDSEP|TS_RECSEP, c)
#define isblank(c)    ts_istype(tsc, TS_BLANK,   c)
#define isnull(c)     ts_istype(tsc, TS_NULL,    c)
#define iscomment(c)  ts_istype(tsc, TS_COMMENT, c)

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

TABSCAN* ts_create (void)
{                               /* --- create a table scanner */
  TABSCAN *tsc;                 /* created table scanner */
  int     i;                    /* loop variable */
  char    *p;                   /* to traverse character flags */

  tsc = (TABSCAN*)malloc(sizeof(TABSCAN));
  if (!tsc) return NULL;        /* allocate memory and */
  tsc->reccnt = 1;              /* initialize the fields */
  tsc->delim  = TS_EOF;
  for (p = tsc->cflags +256, i = 256; --i >= 0; )
    *--p = '\0';                /* initialize the character flags */
  tsc->cflags['\n'] = TS_RECSEP;
  tsc->cflags['\t'] = tsc->cflags[' '] = TS_BLANK|TS_FLDSEP;
  tsc->cflags['\r'] = TS_BLANK;
  tsc->cflags[',']  = TS_FLDSEP;
  tsc->cflags['?']  = tsc->cflags['*'] = TS_NULL;
  tsc->cflags['#']  = TS_COMMENT;
  return tsc;                   /* return created table scanner */
}  /* ts_create() */

/*--------------------------------------------------------------------*/

void ts_copy (TABSCAN *dst, const TABSCAN *src)
{                               /* --- copy character flags */
  int  i;                       /* loop variable */
  char *d; const char *s;       /* to traverse the character flags */

  assert(src && dst);           /* check the function arguments */
  s = src->cflags +256; d = dst->cflags +256;
  for (i = 256; --i >= 0; ) *--d = *--s;
}  /* ts_copy() */              /* copy the character flags */

/*--------------------------------------------------------------------*/

int ts_chars (TABSCAN *tsc, int type, const char *chars)
{                               /* --- set characters of a class */
  int  i, c, d;                 /* loop variable, characters */
  char *p;                      /* to traverse character flags */
  char const **s;

  assert(tsc);                  /* check argument */
  if (!chars) return -1;        /* if no characters given, abort */
  p = tsc->cflags +256;         /* clear character flags in type */
  for (i = 256; --i >= 0; ) *--p &= (char)~type;
  s = &chars;                   /* traverse the given characters */
  for (c = d = ts_decode(s); c >= 0; c = ts_decode(s))
    tsc->cflags[c] |= (char)type;  /* set character flags */
  return (d >= 0) ? d : 0;      /* return first character */
}  /* ts_chars() */

/*--------------------------------------------------------------------*/

void ts_allchs (TABSCAN *tsc, const char *recseps, const char *fldseps,
                const char *blanks, const char *nullchs,
                const char *comment)
{                               /* --- set characters of all classes */
  if (recseps != NULL) ts_chars(tsc, TS_RECSEP,  recseps);
  if (fldseps != NULL) ts_chars(tsc, TS_FLDSEP,  fldseps);
  if (blanks  != NULL) ts_chars(tsc, TS_BLANK,   blanks);
  if (nullchs != NULL) ts_chars(tsc, TS_NULL,    nullchs);
  if (comment != NULL) ts_chars(tsc, TS_COMMENT, comment);
}  /* ts_allchs() */

/*--------------------------------------------------------------------*/

int ts_next (TABSCAN *tsc, FILE *file, char *buf, int len)
{                               /* --- read the next table field */
  int  c, d;                    /* character read, delimiter type */
  char *p;                      /* to traverse the buffer */

  assert(tsc && (!buf || (len >= 0)));  /* check function argumens */

  /* --- initialize --- */
  if (!buf) {                   /* if no buffer given, use internal */
    buf = tsc->buf; len = TS_SIZE; }
  p = buf; *p = '\0';           /* clear the read buffer and */
  tsc->cnt = 0;                 /* the number of characters read */
  c = getc(file);               /* get the first character and */
  if (c == EOF)                 /* check for end of file/error */
    return tsc->delim = (ferror(file)) ? TS_ERR : TS_EOF;

  /* --- skip comment records --- */
  if (tsc->delim != 0) {        /* if at the start of a record */
    while (iscomment(c)) {      /* while the record is a comment */
      tsc->reccnt++;            /* count the record to be read */
      while (!isrecsep(c)) {    /* while not at end of record */
        c = getc(file);         /* get the next character and */
        if (c == EOF)           /* check for end of file/error */
          return tsc->delim = (ferror(file)) ? TS_ERR : TS_EOF;
      }                         /* (read up to a record separator) */
      c = getc(file);           /* get the next character and */
      if (c == EOF)             /* check for end of file/error */
        return tsc->delim = (ferror(file)) ? TS_ERR : TS_EOF;
    }              
  }                             /* (comment records are skipped) */

  /* --- skip leading blanks --- */
  while (isblank(c)) {          /* while character is blank, */
    c = getc(file);             /* get the next character and */
    if (c == EOF)               /* check for end of file/error */
      return tsc->delim = (ferror(file)) ? TS_ERR : TS_REC;
  }                             /* check for end of file */
  if (issep(c)) {               /* check for field/record separator */
    if (isfldsep(c)) return tsc->delim = TS_FLD;
    tsc->reccnt++;   return tsc->delim = TS_REC;
  }                             /* if at end of record, count reocrd */
  /* Note that after at least one valid character was read, even  */
  /* if it is a blank, the end of file/input is translated into a */
  /* record separator. -1 is returned only if no character could  */
  /* be read before the end of file/input is encountered.         */

  /* --- read the field --- */
  while (1) {                   /* field read loop */
    if (len > 0) {              /* if the buffer is not full, */
      len--; *p++ = (char)c; }  /* store the character in the buffer */
    c = getc(file);             /* get the next character */
    if (issep(c)) { d = (isfldsep(c))  ? TS_FLD : TS_REC; break; }
    if (c == EOF) { d = (ferror(file)) ? TS_ERR : TS_REC; break; }
  }                             /* while character is no separator */

  /* --- remove trailing blanks --- */
  while (isblank(*--p));        /* while character is blank */
  *++p = '\0';                  /* terminate string in buffer */
  tsc->cnt = (int)(p -buf);     /* store number of characters read */
  if (d != TS_FLD) {            /* if not at a field separator */
    if (d == TS_REC) tsc->reccnt++;
    return tsc->delim = d;      /* if at end of record, count record, */
  }                             /* and then abort the function */

  /* --- check for a null value --- */
  while (--p >= buf)            /* check for only null value chars. */
    if (!isnull((unsigned char)*p)) break;
  if (p < buf) buf[0] = '\0';   /* clear buffer if null value */
  
  /* --- skip trailing blanks --- */
  while (isblank(c)) {          /* while character is blank, */
    c = getc(file);             /* get the next character and */
    if (c == EOF)               /* check for end of file/error */
      return tsc->delim = ferror(file) ? TS_ERR : TS_REC;
  }                             /* check for end of file */
  if (isrecsep(c)) {            /* check for a record separator */
    tsc->reccnt++; return tsc->delim = TS_REC; }
  if (!isfldsep(c))             /* put back character (may be */
    ungetc(c, file);            /* necessary if blank = field sep.) */
  return tsc->delim = TS_FLD;   /* return the delimiter type */
}  /* ts_next() */

/*--------------------------------------------------------------------*/

void ts_reset (TABSCAN *tsc)
{                               /* --- reset a table scanner */
  tsc->reccnt =  1;             /* reset the record counter */
  tsc->delim  = -1;             /* and the field delimiter */
}  /* ts_reset() */

/*--------------------------------------------------------------------*/

int ts_decode (char const **s)
{                               /* --- decode ASCII character codes */
  int c, code;                  /* character and character code */

  assert(s && *s);              /* check the function arguments */
  if (**s == '\0')              /* if at the end of the string, */
    return -1;                  /* abort the function */
  c = (unsigned char)*(*s)++;   /* get the next character */
  if (c != '\\')                /* if no quoted character, */
    return c;                   /* simply return the character */
  c = (unsigned char)*(*s)++;   /* get the next character */
  switch (c) {                  /* and evaluate it */
    case 'a': return '\a';      /* 0x07 (BEL) */
    case 'b': return '\b';      /* 0x08 (BS)  */
    case 'f': return '\f';      /* 0x0c (FF)  */
    case 'n': return '\n';      /* 0x0a (NL)  */
    case 'r': return '\r';      /* 0x0d (CR)  */
    case 't': return '\t';      /* 0x09 (HT)  */
    case 'v': return '\v';      /* 0x0b (VT)  */
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
      code = c -'0';            /* --- octal character code */
      c    = **s;               /* get the next character */
      if ((c >= '0') && (c <= '7')) code = (code << 3) +c -'0';
      else return code;         /* decode second digit */
      c    = *++(*s);           /* get the next character */
      if ((c >= '0') && (c <= '7')) code = (code << 3) +c -'0';
      else return c;            /* decode third digit */
      (*s)++;                   /* consume the decoded character */
      return code & 0xff;       /* and return the character code */
    case 'x':                   /* --- hexadecimal character code */
      c = **s;                  /* get the next character */
      if      ((c >= '0') && (c <= '9')) code = c -'0';
      else if ((c >= 'a') && (c <= 'f')) code = c -'a' +10;
      else if ((c >= 'A') && (c <= 'F')) code = c -'A' +10;
      else return 'x';          /* decode first digit */
      c = *++(*s);              /* get the next character */
      if      ((c >= '0') && (c <= '9')) code = (code << 4) +c -'0';
      else if ((c >= 'a') && (c <= 'f')) code = (code << 4) +c -'a' +10;
      else if ((c >= 'A') && (c <= 'F')) code = (code << 4) +c -'A' +10;
      else return code;         /* decode second digit */
      (*s)++;                   /* consume the decoded character */
      return code;              /* and return the character code */
    default:                    /* non-function characters */
      if (**s == '\0') return '\\';
      else             return (unsigned char)*(*s)++;
  }                             /* return character or backslash */
}  /* ts_decode() */

/*--------------------------------------------------------------------*/
#if 0

int main (int argc, char* argv[])
{                               /* --- main function for testing */
  int     d;                    /* delimiter of current field */
  FILE    *file;                /* file to read */
  TABSCAN *tsc;                 /* table scanner for testing */
  char    buf[256];             /* read buffer */

  if (argc < 2) {               /* if no arguments given, abort */
    printf("usage: %s file\n", argv[0]); return 0; }
  file = fopen(argv[1], "rb");  /* open the input file */
  if (!file) { printf("cannot open %s\n", argv[1]); return -1; }
  tsc = ts_create();            /* create a table scanner */
  if (!tsc)  { printf("not enough memory\n");       return -1; }
  ts_chars(tsc, TS_COMMENT, "#");
  do {                          /* file read loop */
    d = ts_next(tsc, file, buf, sizeof(buf));
    printf("%d : %s\n", d, buf);/* print delimiter and field */
  } while (d >= 0);             /* while not at end of file */
  ts_delete(tsc);               /* delete the table scanner */
  fclose(file);                 /* and close the input file */
  return 0;                     /* return 'ok' */
}  /* main() */

#endif
