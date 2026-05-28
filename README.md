SORT(3)                    Library Functions Manual                    SORT(3)

NAME
       sort,  sort_set_method,  sort_set_direction  -  generic  in-place array
       sorting library

SYNOPSIS
       #include sort.h

       int sort(void *array, int size, int elemsize,
                int (*comp)(void *a1, void *a2));

       int sort_set_method(sort_method_t method);

       int sort_set_direction(sort_direction_t direction);

       Compile and link with:

              gcc -o program program.c sort.c

DESCRIPTION
       sort() sorts the array pointed to by array in place using the algorithm
       and direction previously selected by sort_set_method() and sort_set_di-
       rection().

       The array contains size elements, each of elemsize bytes.  The  library
       uses  memcpy(3)  internally so it works correctly for any element type,
       including structs.

       The user-supplied function comp is called to compare two  elements  and
       must return:

              -1     if the element at a1 is less than the element at a2

              0      if they are equal

              1      if the element at a1 is greater than the element at a2

       sort_set_method() selects the sorting algorithm.  The setting is stored
       in  a  static variable inside sort.c, invisible to the caller.  It per-
       sists until the next call to sort_set_method().

       sort_set_direction() selects the sort order.  When SORT_DESC is active,
       the library automatically inverts the result of comp before every  com-
       parison,  so  all  algorithms  produce  descending  output  without any
       changes to their code.

ALGORITHMS
       BUBBLE_SORT
              Simple exchange sort. Repeatedly walks the array and swaps adja-
              cent elements that are out of order.  Time complexity O(n^2) av-
              erage and worst case.

       INSERTION_SORT
              Builds the sorted sequence one element at a  time  by  inserting
              each  new  element  into  the correct position.  Time complexity
              O(n^2). Stable.

       SELECTION_SORT
              On each pass finds the minimum element in the unsorted tail  and
              moves it to the front.  Time complexity O(n^2). Not stable.

       COMB_SORT
              Improvement  over  bubble sort that uses a shrinking gap (factor
              1.3) to eliminate out-of-order elements  far  apart  ("turtles")
              early.  Time complexity O(n^2) worst, O(n log n) average.

       SHELL_SORT
              Generalisation  of insertion sort. Sorts elements separated by a
              gap that halves on each pass (n/2, n/4, ..., 1).  Time  complex-
              ity O(n^(3/2)) with this gap sequence.

       QUICK_SORT
              Divide-and-conquer  algorithm.  Partitions  the array around the
              last element as pivot and recurses on both  halves.   Time  com-
              plexity O(n log n) average, O(n^2) worst case.  Not stable.

       MERGE_SORT
              Recursively  divides  the  array  in half, sorts each half, then
              merges the two sorted  halves.   Allocates  O(n)  extra  memory.
              Time complexity O(n log n). Stable.

       HEAP_SORT
              Builds  a  max-heap from the array, then repeatedly extracts the
              maximum element.  Time complexity O(n log n). In-place. Not sta-
              ble.

RETURN VALUE
       All three functions return SORT_OK (0) on success and a non-zero  error
       code on failure.

ERROR CODES
       SORT_OK        (0)
              Success.

       SORT_ERR_NULL  (1)
              array or comp is NULL.

       SORT_ERR_SIZE  (2)
              size or elemsize is <= 0.

       SORT_ERR_METHOD (3)
              Unknown sort_method_t value passed to sort_set_method().

       SORT_ERR_DIRECTION (4)
              Unknown sort_direction_t value passed to sort_set_direction().

       SORT_ERR_ALLOC (5)
              Internal malloc(3) failed.

TYPES AND CONSTANTS
   sort_method_t
       typedef enum {
           BUBBLE_SORT    = 1,
           INSERTION_SORT = 2,
           SELECTION_SORT = 3,
           COMB_SORT      = 4,
           SHELL_SORT     = 5,
           QUICK_SORT     = 6,
           MERGE_SORT     = 7,
           HEAP_SORT      = 8
       } sort_method_t;

   sort_direction_t
       typedef enum {
           SORT_ASC  = 1,
           SORT_DESC = 2
       } sort_direction_t;

       The  default  method  is  BUBBLE_SORT  and  the  default  direction  is
       SORT_ASC.

EXAMPLES
   Sorting an array of int
       #include <stdio.h>
       #include "sort.h"

       int comp_int(void *a, void *b)
       {
           int n1 = *((int *)a);
           int n2 = *((int *)b);
           if (n1 < n2) return -1;
           if (n1 > n2) return  1;
           return 0;
       }

       int main(void)
       {
           int arr[] = {5, 3, 1, 4, 2};
           int n = 5;

           sort_set_method(QUICK_SORT);
           sort_set_direction(SORT_ASC);

           if (sort(arr, n, sizeof(int), comp_int) != SORT_OK) {
               return 1;
           }
           /* arr is now: 1 2 3 4 5 */
           return 0;
       }

   Sorting an array of structs by a double field
       #include "sort.h"

       typedef struct {
           int    id;
           double value;
       } item_t;

       int comp_item(void *a, void *b)
       {
           item_t *s1 = (item_t *)a;
           item_t *s2 = (item_t *)b;
           if (s1->value < s2->value) return -1;
           if (s1->value > s2->value) return  1;
           return 0;
       }

       int main(void)
       {
           item_t items[] = {{3, 3.0}, {1, 1.0}, {2, 2.0}};

           sort_set_method(MERGE_SORT);
           sort_set_direction(SORT_DESC);
           sort(items, 3, sizeof(item_t), comp_item);
           /* result: {3,3.0}, {2,2.0}, {1,1.0} */
           return 0;
       }

   Passing a comparator to a helper function
       /* Returns 1 if *a > *b, 2 otherwise. */
       int max_elem(void *a, void *b, int (*compare)(void *, void *))
       {
           if (compare(a, b) == 1) return 1;
           return 2;
       }

NOTES
       +o The current method and direction are stored in static  variables  in-
         side  sort.c.  They are invisible outside that translation unit.  The
         only way to change them is through sort_set_method() and sort_set_di-
         rection(), which validate the input before storing it.  Using an  in-
         valid  enum value causes these functions to return an error and leave
         the current setting unchanged.

       +o Pointer arithmetic on void * is undefined in standard C.  The library
         always casts to char * (one byte per unit) before  computing  element
         offsets.

       +o Element swaps use a 256-byte stack buffer for small elements to avoid
         a  malloc(3) call on every swap.  A heap allocation is used only when
         elemsize exceeds 256 bytes.

       +o The   library   is   not   thread-safe.    If   two   threads    call
         sort_set_method() or sort() concurrently, the behaviour is undefined.

SEE ALSO
       qsort(3), memcpy(3), malloc(3)

AUTHORS
       Written as a coursework assignment.

libsort 1.0                       2026-05-02                           SORT(3)
