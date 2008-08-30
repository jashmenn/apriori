/*----------------------------------------------------------------------
  File    : parse.h
  Contents: parser utilities
  Author  : Christian Borgelt
  History : 2004.08.12 file created
            2006.02.02 error E_EDGE   added
            2007.01.16 error E_MSDCNT added
----------------------------------------------------------------------*/
#ifndef __PARSE__
#define __PARSE__
#ifndef SC_SCAN
#define SC_SCAN
#endif
#include "scan.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
/* --- error codes --- */
#define E_CHREXP    (-16)       /* character expected */
#define E_STREXP    (-17)       /* string expected */
#define E_NUMEXP    (-18)       /* number expected */
#define E_NUMBER    (-19)       /* invalid number */

#define E_ATTEXP    (-20)       /* attribute expected */
#define E_UNKATT    (-21)       /* unknown attribute */
#define E_DUPATT    (-22)       /* duplicate attribute value */
#define E_MISATT    (-23)       /* missing attribute */
#define E_ATTRIB    (-24)       /* invalid attribute */
#define E_ATTYPE    (-25)       /* wrong attribute type */

#define E_VALEXP    (-26)       /* attribute value expected */
#define E_UNKVAL    (-27)       /* unknown attribute value */
#define E_DUPVAL    (-28)       /* duplicate attribute value */
#define E_MISVAL    (-29)       /* missing attribute value */

#define E_CLSEXP    (-30)       /* class value expected */
#define E_UNKCLS    (-31)       /* unknown class value */
#define E_DUPCLS    (-32)       /* duplicate class value */
#define E_MISCLS    (-33)       /* missing class value */
#define E_CLSTYPE   (-34)       /* class attribute must be nominal */
#define E_CLSCNT    (-35)       /* class attribute has too few values */

#define E_DOMAIN    (-36)       /* invalid attribute domain */

#define E_PAREXP    (-37)       /* parameter expected */
#define E_CMPOP     (-38)       /* invalid comparison operator */
#define E_COVMAT    (-39)       /* invalid covariance matrix */

#define E_DUPCDL    (-40)       /* duplicate candidate list */
#define E_RANGE     (-41)       /* invalid candidate range */
#define E_CAND      (-42)       /* invalid candidate */
#define E_LINK      (-43)       /* invalid link */

#define E_LYRCNT    (-44)       /* invalid number of layers */
#define E_UNITCNT   (-45)       /* invalid number of units */

#define E_EDGE      (-46)       /* invalid edge type */

#define E_MSDCNT    (-47)       /* wrong number of membership degrees */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern void pa_init  (SCAN *scan);
extern int  pa_error (SCAN *scan, int code, int c, const char *s);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ERROR(c)    return pa_error(scan, c,        -1, NULL)
#define XERROR(c,s) return pa_error(scan, c,        -1, s)
#define ERR_CHR(c)  return pa_error(scan, E_CHREXP,  c, NULL)
#define ERR_STR(s)  return pa_error(scan, E_STREXP, -1, s)
#define GET_TOK()   if (sc_next(scan) < 0) \
                      return sc_error(scan, sc_token(scan))
#define GET_CHR(c)  if (sc_token(scan) != (c)) ERR_CHR(c); \
                      else GET_TOK()
#define RECOVER()   if (sc_recover(scan, ';', '{', '}', 0) == T_EOF) \
                      return 1
#endif
