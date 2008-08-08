/*----------------------------------------------------------------------
  File    : memsys.c
  Contents: memory management system for equally sized (small) objects
  Author  : Christian Borgelt
  History : 2004.12.10 file created from fpgrowth.c
            2008.01.23 counting of used objects added
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "memsys.h"
#ifdef STORAGE
#include "storage.h"
#endif

/*----------------------------------------------------------------------
  Main Functions
----------------------------------------------------------------------*/

MEMSYS* ms_create (int size, int cnt)
{                               /* --- create a memory system */
  MEMSYS *ms;                   /* created memory system */

  assert((cnt > 0)              /* check the function arguments */
      && (size > 0) && (size % sizeof(void*) == 0));
  ms = malloc(sizeof(MEMSYS));  /* create a memory management system */
  if (!ms) return NULL;         /* with no free objects and */
  ms->cnt   = cnt;              /* initialize the variables */
  ms->size  = size /sizeof(void*);
  ms->blksz = sizeof(MSBLOCK) +ms->cnt *size;
  ms->free  = ms->blocks = NULL;
  return ms;                    /* return the created memory system */
}  /* ms_create() */

/*--------------------------------------------------------------------*/

void ms_delete (MEMSYS *ms)
{                               /* --- delete a memory system */
  MSBLOCK *block;               /* to traverse the memory blocks */

  assert(ms);                   /* check the function argument */
  while (ms->blocks) {          /* while there is another block */
    block = ms->blocks;         /* note the memory block and */
    ms->blocks = block->succ;   /* remove it from the block list */
    free(block);                /* delete the memory block */
  }
  free(ms);                     /* delete the base structure */
}  /* ms_delete() */

/*--------------------------------------------------------------------*/

void* ms_alloc (MEMSYS *ms)
{                               /* --- allocate an object */
  int     i;                    /* loop variable */
  void    **obj, **tmp;         /* allocated object, buffer */
  MSBLOCK *block;               /* new block */

  assert(ms);                   /* check the function argument */
  obj = ms->free;               /* get the head of the free list */
  if (!obj) {                   /* if there is no free node, */
    block = (MSBLOCK*)malloc(ms->blksz);
    if (!block) return NULL;    /* allocate a new memory block */
    block->succ = ms->blocks;   /* and add it at the head */
    ms->blocks  = block;        /* of the block list */
    ms->free    = obj = (void*)(block +1);
    for (i = ms->cnt; --i > 0; ) {
      tmp = obj; *tmp = obj += ms->size; }
    *obj = NULL;                /* traverse the object vector */
    obj  = ms->free;            /* and link the objects together, */
  }                             /* then get the next free object */
  ms->used++;                   /* count the allocated object */
  ms->free = *obj;              /* remove object from the free list */
  return (void*)obj;            /* and return the retrieved object */
}  /* ms_alloc() */

/*--------------------------------------------------------------------*/

void ms_free (MEMSYS *ms, void *obj)
{                              /* --- deallocate an f.p. tree node */
  assert(ms && obj);           /* check the function arguments */
  *(void**)obj = ms->free;     /* insert the freed object */
  ms->free     = obj;          /* at the head of the free list */
  ms->used--;                  /* count the deallocated object */
}  /* ms_free() */
