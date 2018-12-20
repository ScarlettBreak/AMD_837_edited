/* ========================================================================= */
/* === AMD_valid =========================================================== */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/* AMD Version 1.1 (Jan. 21, 2004), Copyright (c) 2004 by Timothy A. Davis,  */
/* Patrick R. Amestoy, and Iain S. Duff.  See ../README for License.         */
/* email: davis@cise.ufl.edu    CISE Department, Univ. of Florida.           */
/* web: http://www.cise.ufl.edu/research/sparse/amd                          */
/* ------------------------------------------------------------------------- */

/* Check if a column-form matrix is valid or not.  The matrix A is
 * n_row-by-n_col.  The row indices of entries in column j are in
 * Ai [Ap [j] ... Ap [j+1]-1].  Required conditions are:
 *
 *	n_row >= 0
 *	n_col >= 0
 *	nz = Ap [n_col] >= 0	    number of entries in the matrix
 *	Ap [0] == 0
 *	Ap [j] <= Ap [j+1] for all j in the range 0 to n_col.
 *	row indices in Ai [Ap [j] ... Ap [j+1]-1] must be sorted in ascending
 *	    order, must be in the range 0 to n_row-1, and no duplicate entries
 *	    can exist.
 *
 * Not user-callable.
 */

#include "amd_internal.h"

GLOBAL Int AMD_valid
(
    /* inputs, not modified on output: */
    Int n_row,		/* A is n_row-by-n_col */
	Int n_col,
    const Int Ap [ ],	/* column pointers of A, of size n_col+1 */
    const Int Ai [ ]	/* row indices of A, of size nz = Ap [n_col] */
	)
{
	Int nz, j, p1, p2, ilast, i, p ;

    /* [debug] make sure(1,2): n_row >= 0,n_col >= 0  */
	if (n_row < 0 || n_col < 0)
	{
		AMD_DEBUG0 (("n must be >= 0: "ID" "ID"\n", n_row, n_col)) ;
		printf("n_row-by-n_col condition(1,2) not satisfied\n");
		return (FALSE) ;
	}

    /* [debug] make sure(3,4): nz = Ap [n_col] >= 0  */
	nz = Ap [n_col] ;
	if (Ap [0] != 0 || nz < 0)
	{
	/* column pointers must start at Ap [0] = 0, and Ap [n] must be >= 0 */
		AMD_DEBUG0 (("column 0 pointer bad or nz < 0\n")) ;
		printf("n_row-by-n_col condition(3,4) not satisfied\n");
		return (FALSE) ;
	}

	for (j = 0 ; j < n_col ; j++)
	{
		p1 = Ap [j] ;
		p2 = Ap [j+1] ;
		AMD_DEBUG2 (("\nColumn: "ID" p1: "ID" p2: "ID"\n", j, p1, p2)) ;

		/* [debug] make sure(5): Ap [j] <= Ap [j+1] for all j in the range 0 to n_col. */
		if (p1 > p2)
		{
	    /* column pointers must be ascending */
			AMD_DEBUG0 (("column "ID" pointer bad\n", j)) ;
			printf("n_row-by-n_col condition(5) not satisfied\n");
			return (FALSE) ;
		}

		//[debug] 第一个元素开始，第一个元素肯定没问题
		// ilast = EMPTY ;
		ilast = Ai[p1];
		printf("\n column:%d\n", j);//当前是第j列
		printf("row:%d,", Ai[p1]);//当前的行数

		// [debug]
		for (p = p1+1 ; p < p2 ; p++) // 从第2个元素开始
		// for (p = p1 ; p < p2 ; p++)
		{
			i = Ai [p] ;
			AMD_DEBUG3 (("row: "ID"\n", i)) ;

	    /* [debug] make sure(6): 
            * row indices in Ai [Ap [j] ... Ap [j+1]-1] must be sorted in ascending
            * order, must be in the range 0 to n_row-1, and no duplicate entries
            * can exist.
        */
			// [begin: debug]
			// printf("ilast=%d,i=%d,i<=ilast=%d\n", ilast, i, i<=ilast);
			printf("%d,", i);
			if (i <= ilast || i >= n_row)
			{
				/* row index out of range, or unsorted */
				AMD_DEBUG0 (("index out of range, col "ID" row "ID"\n", j, i));
				printf("n_row-by-n_col condition(6) not satisfied\n");
				return (FALSE) ;
			}
			// if (i >= n_row)
			// {
			// 	/* row index out of range */
			// 	printf("row index out of range\n");
			// 	return (FALSE) ;
			// }
			// if (i <= ilast)
			// {
			// 	/* row index unsorted */
			// 	printf("row index unsorted\n");
			// 	return (FALSE) ;
			// }
			// [end: debug]
			ilast = i ;
		}
	}
	return (TRUE) ;
}
