/* ========================================================================= */
/* === AMD mexFunction ===================================================== */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */
/* AMD Version 1.1 (Jan. 21, 2004), Copyright (c) 2004 by Timothy A. Davis,  */
/* Patrick R. Amestoy, and Iain S. Duff.  See ../README for License.         */
/* email: davis@cise.ufl.edu    CISE Department, Univ. of Florida.           */
/* web: http://www.cise.ufl.edu/research/sparse/amd                          */
/* ------------------------------------------------------------------------- */


/*
 * Usage:
 *  p = amd (A)
 *  p = amd (A, Control)
 *  [p, Info] = amd (A)
 *  [p, Info] = amd (A, Control)
 *  Control = amd ;     % return the default Control settings for AMD
 *  amd ;           % print the default Control settings for AMD
 *
 * Given a square matrix A, compute a permutation P suitable for a Cholesky
 * factorization of the matrix B (P,P), where B = spones (A) + spones (A').
 * The method used is the approximate minimum degree ordering method.  See
 * amd.m and amd.h for more information.
 */

#include "amd.h"
#include "mex.h"
#include "matrix.h"

void mexFunction
(
    int nlhs,
    mxArray *plhs[],
    int nrhs,
    const mxArray *prhs[]
)
{
    /* int i, m, n, *Ap, *Ai, *P, nc, result, spumoni, full ; */
    int i, nc, spumoni, full;
    size_t  m, n, *Ap, *Ai, *P, result;
    double  *Pout, *InfoOut, Control [AMD_CONTROL], Info [AMD_INFO], *ControlIn;
    mxArray *A, *string, *parameter;

    /* --------------------------------------------------------------------- */
    /* get control parameters */
    /* --------------------------------------------------------------------- */

    spumoni = 0;
    if ( nrhs == 0 )
    {
        /* get the default control parameters, and return */
        plhs [0] = mxCreateDoubleMatrix( AMD_CONTROL, 1, mxREAL );
        amd_defaults( mxGetPr( plhs [0] ) );
        if ( nlhs == 0 )
        {
            amd_control( mxGetPr( plhs [0] ) );
        }
        return;
    }

    amd_defaults( Control );
    if ( nrhs > 1 )
    {
        ControlIn   = mxGetPr( prhs [1] );
        nc      = mxGetM( prhs [1] ) * mxGetN( prhs [1] );
        Control [AMD_DENSE]
            = (nc > 0) ? ControlIn [AMD_DENSE] : AMD_DEFAULT_DENSE;
        Control [AMD_AGGRESSIVE]
            = (nc > 1) ? ControlIn [AMD_AGGRESSIVE] : AMD_DEFAULT_AGGRESSIVE;
        spumoni = (nc > 2) ? (ControlIn [2] != 0) : 0;
    }

    if ( spumoni > 0 )
    {
        amd_control( Control );
    }

    /* --------------------------------------------------------------------- */
    /* get inputs */
    /* --------------------------------------------------------------------- */

    if ( nlhs > 2 || nrhs > 2 )
    {
        mexErrMsgTxt( "Usage: p = amd (A)\nor [p, Info] = amd (A, Control)" );
    }

    A   = (mxArray *) prhs [0];
    n   = mxGetN( A );
    m   = mxGetM( A );
    if ( spumoni > 0 )
    {
        mexPrintf( "    input matrix A is %d-by-%d\n", m, n );
    }
    if ( mxGetNumberOfDimensions( A ) != 2 )
    {
        mexErrMsgTxt( "amd: A must be 2-dimensional" );
    }
    if ( m != n )
    {
        mexErrMsgTxt( "amd: A must be square" );
    }

    /* --------------------------------------------------------------------- */
    /* allocate workspace for output permutation */
    /* --------------------------------------------------------------------- */

    P = mxMalloc( (n + 1) * sizeof(size_t) );

    /* --------------------------------------------------------------------- */
    /* if A is full, convert to a sparse matrix */
    /* --------------------------------------------------------------------- */

    full = !mxIsSparse( A );
    if ( full )
    {
        if ( spumoni > 0 )
        {
            mexPrintf(
                "    input matrix A is full (sparse copy of A will be created)\n" );
        }
        mexCallMATLAB( 1, &A, 1, (mxArray * *) prhs, "sparse" );
    }
    Ap  = mxGetJc( A );
    Ai  = mxGetIr( A );

    // [debug] 查错：Ai 有问题
    /* printf("size of mxGetJc (A) is %d,size of int is%d,size of unsigned long is%d\n", sizeof(mxGetJc (A)),sizeof(int),sizeof(unsigned long)); */
    // printf( "Ai=" );
    // for ( i = 0; i < Ap[n]; i++ )
    // {
    //     printf( "%lu,", Ai[i] );
    // }
    // printf( "\nAp[n]=%d, mxGetNzmax=%d,#row=%d,#column=%d\n", Ap[n], mxGetNzmax(A), m, n);

    if ( spumoni > 0 )
    {
        mexPrintf( "    input matrix A has %d nonzero entries\n", Ap [n] );
    }

    /* --------------------------------------------------------------------- */
    /* order the matrix */
    /* --------------------------------------------------------------------- */

    result = amd_order( n, Ap, Ai, P, Control, Info );

    /* --------------------------------------------------------------------- */
    /* if A is full, free the sparse copy of A */
    /* --------------------------------------------------------------------- */

    if ( full )
    {
        mxDestroyArray( A );
    }

    /* --------------------------------------------------------------------- */
    /* print results (including return value) */
    /* --------------------------------------------------------------------- */

    if ( spumoni > 0 )
    {
        amd_info( Info );
    }

    /* --------------------------------------------------------------------- */
    /* check error conditions */
    /* --------------------------------------------------------------------- */

    if ( result == AMD_OUT_OF_MEMORY )
    {
        mexErrMsgTxt( "amd: out of memory" );
    }else if ( result == AMD_INVALID )
    {
        mexErrMsgTxt( "amd: input matrix A is corrupted" );
    }

    /* --------------------------------------------------------------------- */
    /* copy the outputs to MATLAB */
    /* --------------------------------------------------------------------- */

    /* output permutation, P */
    plhs [0]    = mxCreateDoubleMatrix( 1, n, mxREAL );
    Pout        = mxGetPr( plhs [0] );
    for ( i = 0; i < n; i++ )
    {
        Pout [i] = P [i] + 1; /* change to 1-based indexing for MATLAB */
    }
    mxFree( P );

    /* Info */
    if ( nlhs > 1 )
    {
        plhs [1]    = mxCreateDoubleMatrix( AMD_INFO, 1, mxREAL );
        InfoOut     = mxGetPr( plhs [1] );
        for ( i = 0; i < AMD_INFO; i++ )
        {
            InfoOut [i] = Info [i];
        }
    }
}