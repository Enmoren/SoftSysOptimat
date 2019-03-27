/* Prints a matrix (not used)
    Copied from How to Optimize Gemm Tutorial
      created by Prof. Robert van de Geijn and Jianyu Huang
*/

#include <stdio.h>

#define A( i, j ) a[ (j)*lda + (i) ]

void print_matrix( int m, int n, double *a, int lda )
{
  int i, j;

  for ( j=0; j<n; j++ ){
    for ( i=0; i<m; i++ )
      printf("%le ", A( i,j ) );
    printf("\n");
  }
  printf("\n");
}
