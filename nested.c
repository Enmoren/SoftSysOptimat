/* performs matrix multiplication using a nested for loop */
#include <stdio.h>
#include <stdlib.h>
#include "generate_matrix.h"

int* matmul(int *res, int size, int* random_matrix1, int* random_matrix2){
  res = (int *)malloc(size * size * sizeof(int));
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      *(res + i*size + j) = 0;
      for (int k=0; k<size; k++){
        *(res + i*size + j) += (*(random_matrix1 + i*size + k)) * (*(random_matrix2 + k*size + j));
      }
    }
  }
  return res;
}

int main(int argc, char* argv[]){
  if (argc != 2) {
    fprintf(stderr, "You need to input size arugment!");
    return 1;
  }
  int size = atoi(argv[1]);
  int *random_matrix1 = generate_matrix(size);
  int *random_matrix2 = generate_matrix(size);
  int *res;
  res = matmul(res, size, random_matrix1, random_matrix2);

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
