/* Copy matrix A into matrix B
    Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
    Adapted from How to Optimize Gemm Tutorial
      created by Prof. Robert van de Geijn and Jianyu Huang
    Copyright: MIT License
*/

#define A( i, j ) a[ (j)*lda + (i) ]
#define B( i, j ) b[ (j)*ldb + (i) ]

void copy_matrix( int m, int n, double *a, int lda, double *b, int ldb )
{
  int i, j;

  for ( j=0; j<n; j++ )
    for ( i=0; i<m; i++ )
      B( i,j ) = A( i,j );
}
