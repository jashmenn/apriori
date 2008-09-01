/*----------------------------------------------------------------------
  File    : apriori.c
  Contents: apriori algorithm for finding association rules
  Author  : Christian Borgelt
  History : 1996.02.14 file created
            1996.07.26 output precision reduced
            1996.11.22 options -b, -f, and -r added
            1996.11.24 option -e added (add. evaluation measures)
            1997.08.18 normalized chi^2 measure added
                       option -m (minimal rule length) added
            1997.10.13 quiet version (no output to stdout or stderr)
            1998.01.27 adapted to changed ist_create() function
            1998.08.08 optional input file (item appearances) added
            1998.09.02 several assertions added
            1998.09.07 hyperedge mode (option -h) added
            1998.12.08 output of absolute support (option -a) added
                       float changed to double
            1998.12.09 conversion of names to a scanable form added
            1999.02.05 long int changed to int
            1999.02.09 input from stdin, output to stdout added
            1999.08.09 bug in check of support parameter (<= 0) fixed
            1999.11.05 rule evaluation measure EM_AIMP added
            1999.11.08 output of add. rule eval. measure value added
            2000.03.16 optional use of original rule support definition
            2001.04.01 option -h replaced by option -t (target type)
            2001.05.26 extended support output added (option -x)
            2001.06.09 extended support output for item sets added
            2001.08.15 module scan used for output formatting
            2001.11.18 item and transaction functions made a module
            2001.11.19 options -C, -l changed, option -y removed
            2001.12.28 adapted to module tract, some improvements
            2002.01.11 evaluation measures codes changed to letters
            2002.02.10 option -q extended by a direction parameter
            2002.02.11 memory usage minimization option added
            2002.06.09 arbitrary supp./conf. formats made possible
            2003.01.09 option -k (item separator) added
            2003.01.14 check for empty transaction set added
            2003.03.12 output of lift value (conf/prior) added
            2003.07.17 item filtering w.r.t. usage added (option -u)
            2003.07.17 sorting w.r.t. transaction size sum added
            2003.07.18 maximal itemset filter added
            2003.08.11 closed  itemset filter added
            2003.08.15 item filtering for transaction tree added
            2003.08.16 parameter for transaction filtering added
            2003.08.18 dynamic filtering decision based on times added
            2003.08.21 option -j (heap sort for transactions) added
            2003.09.22 meaning of option -j reversed (heapsort default)
            2004.03.25 option -S added (maximal support of a set/rule)
            2004.05.09 additional selection measure for sets added
            2004.10.28 two unnecessary assignments removed
            2004.11.20 bug in evaluation of -j (heap/quicksort) fixed
            2004.11.23 absolute/relative support output changed
            2004.12.09 semantics of option -p changed
            2005.01.25 bug in output of absolute/relative support fixed
            2005.01.31 another bug in this output fixed
            2005.06.20 use of flag for "no item sorting" corrected
            2007.02.13 adapted to modified module tabscan
            2008.03.13 additional hyperedge evaluation added
            2008.03.24 additional target added (association groups)
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "apriori_wrapper.h"
#include "scan.h"
#include "tract.h"
#include "istree.h"
#ifdef STORAGE
#include "storage.h"
#endif

// #include "symbtab.h"

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define PRGNAME     "apriori"
#define DESCRIPTION "find association rules with the apriori algorithm"
#define VERSION     "version 4.35 (2008.03.24)        " \
                    "(c) 1996-2008   Christian Borgelt"

/* --- target types --- */
#define TT_SET         0        /* frequent item sets */
#define TT_CLSET       1        /* closed   item sets */
#define TT_MFSET       2        /* maximal  item sets */
#define TT_RULE        3        /* association rules */
#define TT_HEDGE       4        /* association hyperedges */
#define TT_GROUP       5        /* association groups */

/* --- error codes --- */
#define E_OPTION     (-5)       /* unknown option */
#define E_OPTARG     (-6)       /* missing option argument */
#define E_ARGCNT     (-7)       /* too few/many arguments */
#define E_STDIN      (-8)       /* double assignment of stdin */
#define E_TARGET     (-9)       /* invalid target type */
#define E_SUPP      (-10)       /* invalid support */
#define E_CONF      (-11)       /* invalid confidence */
#define E_MEASURE   (-12)       /* invalid evaluation measure */
#define E_RULELEN   (-13)       /* invalid rule length */
#define E_NOTAS     (-14)       /* no items or transactions */
#define E_NOFREQ    (-15)       /* no frequent items */
#define E_UNKNOWN   (-21)       /* unknown error */

#ifndef QUIET                   /* if not quiet version */
#ifdef FFLUSH
#define MSG(x)        x         /* print messages */
#else                           /* if to flush every output */
#define MSG(x)        x, fflush(stderr)
#endif
#else                           /* if quiet version */
#define MSG(x)                  /* suppress messages */
#endif

#define SEC_SINCE(t)  ((clock()-(t)) /(double)CLOCKS_PER_SEC)
#define RECCNT(s)     (ts_reccnt(is_tabscan(s)) \
                      - ((ts_delim(is_tabscan(s)) == TS_REC) ? 1 : 0))
#define BUFFER(s)     ts_buf(is_tabscan(s))

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/
#ifndef QUIET                   /* if not quiet version */
/* --- target types --- */
static const char *ttypes[] = {
  /* TT_SET      0 */  "set",
  /* TT_CLSET    1 */  "set",
  /* TT_MFSET    2 */  "set",
  /* TT_RULE     3 */  "rule",
  /* TT_HEDGE    4 */  "hyperedge",
  /* TT_GROUP    5 */  "group",
};

/* --- error messages --- */
static const char *errmsgs[] = {
  /* E_NONE      0 */  "no error\n",
  /* E_NOMEM    -1 */  "not enough memory\n",
  /* E_FOPEN    -2 */  "cannot open file %s\n",
  /* E_FREAD    -3 */  "read error on file %s\n",
  /* E_FWRITE   -4 */  "write error on file %s\n",
  /* E_OPTION   -5 */  "unknown option -%c\n",
  /* E_OPTARG   -6 */  "missing option argument\n",
  /* E_ARGCNT   -7 */  "wrong number of arguments\n",
  /* E_STDIN    -8 */  "double assignment of standard input\n",
  /* E_TARGET   -9 */  "invalid target type '%c'\n",
  /* E_SUPP    -10 */  "invalid minimal support %g%%\n",
  /* E_CONF    -11 */  "invalid minimal confidence %g%%\n",
  /* E_MEASURE -12 */  "invalid additional evaluation measure %c\n",
  /* E_RULELEN -13 */  "invalid set size/rule length %d\n",
  /* E_NOTAS   -14 */  "no items or transactions to work on\n",
  /* E_NOFREQ  -15 */  "no frequent items\n",
  /* E_ITEMEXP -16 */  "file %s, record %d: item expected\n",
  /* E_DUPITEM -17 */  "file %s, record %d: duplicate item %s\n",
  /* E_APPEXP  -18 */  "file %s, record %d: "
                         "appearance indicator expected\n",
  /* E_UNKAPP  -19 */  "file %s, record %d: "
                         "unknown appearance indicator %s\n",
  /* E_FLDCNT  -20 */  "file %s, record %d: too many fields\n",
  /* E_UNKNOWN -21 */  "unknown error\n"
};
#endif

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
#ifndef QUIET
static char    *prgname;        /* program name for error messages */
#endif
static ITEMSET *itemset = NULL; /* item set */
static TASET   *taset   = NULL; /* transaction set */
static TATREE  *tatree  = NULL; /* transaction tree */
static ISTREE  *istree  = NULL; /* item set tree */
static FILE    *in      = NULL; /* input  file */
static FILE    *out     = NULL; /* output file */

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

void help (void)
{                               /* --- print help on eval. measures */
  #ifndef QUIET
  fprintf(stderr, "\n");        /* terminate startup message */
  printf("additional evaluation measures (option -e#)\n");
  printf("frequent item sets:\n");
  printf("d or 1: binary logarithm of support quotient\n");
  printf("association rules:\n");
  printf("d or 1: absolute confidence difference to prior\n");
  printf("q or 2: absolute difference of confidence quotient to 1\n");
  printf("a or 3: absolute difference of improvement value to 1\n");
  printf("i or 4: information difference to prior\n");
  printf("c or 5: normalized chi^2 measure\n");
  printf("p or 6: p-value computed from chi^2 measure\n");
  #endif
  exit(0);                      /* abort the program */
}  /* help() */

/*--------------------------------------------------------------------*/

static void error (int code, ...)
{                               /* --- print an error message */
  #ifndef QUIET                 /* if not quiet version */
  va_list    args;              /* list of variable arguments */
  const char *msg;              /* error message */

  assert(prgname);              /* check the program name */
  if (code < E_UNKNOWN) code = E_UNKNOWN;
  if (code < 0) {               /* if to report an error, */
    msg = errmsgs[-code];       /* get the error message */
    if (!msg) msg = errmsgs[-E_UNKNOWN];
    fprintf(stderr, "\n%s: ", prgname);
    va_start(args, code);       /* get variable arguments */
    vfprintf(stderr, msg, args);/* print error message */
    va_end(args);               /* end argument evaluation */
  }
  #endif
  #ifndef NDEBUG                /* if debug version */
  if (istree)  ist_delete(istree);   /* clean up memory */
  if (tatree)  tat_delete(tatree);   /* and close files */
  if (taset)   tas_delete(taset, 0);
  if (itemset) is_delete(itemset);
  if (in  && (in  != stdin))  fclose(in);
  if (out && (out != stdout)) fclose(out);
  #endif
  #ifdef STORAGE                /* if storage debugging */
  showmem("at end of program"); /* check memory usage */
  #endif
  exit(code);                   /* abort the program */
}  /* error() */

/*--------------------------------------------------------------------*/

int do_apriori (int argc, char *argv[])
{                               /* --- main function */
  int    i, k = 0, n;           /* loop variables, counters */
  char   *s;                    /* to traverse the options */
  char   **optarg = NULL;       /* option argument */
  char   *fn_in   = NULL;       /* name of input  file */
  char   *fn_out  = NULL;       /* name of output file */
  char   *fn_app  = NULL;       /* name of item appearances file */
  char   *blanks  = NULL;       /* blanks */
  char   *fldseps = NULL;       /* field  separators */
  char   *recseps = NULL;       /* record separators */
  char   *comment = NULL;       /* comment indicators */
  char   *used    = NULL;       /* item usage vector */
  double supp     = 0.1;        /* minimal support    (in percent) */
  double smax     = 1.0;        /* maximal support    (in percent) */
  double conf     = 0.8;        /* minimal confidence (in percent) */
  int    mode     = IST_BODY;   /* search mode (rule support def.) */
  int    target   = 'r';        /* target type (sets/rules/h.edges) */
  int    arem     = 0;          /* additional rule evaluation measure */
  int    lift     = 0;          /* flag for printing the lift */
  double minval   = 0.1;        /* minimal evaluation measure value */
  double lftval   = 0;          /* lift value (confidence/prior) */
  int    minlen   = 1;          /* minimal rule length */
  int    maxlen   = INT_MAX;    /* maximal rule length */
  int    load     = 1;          /* flag for loading transactions */
  int    sort     = 2;          /* flag for item sorting and recoding */
  double filter   = 0.1;        /* item usage filtering parameter */
  int    tree     = 1;          /* flag for transaction tree */
  int    heap     = 1;          /* flag for heap sort vs. quick sort */
  int    c2scf    = 0;          /* flag for conv. to scanable form */
  char   *sep     = " ";        /* item separator for output */
  char   *fmt     = "%.1f";     /* output format for support/conf. */
  int    sout     = 1;          /* flag for abs./rel. support output */
  int    ext      = 0;          /* flag for extended support output */
  int    aval     = 0;          /* flag for add. eval. measure value */
  int    maxcnt   = 0;          /* maximal number of items per set */
  int    tacnt;                 /* number of transactions */
  int    frq;                   /* frequency of an item set */
  int    *map, *set;            /* identifier map, item set */
  int    verbose  = 0;          /* flag for verboseness */
  const  char *name;            /* buffer for item names */
  static char buf[4*TS_SIZE+4]; /* buffer for formatting */
  clock_t     t, tt, tc, x;     /* timer for measurements */

  #ifndef QUIET                 /* if not quiet version */
  prgname = argv[0];            /* get program name for error msgs. */

  /* --- print usage message --- */
  if (argc > 1) {               /* if arguments are given */
    fprintf(stderr, "%s - %s\n", argv[0], DESCRIPTION);
    fprintf(stderr, VERSION); } /* print a startup message */
  else {                        /* if no arguments given */
    printf("usage: %s [options] infile outfile [appfile]\n", argv[0]);
    printf("%s\n", DESCRIPTION);
    printf("%s\n", VERSION);
    printf("-t#      target type (default: association rules)\n"
           "         (s: item sets, c: closed item sets,"
                    " m: maximal item sets,\n"
           "          r: association rules,"
                    " h: association hyperedges)\n");
    printf("-m#      minimal number of items per set/rule/hyperedge "
                    "(default: %d)\n", minlen);
    printf("-n#      maximal number of items per set/rule/hyperedge "
                    "(default: no limit)\n");
    printf("-s#      minimal support    of a     set/rule/hyperedge "
                    "(default: %g%%)\n", supp *100);
    printf("-S#      maximal support    of a     set/rule/hyperedge "
                    "(default: %g%%)\n", smax *100);
    printf("-c#      minimal confidence of a         rule/hyperedge "
                    "(default: %g%%)\n", conf *100);
    printf("-o       use original definition of the support of a rule "
                    "(body & head)\n");
    printf("-k#      item separator for output "
                    "(default: \"%s\")\n", sep);
    printf("-p#      output format for support/confidence "
                    "(default: \"%s\")\n", fmt);
    printf("-x       extended support output "
                    "(print both rule support types)\n");
    printf("-a       print absolute support "
                    "(number of transactions)\n");
    printf("-y       print lift value (confidence divided by prior)\n");
    printf("-e#      additional evaluation measure (default: none)\n");
    printf("-!       print a list of additional evaluation measures\n");
    printf("-d#      minimal value of additional evaluation measure "
                    "(default: %g%%)\n", minval *100);
    printf("-v       print value of additional "
                    "rule evaluation measure\n");
    printf("-g       write output in scanable form "
                    "(quote certain characters)\n");
    printf("-l       do not load transactions into memory "
                    "(work on input file)\n");
    printf("-q#      sort items w.r.t. their frequency (default: %d)\n"
           "         (1: ascending, -1: descending, 0: do not sort,\n"
           "          2: ascending, -2: descending w.r.t. "
                    "transaction size sum)\n", sort);
    printf("-u#      filter unused items from transactions "
                    "(default: %g)\n", filter);
    printf("         (0: do not filter items w.r.t. usage in sets,\n"
           "         <0: fraction of removed items for filtering,\n"
           "         >0: take execution times ratio into account)\n");
    printf("-h       do not organize transactions as a prefix tree\n");
    printf("-j       use quicksort to sort the transactions "
                    "(default: heapsort)\n");
    printf("-z       minimize memory usage "
                    "(default: maximize speed)\n");
    printf("-b/f/r#  blank characters, field and record separators\n"
           "         (default: \" \\t\\r\", \" \\t\", \"\\n\")\n");
    printf("-C#      comment characters (default: \"#\")\n");
    printf("-V       verbose\n");

    printf("infile   file to read transactions from\n");
    printf("outfile  file to write item sets/association rules"
                    "/hyperedges to\n");
    printf("appfile  file stating item appearances (optional)\n");
    return 0;                   /* print a usage message */
  }                             /* and abort the program */
  #endif  /* #ifndef QUIET */

  /* --- evaluate arguments --- */
  for (i = 1; i < argc; i++) {  /* traverse arguments */
    s = argv[i];                /* get option argument */
    if (optarg) { *optarg = s; optarg = NULL; continue; }
    if ((*s == '-') && *++s) {  /* -- if argument is an option */
      while (*s) {              /* traverse options */
        switch (*s++) {         /* evaluate switches */
          case '!': help();                         break;
          case 't': target = (*s) ? *s++ : 'r';     break;
          case 'm': minlen = (int)strtol(s, &s, 0); break;
          case 'n': maxlen = (int)strtol(s, &s, 0); break;
          case 's': supp   = 0.01*strtod(s, &s);    break;
          case 'S': smax   = 0.01*strtod(s, &s);    break;
          case 'c': conf   = 0.01*strtod(s, &s);    break;
          case 'o': mode  |= IST_BOTH;              break;
          case 'k': optarg = &sep;                  break;
          case 'p': optarg = &fmt;                  break;
          case 'x': ext    = 1;                     break;
          case 'a': sout  |= 2;                     break;
          case 'y': lift   = 1;                     break;
          case 'e': arem   = (*s) ? *s++ : 0;       break;
          case 'd': minval = 0.01*strtod(s, &s);    break;
          case 'v': aval   = 1;                     break;
          case 'g': c2scf  = 1;                     break;
          case 'l': load   = 0;                     break;
          case 'q': sort   = (int)strtol(s, &s, 0); break;
          case 'u': filter =      strtod(s, &s);    break;
          case 'h': tree   = 0;                     break;
          case 'j': heap   = 0;                     break;
          case 'z': mode  |= IST_MEMOPT;            break;
          case 'b': optarg = &blanks;               break;
          case 'f': optarg = &fldseps;              break;
          case 'r': optarg = &recseps;              break;
          case 'C': optarg = &comment;              break;
          case 'V': verbose = 1;                    break;
          default : error(E_OPTION, *--s);          break;
        }                       /* set option variables */
        if (optarg && *s) { *optarg = s; optarg = NULL; break; }
      } }                       /* get option argument */
    else {                      /* -- if argument is no option */
      switch (k++) {            /* evaluate non-options */
        case  0: fn_in  = s;      break;
        case  1: fn_out = s;      break;
        case  2: fn_app = s;      break;
        default: error(E_ARGCNT); break;
      }                         /* note filenames */
    }
  }
  if (optarg) error(E_OPTARG);  /* check option argument */
  if ((k < 2) || (k > 3))       /* and the number of arguments */
    error(E_ARGCNT);            /* (either in/out or in/out/app) */
  if ((!fn_in || !*fn_in) && (fn_app && !*fn_app))
    error(E_STDIN);             /* stdin must not be used twice */
  switch (target) {             /* check and translate target type */
    case 's': target = TT_SET;               break;
    case 'c': target = TT_CLSET;             break;
    case 'm': target = TT_MFSET;             break;
    case 'r': target = TT_RULE;              break;
    case 'h': target = TT_HEDGE;             break;
    case 'g': target = TT_GROUP;             break;
    default : error(E_TARGET, (char)target); break;
  }
  if (supp > 1)                 /* check the minimal support */
    error(E_SUPP, supp);        /* (< 0: absolute number) */
  if ((conf  <  0) || (conf > 1))
    error(E_CONF, conf);        /* check the minimal confidence */
  if (minlen <= 0) error(E_RULELEN, minlen);  /* check the limits */
  if (maxlen <= 0) error(E_RULELEN, maxlen);  /* for the rule length */
  switch (arem) {               /* check and translate measure */
    case  0 : case '0': arem = EM_NONE;     break;
    case 'd': case '1': arem = EM_DIFF;     break;
    case 'q': case '2': arem = EM_QUOT;     break;
    case 'a': case '3': arem = EM_AIMP;     break;
    case 'i': case '4': arem = EM_INFO;     break;
    case 'c': case '5': arem = EM_CHI2;     break;
    case 'p': case '6': arem = EM_PVAL;     break;
    default : error(E_MEASURE, (char)arem); break;
  }
  if (target <= TT_MFSET) {     /* in item set mode neutralize */
    mode |= IST_BOTH; conf = 1;}/* rule specific settings */
  if (arem == EM_NONE)          /* if no add. rule eval. measure, */
    aval = 0;                   /* clear the corresp. output flag */
  if ((filter <= -1) || (filter >= 1)) filter = 0;

  /* --- create item set and transaction set --- */
  itemset = is_create(-1);      /* create an item set and */
  if (!itemset) error(E_NOMEM); /* set the special characters */
  is_chars(itemset, blanks, fldseps, recseps, comment);
  if (load) {                   /* if to load the transactions */
    taset = tas_create(itemset);
    if (!taset) error(E_NOMEM); /* create a transaction set */
  }                             /* to store the transactions */
  MSG(fprintf(stderr, "\n"));   /* terminate the startup message */

  /* --- read item appearances --- */
  if (fn_app) {                 /* if item appearances are given */
    t = clock();                /* start the timer */
    if (*fn_app)                /* if an app. file name is given, */
      in = fopen(fn_app, "r");  /* open the item appearances file */
    else {                      /* if no app. file name is given, */
      in = stdin; fn_app = "<stdin>"; }   /* read from std. input */
    MSG(fprintf(stderr, "reading %s ... ", fn_app));
    if (!in) error(E_FOPEN, fn_app);
    k = is_readapp(itemset,in); /* read the item appearances */
    if (k  != 0) error(k, fn_app, RECCNT(itemset), BUFFER(itemset));
    if (in != stdin)            /* if not read from standard input, */
      fclose(in);               /* close the input file */
    MSG(fprintf(stderr, "[%d item(s)]", is_cnt(itemset)));
    MSG(fprintf(stderr, " done [%.2fs].\n", SEC_SINCE(t)));
  }                             /* print a log message */

  /* --- read transactions --- */
  t = clock();                  /* start the timer */
  if (fn_in && *fn_in)          /* if an input file name is given, */
    in = fopen(fn_in, "r");     /* open input file for reading */
  else {                        /* if no input file name is given, */
    in = stdin; fn_in = "<stdin>"; }   /* read from standard input */
  MSG(fprintf(stderr, "reading %s ... \n", fn_in));
  if (!in) error(E_FOPEN, fn_in);
  while (1) {                   /* transaction read loop */
    k = is_read(itemset, in);   /* read the next transaction */
    if (k < 0) error(k, fn_in, RECCNT(itemset), BUFFER(itemset));
    if (k > 0) break;           /* check for error and end of file */
    k = is_tsize(itemset);      /* update the maximal */
    if (k > maxcnt) maxcnt = k; /* transaction size */
    if (taset && (tas_add(taset, NULL, 0) != 0))
      error(E_NOMEM);           /* add the loaded transaction */
  }                             /* to the transaction set */
  if (taset) {                  /* if transactions have been loaded */
    if (in != stdin) fclose(in);/* if not read from standard input, */
    in = NULL;                  /* close the input file */
  }                             /* clear the file variable */
  n     = is_cnt(itemset);      /* get the number of items */
  tacnt = is_gettac(itemset);   /* and the number of transactions */
  MSG(fprintf(stderr, "[%d item(s), %d transaction(s)]", n, tacnt));
  MSG(fprintf(stderr, " done [%.2fs].", SEC_SINCE(t)));
  if ((n <= 0) || (tacnt <= 0)) error(E_NOTAS);
  MSG(fprintf(stderr, "\n"));   /* check for at least one transaction */
  if (supp >= 0)                /* if relative support is given */
    supp = ceil(tacnt *supp);   /* compute absolute support */
  else {                        /* if absolute support is given, */
    supp = ceil(-100  *supp);   /* make the support value positive */
    if (!(sout & 2)) sout = 2;  /* switch to absolute support output */
  }                             /* do the same with the max. support */
  smax = floor(((smax >= 0) ? tacnt : -100) *smax);

  /* --- sort and recode items --- */
  MSG(fprintf(stderr, "filtering, sorting and recoding items ... "));
  t   = clock();                /* start the timer */
  map = (int*)malloc(is_cnt(itemset) *sizeof(int));
  if (!map) error(E_NOMEM);     /* create an item identifier map */
  k = (int)((mode & IST_HEAD) ? supp : ceil(supp *conf));
  n = is_recode(itemset, k, sort, map);
  if (taset) {                  /* sort and recode the items and */
    tas_recode(taset, map,n);   /* recode the loaded transactions */
    maxcnt = tas_max(taset);    /* get the new maximal t.a. size */
  }                             /* (may be smaller than before) */
  free(map);                    /* delete the item identifier map */
  MSG(fprintf(stderr, "[%d item(s)] ", n));
  MSG(fprintf(stderr, "done [%.2fs].", SEC_SINCE(t)));
  if (n <= 0) error(E_NOFREQ);  /* print a log message and */
  MSG(fprintf(stderr, "\n"));   /* check the number of items */
  if (maxlen > maxcnt)          /* clamp the set/rule length */
    maxlen = maxcnt;            /* to the maximum set size */

  /* --- create a transaction tree --- */
  tt = 0;                       /* init. the tree construction time */
  if (tree && taset) {          /* if transactions were loaded */
    MSG(fprintf(stderr, "creating transaction tree ... "));
    t = clock();                /* start the timer */
    tatree = tat_create(taset, heap); 
    if (!tatree) error(E_NOMEM);/* create a transaction tree */
    if (filter == 0) {          /* if a tree rebuild is not needed, */
      tas_delete(taset, 0); taset = NULL; }  /* delete transactions */
    tt = clock() -t;            /* note the time for the construction */
    MSG(fprintf(stderr, "done [%.2fs].\n", SEC_SINCE(t)));
  }                             /* print a log message */

  /* --- create an item set tree --- */
  t = clock(); tc = 0;          /* start the timer */
  istree = ist_create(itemset, mode, (int)supp, conf);
  if (!istree) error(E_NOMEM);  /* create an item set tree */

  /* --- check item subsets --- */
  if (filter) {                 /* if to filter unused items */
    used = (char*)malloc(is_cnt(itemset) *sizeof(char));
    if (!used) error(E_NOMEM);  /* create a flag vector */
  }                             /* for the items */
  MSG(fprintf(stderr, "checking subsets of size 1"));
  while (ist_height(istree) < maxlen) {
    if (filter != 0) {          /* if to filter w.r.t. item usage, */
      i = ist_check(istree, used);     /* check current item usage */
      if (i < maxlen) maxlen = i;      /* update the maximum size */
      if (ist_height(istree) >= i) break;
    }                           /* check the tree height */
    k = ist_addlvl(istree);     /* while max. height is not reached, */
    if (k <  0) error(E_NOMEM); /* add a level to the item set tree */
    if (k != 0) break;          /* if no level was added, abort */
    MSG(fprintf(stderr, " %d", ist_height(istree)));
    if (tatree) {               /* if a transaction tree was created */
      if (((filter < 0)         /* if to filter w.r.t. item usage */
      &&   (i < -filter *n))    /* and enough items were removed */
      ||  ((filter > 0)         /* or counting time is long enough */
      &&   (i < n) && (i *(double)tt < filter *n *tc))) {
        n = i; x = clock();     /* note the new number of items */
        tas_filter(taset, used);/* and remove unnecessary items */
        tat_delete(tatree);     /* delete the transaction tree */
        tatree = tat_create(taset, heap);
        if (!tatree) error(E_NOMEM);
        tt = clock() -x;        /* rebuild the transaction tree and */
      }                         /* note the new construction time */
      x  = clock();             /* count the transaction tree */
      ist_countx(istree, tatree);
      tc = clock() -x; }        /* note the new count time */
    else if (taset) {           /* if transactions were loaded */
      if (((filter < 0)         /* if to filter w.r.t. item usage */
      &&   (i <= -filter *n))   /* and enough items were removed */
      ||  ((filter > 0)         /* or counting time is long enough */
      &&   (i *(double)tt <= filter *n *tc))) {
        n = i; x = clock();     /* note the new number of items */
        tas_filter(taset, used);/* and remove unnecessary items */
        tt = clock() -t;        /* from the transactions */
      }                         /* note the filtering time */
      for (i = tacnt; --i >= 0;)/* traverse and count transactions */
        ist_count(istree, tas_tract(taset, i), tas_tsize(taset, i));
      tc = clock() -t; }        /* note the new count time */
    else {                      /* if to work on the input file, */
      rewind(in);               /* reset the file position */
      for (maxcnt = 0; (i = is_read(itemset, in)) == 0; ) {
        if (filter != 0)        /* (re)read the transactions and */
          is_filter(itemset, used);  /* remove unnecessary items */
        k = is_tsize(itemset);  /* update the maximum size */
        if (k > maxcnt) maxcnt = k;  /* of a transaction */
        ist_count(istree, is_tract(itemset), k);
      }                         /* count the transaction in the tree */
      if (i < 0) error(i, fn_in, RECCNT(itemset), BUFFER(itemset));
      if (maxcnt < maxlen)      /* update the maximal rule length */
        maxlen = maxcnt;        /* according to the max. t.a. size */
    }                           /* (may be smaller than before) */
  }
  if (!taset && !tatree) {      /* if transactions were not loaded */
    if (in != stdin) fclose(in);/* if not read from standard input, */
    in = NULL;                  /* close the input file */
  }                             /* clear the file variable */
  MSG(fprintf(stderr, " done [%.2fs].\n", SEC_SINCE(t)));

  /* --- filter found item sets --- */
  if ((target == TT_CLSET) || (target == TT_MFSET)) {
    MSG(fprintf(stderr, "filtering %s item sets ... ",
        (target == TT_MFSET) ? "maximal" : "closed"));
    t = clock();                /* filter the item sets */
    ist_filter(istree, (target == TT_MFSET) ? IST_MAXFRQ : IST_CLOSED);
    MSG(fprintf(stderr, "done [%.2fs].\n", SEC_SINCE(t)));
  }                             /* (filter takes longer than print) */

  /* --- sort transactions --- */
  if (target <= TT_MFSET) {     /* if to find frequent item sets */
    if (!taset)                 /* transactions must be loaded */
      ext = 0;                  /* for extended support output */
    else if (ext) {             /* if extended output is requested */
      MSG(fprintf(stderr, "sorting transactions ... "));
      t = clock();              /* start the timer */
      tas_sort(taset, heap);    /* sort the transactions */
      MSG(fprintf(stderr, "done [%.2fs].\n", SEC_SINCE(t)));
    }                           /* (sorting is necessary to find the */
  }                             /* number of identical transactions) */

  /* --- print item sets/rules/hyperedges --- */
  t = clock();                  /* start the timer */
  if (fn_out && *fn_out)        /* if an output file name is given, */
    out = fopen(fn_out, "w");   /* open the output file */
  else {                        /* if no output file name is given, */
    out = stdout; fn_out = "<stdout>"; }    /* write to std. output */
  MSG(fprintf(stderr, "writing %s ... ", fn_out));
  if (!out) error(E_FOPEN, fn_out);
  ist_init(istree, minlen, arem, minval);
  set = is_tract(itemset);      /* get the transaction buffer */
  if (target <= TT_MFSET) {     /* if to find frequent item sets */
    for (n = 0; 1; ) {          /* extract item sets from the tree */
      k = ist_set(istree, set, &frq, &conf);
      if (k <= 0) break;        /* get the next frequent item set */
      if (frq > smax) continue; /* check against maximal support */
      for (i = 0; i < k; i++) { /* traverse the set's items */
        name = is_name(itemset, set[i]);
        if (c2scf) { sc_format(buf, name, 0); name = buf; }
        fputs(name, out);       /* print the name of the next item */
        fputs((i < k-1) ? sep : " ", out);
      }                         /* print a separator */
      fputs(" (", out);         /* print the item set's support */
      if (sout & 1) { fprintf(out, fmt, (frq/(double)tacnt) *100);
                      if (sout & 2) fputc('/', out); }
      if (sout & 2) { fprintf(out, "%d", frq); }
      if (ext) {                /* if to print the extended support */
        frq = tas_occur(taset, set, k);
        fputs(", ", out);       /* get the number of occurrences */
        fprintf(out, fmt, (frq/(double)tacnt) *100);
        if (sout & 2) fprintf(out, "/%d", frq);
      }                         /* print the extended support data */
      if (aval) { fputs(", ", out); fprintf(out, fmt, conf *100); }
      fputs(")\n", out);        /* print the add. eval. measure, */
      n++;                      /* terminate the support output, */
    } }                         /* and count the item set */
  else if (target == TT_RULE) { /* if to find association rules, */
    for (n = 0; 1; ) {          /* extract rules from tree */
      k = ist_rule(istree, set, &frq, &conf, &lftval, &minval);
      if (k <= 0) break;        /* get the next association rule */
      if (frq > smax) continue; /* check against maximal support */
      for (i = 0; i < k; i++) { /* traverse the rule's items */
        name = is_name(itemset, set[i]);
        if (c2scf) { sc_format(buf, name, 0); name = buf; }
        fputs(name, out);       /* print the next item */
        fputs((i <= 0) ? " <- " : ((i < k-1) ? sep : " "), out);
      }                         /* print a separator */
      fputs(" (", out);         /* print the rule evaluation */
      if (sout & 1) supp = frq/(double)tacnt;
      if (ext && !(mode & IST_HEAD)) {
        if (sout & 1) { fprintf(out, fmt, supp *conf *100);
                        if (sout & 2) fputc('/', out); }
        if (sout & 2) { fprintf(out, "%d", (int)(frq *conf +0.5));}
        fputs(", ", out);       /* print the support of the rule */
      }                         /* from  the support of the body */
      if (sout & 1) { fprintf(out, fmt, supp *100);
                      if (sout & 2) fputc('/', out); }
      if (sout & 2) { fprintf(out, "%d", frq); }
      fputs(", ", out);         /* print the rule support */
      if (ext && (mode & IST_HEAD)) {
        if (sout & 1) { fprintf(out, fmt, (supp/conf) *100);
                        if (sout & 2) fputc('/', out); }
        if (sout & 2) { fprintf(out, "%d", (int)(frq /conf +0.5));}
        fputs(", ", out);       /* print the support of the body */
      }                         /* from  the support of the rule */
      fprintf(out, fmt, conf *100); /* print the rule confidence */
      if (lift) { fputs(", ", out); fprintf(out, fmt, lftval *100); }
      if (aval) { fputs(", ", out); fprintf(out, fmt, minval *100); }
      fputs(")\n", out);        /* print the value of the additional */
      n++;                      /* rule evaluation measure and */
    } }                         /* count the association rule */
  else if (target == TT_HEDGE){ /* if to find association hyperedges */
    for (n = 0; 1; ) {          /* extract hyperedges from tree */
      k = ist_hedge(istree, set, &frq, &conf, &minval);
      if (k <= 0) break;        /* get the next hyperedge */
      if (frq > smax) continue; /* check against maximal support */
      for (i = 0; i < k; i++) { /* traverse the edge's items */
        name = is_name(itemset, set[i]);
        if (c2scf) { sc_format(buf, name, 0); name = buf; }
        fputs(name, out);       /* print the name of the next item */
        fputs((i < k-1) ? sep : " ", out);
      }                         /* print a separator */
      fputs(" (", out);         /* print the hyperedge evaluation */
      if (sout & 1) { fprintf(out, fmt, (frq/(double)tacnt) *100);
                      if (sout & 2) fputc('/', out); }
      if (sout & 2) { fprintf(out, "%d", frq); }
      fputs(", ", out); fprintf(out, fmt, conf *100);
      if (aval) { fputs(", ", out); fprintf(out, fmt, minval *100); }
      fputs(")\n", out);        /* print support and confidence */
      n++;                      /* of the hyperedge and */
    } }                         /* count the hyperedge */
  else {                        /* if to find association groups */
    for (n = 0; 1; ) {          /* extract groups from tree */
      k = ist_group(istree, set, &frq, &minval);
      if (k <= 0) break;        /* get the next group */
      if (frq > smax) continue; /* check against maximal support */
      for (i = 0; i < k; i++) { /* traverse the group's items */
        name = is_name(itemset, set[i]);
        if (c2scf) { sc_format(buf, name, 0); name = buf; }
        fputs(name, out);       /* print the name of the next item */
        fputs((i < k-1) ? sep : " ", out);
      }                         /* print a separator */
      fputs(" (", out);         /* print the group evaluation */
      if (sout & 1) { fprintf(out, fmt, (frq/(double)tacnt) *100);
                      if (sout & 2) fputc('/', out); }
      if (sout & 2) { fprintf(out, "%d", frq); }
      if (aval) { fputs(", ", out); fprintf(out, fmt, minval *100); }
      fputs(")\n", out);        /* print support and add. measure */
      n++;                      /* and count the group */
    }
  }  /* if (target <= TT_MFSET) .. else .. */
  if (fflush(out) != 0) error(E_FWRITE, fn_out);
  if (out != stdout) fclose(out);
  out = NULL;                   /* close the output file */
  MSG(fprintf(stderr, "[%d %s(s)] done ", n, ttypes[target]));
  MSG(fprintf(stderr, "[%.2fs].\n", SEC_SINCE(t)));
  #ifdef BENCH
  printf("number of support counters: %d\n", istree->sccnt);
  printf("necessary support counters: %d\n", istree->scnec);
  printf("number of child pointers  : %d\n", istree->cpcnt);
  printf("necessary child pointers  : %d\n", istree->cpnec);
  printf("allocated memory (bytes)  : %d\n", istree->bytes);
  #endif

  /* --- clean up --- */
  #ifndef NDEBUG                /* if this is a debug version */
  free(used);                   /* delete the item app. vector */
  ist_delete(istree);           /* delete the item set tree, */
  if (tatree) tat_delete(tatree);     /* the transaction tree, */
  if (taset)  tas_delete(taset, 0);   /* the transaction set, */
  is_delete(itemset);                 /* and the item set */
  #endif
  #ifdef STORAGE                /* if storage debugging */
  showmem("at end of program"); /* check memory usage */
  #endif
  return 0;                     /* return 'ok' */
}  /* main() */
