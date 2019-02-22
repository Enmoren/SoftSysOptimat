/* performs matrix multiplication using a nested for loop */
#include <stdio.h>
#include <stdlib.h>
#include "generate_matrix.h"

void matmul(int *res, int size){
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      *(res + i*size + j) = 0;
      for (int k=0; k<size; k++){
        *(res + i*size + j) += (*(random_matrix1 + i*size + k)) * (*(random_matrix2 + k*size + j));
      }
    }
  }
}

int main(int argc, char* argv[]){
  if (argc != 2) {
    fprintf(stderr, "You need to input size arugment!");
    return 1;
  }
  int size = atoi(argv[1]);
  generate_matrix(size);
  int *res = (int *)malloc(size * size * sizeof(int));
  fprintf(stdout, "Matrix: \n");
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      fprintf(stdout, "%i ", *(random_matrix1 + i*size + j));
    }
    fprintf(stdout, "\n");
  }

  fprintf(stdout, "Matrix: \n");
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      fprintf(stdout, "%i ", *(random_matrix2 + i*size + j));
    }
    fprintf(stdout, "\n");
  }

  // double A[N][N] = {{1.0, 2.0},{3.0, 4.0}};
  // double B[N][N] = {{2.0, 5.0},{6.0, 7.0}};
  //double res[size][size];
  matmul(res, size);
  // Print matrix to Standard Output
  fprintf(stdout, "Matrix: \n");
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      fprintf(stdout, "%i ", *(res + i*size + j));
    }
    fprintf(stdout, "\n");
  }
  return 0;
}
