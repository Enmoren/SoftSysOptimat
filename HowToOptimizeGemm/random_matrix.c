/* Generates a random matrix (similar to generate_matrix.c)
    Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
    Adapted from How to Optimize Gemm Tutorial
      created by Prof. Robert van de Geijn and Jianyu Huang
*/

#include <stdlib.h>

#define A( i,j ) a[ (j)*lda + (i) ]

void random_matrix( int m, int n, double *a, int lda )
{
  double drand48();
  int i,j;

  for ( j=0; j<n; j++ )
    for ( i=0; i<m; i++ )
      A( i,j ) = 2.0 * drand48( ) - 1.0;
}
