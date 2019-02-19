/* performs matrix multiplication using a nested for loop */
#include <stdio.h>
#include <stdlib.h>

#define N  2

void matmul(double A[][N], double B[][N], double res[][N]){
  for (int i=0; i<N; i++){
    for (int j=0; j<N; j++){
      res[i][j] = 0;
      for (int k=0; k<N; k++){
        res[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

int main(){
  double A[N][N] = {{1.0, 2.0},{3.0, 4.0}};
  double B[N][N] = {{2.0, 5.0},{6.0, 7.0}};
  double res[N][N];
  matmul(A, B, res);
  for (int i=0; i<N; i++){
    for (int j=0; j<N; j++){
      printf("%f ", res[i][j]);
    }
    printf("\n");
  }
  return 0;
}
