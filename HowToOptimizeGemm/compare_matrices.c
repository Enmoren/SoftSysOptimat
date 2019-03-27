/* Compare two matrices and return the maximum difference bewteen entries
    Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
    Adapted from How to Optimize Gemm Tutorial
      created by Prof. Robert van de Geijn and Jianyu Huang
*/

#define A( i, j ) a[ (j)*lda + (i) ]
#define B( i, j ) b[ (j)*ldb + (i) ]
#define abs( x ) ( (x) < 0.0 ? -(x) : (x) )

double compare_matrices( int m, int n, double *a, int lda, double *b, int ldb )
{
  int i, j;
  double max_diff = 0.0, diff;

  for ( j=0; j<n; j++ )
    for ( i=0; i<m; i++ ){
      diff = abs( A( i,j ) - B( i,j ) );
      max_diff = ( diff > max_diff ? diff : max_diff );
    }

  return max_diff;
}
