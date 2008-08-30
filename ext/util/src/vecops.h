/*----------------------------------------------------------------------
  File    : vecops.h
  Contents: some special vector operations
  Author  : Christian Borgelt
  History : 1996.09.16 file created
            1999.02.04 long int changed to int
            2001.06.03 function v_shuffle added
            2002.01.02 functions v_intsort, v_fltsort, v_dblsort added
            2002.03.03 functions v_reverse, v_intrev etc. added
            2003.08.21 function v_heapsort added
            2007.01.16 shuffle functions for basic data types added
----------------------------------------------------------------------*/
#ifndef __VECOPS__
#define __VECOPS__

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef int VCMPFN (const void *p1, const void *p2, void *data);

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern void v_sort     (void *vec, int n, VCMPFN cmpfn, void *data);
extern void v_heapsort (void *vec, int n, VCMPFN cmpfn, void *data);
extern void v_move     (void *vec, int off, int n, int pos, int esz);
extern void v_shuffle  (void *vec, int n, double randfn (void));
extern void v_reverse  (void *vec, int n);

extern void v_intsort  (int    *vec, int n);
extern void v_intrev   (int    *vec, int n);
extern void v_intshfl  (int    *vec, int n, double randfn (void));

extern void v_fltsort  (float  *vec, int n);
extern void v_fltrev   (float  *vec, int n);
extern void v_fltshfl  (float  *vec, int n, double randfn (void));

extern void v_dblsort  (double *vec, int n);
extern void v_dblrev   (double *vec, int n);
extern void v_dblshfl  (double *vec, int n, double randfn (void));

#endif
