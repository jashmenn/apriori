/*----------------------------------------------------------------------
  File    : memsys.h
  Contents: memory management system for equally sized (small) objects
  Author  : Christian Borgelt
  History : 2004.12.10 file created from fpgrowth.c
            2008.01.23 counting of used blocks added
----------------------------------------------------------------------*/
#ifndef __MEMSYS__
#define __MEMSYS__

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct _msblock {       /* --- memory system block --- */
  struct _msblock *succ;        /* successor block in list */
  void            *rsvd;        /* reserved (for alignment) */
} MSBLOCK;                      /* (memory management system block) */

typedef struct {                /* --- memory management system --- */
  int  size;                    /* size of each object */
  int  cnt;                     /* number of objects per block */
  int  blksz;                   /* size of a memory block */
  int  used;                    /* number of used objects */
  void **free;                  /* list of free objects */
  void *blocks;                 /* allocated memory blocks */
} MEMSYS;                       /* (memory management system) */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern MEMSYS* ms_create (int size, int cnt);
extern void    ms_delete (MEMSYS *ms);
extern void*   ms_alloc  (MEMSYS *ms);
extern void    ms_free   (MEMSYS *ms, void *obj);
extern int     ms_used   (MEMSYS *ms);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define ms_used(m)       ((m)->used)

#endif
