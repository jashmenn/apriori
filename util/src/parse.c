/*----------------------------------------------------------------------
  File    : parse.h
  Contents: parser utilities
  Author  : Christian Borgelt
  History : 2004.08.12 file created
            2006.02.02 error E_EDGE added
            2007.01.16 error E_MSDCNT added
----------------------------------------------------------------------*/
#include <string.h>
#include <assert.h>
#include "parse.h"

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
#ifdef GERMAN                     /* deutsche Texte */
static const char *errmsgs[] = {  /* Fehlermeldungen */
  /* E_CHREXP  -16 */  "\"%c\" erwartet statt %s",
  /* E_STREXP  -17 */  "\"%s\" erwartet statt %s",
  /* E_NUMEXP  -18 */  "Zahl erwartet statt %s",
  /* E_NUMBER  -19 */  "ungültige Zahl %s",

  /* E_ATTEXP  -20 */  "Attribut erwartet statt %s",
  /* E_UNKATT  -21 */  "unbekanntes Attribut %s",
  /* E_DUPATT  -22 */  "doppeltes Attribut %s",
  /* E_MISATT  -23 */  "Attribut %s fehlt",
  /* E_ATTRIB  -24 */  "ungültiges Attribut %s",
  /* E_ATTYPE  -25 */  "Attribut %s hat falschen Typ",

  /* E_VALEXP  -26 */  "Attributwert erwartet statt %s",
  /* E_UNKVAL  -27 */  "unbekannter Attributwert %s",
  /* E_DUPVAL  -28 */  "doppelter Attributwert %s",
  /* E_MISVAL  -29 */  "fehlender Attributwert %s",

  /* E_CLSEXP  -30 */  "Klassenattribut erwartet statt %s",
  /* E_UNKCLS  -31 */  "unbekannte Klasse %s",
  /* E_DUPCLS  -32 */  "doppelte Klasse %s",
  /* E_MISCLS  -33 */  "Klasse %s fehlt",
  /* E_CLSTYPE -34 */  "Klassenattribut %s hat falschen Typ",
  /* E_CLSCNT  -35 */  "Klassenattribut %s hat zu wenige Werte",

  /* E_DOMAIN  -36 */  "ungültiger Wertebereich %s",

  /* E_PAREXP  -37 */  "Parameter erwartet statt %s",
  /* E_CMPOP   -38 */  "ungültiger Vergleichsoperator %s",
  /* E_COVMAT  -39 */  "ungültige Kovarianzmatrix",

  /* E_DUPCDL  -40 */  "doppelte Kandidatenliste für Attribut %s\n",
  /* E_RANGE   -41 */  "ungültiger Kandidatenbereich",
  /* E_CAND    -42 */  "ungültiger Kandidat %s",
  /* E_LINK    -43 */  "ungültiger Verweis",

  /* E_LYRCNT  -44 */  "ungültige Anzahl Schichten",
  /* E_UNITCNT -45 */  "ungültige Anzahl Einheiten",

  /* E_EDGE    -46 */  "ungültiger Kantentyp %s",

  /* E_MSDCNT  -47 */  "falsche Anzahl Zugehörigkeitsgrade",
};
#else                             /* English texts */
static const char *errmsgs[] = {  /* error messages */
  /* E_CHREXP  -16 */  "\"%c\" expected instead of %s",
  /* E_STREXP  -17 */  "\"%s\" expected instead of %s",
  /* E_NUMEXP  -18 */  "number expected instead of %s",
  /* E_NUMBER  -19 */  "invalid number %s",

  /* E_ATTEXP  -20 */  "attribute expected instead of %s",
  /* E_UNKATT  -21 */  "unknown attribute %s",
  /* E_DUPATT  -22 */  "duplicate attribute %s",
  /* E_MISATT  -23 */  "missing attribute %s",
  /* E_ATTRIB  -24 */  "invalid attribute %s",
  /* E_ATTYPE  -25 */  "attribute %s has wrong type",

  /* E_VALEXP  -26 */  "attribute value expected instead of %s",
  /* E_UNKVAL  -27 */  "unknown attribute value %s",
  /* E_DUPVAL  -28 */  "duplicate attribute value %s",
  /* E_MISVAL  -29 */  "missing attribute value %s",

  /* E_CLSEXP  -30 */  "class value expected instead of %s",
  /* E_UNKCLS  -31 */  "unknown class value %s",
  /* E_DUPCLS  -32 */  "duplicate class value %s",
  /* E_MISCLS  -33 */  "missing class value %s",
  /* E_CLSTYPE -34 */  "class attribute %s has wrong type",
  /* E_CLSCNT  -35 */  "class attribute %s has too few values",

  /* E_DOMAIN  -36 */  "invalid attribute domain %s",

  /* E_PAREXP  -37 */  "parameter expected instead of %s",
  /* E_CMPOP   -38 */  "invalid comparison operator %s",
  /* E_COVMAT  -39 */  "invalid covariance matrix",

  /* E_DUPCDL  -40 */  "duplicate candidate list for attribute %s\n",
  /* E_RANGE   -41 */  "invalid candidate range",
  /* E_CAND    -42 */  "invalid candidate %s",
  /* E_LINK    -43 */  "invalid link",

  /* E_LYRCNT  -44 */  "invalid number of layers",
  /* E_UNITCNT -45 */  "invalid number of units",

  /* E_EDGE    -46 */  "invalid edge type %s",

  /* E_MSDCNT  -47 */  "wrong number of membership degrees",
};
#endif
#define MSGCNT  (int)(sizeof(errmsgs)/sizeof(const char*))

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

void pa_init (SCAN *scan)       /* --- initialize parsing */
{ sc_errmsgs(scan, errmsgs, MSGCNT); }

/*--------------------------------------------------------------------*/

int pa_error (SCAN *scan, int code, int c, const char *s)
{                               /* --- report a parse error */
  char src[256], dst[1024];     /* buffers for string formating */

  assert(scan);                 /* check the function arguments */
  if (((code == E_DUPATT) || (code == E_MISATT)
  ||   (code == E_DUPVAL) || (code == E_MISVAL)
  ||   (code == E_DUPCLS) || (code == E_MISCLS)
  ||   (code == E_CAND))  && s)
    sc_format(dst, s,   1);     /* if "missing ..." error message, */
  else {                        /* format the given name */
    strncpy(src, sc_value(scan), 255); src[255] = '\0';
    sc_format(dst, src, 1);     /* if normal error message, */
  }                             /* copy and format the token value */
  if      (code == E_CHREXP) return sc_error(scan, code, c, dst);
  else if (code == E_STREXP) return sc_error(scan, code, s, dst);
  else                       return sc_error(scan, code,    dst);
}  /* _paerr() */               /* print an error message */
