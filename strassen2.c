#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generate_matrix.h"

static int gMatrixSize = 2;

int** createArray(int matrixSize){

    int** rows = malloc(matrixSize*sizeof(int*));
    for (int i=0; i<matrixSize; ++i)
    {
        rows[i] = malloc(matrixSize*sizeof(int));
        for (int j =0; j<matrixSize; ++j)
        {
          rows[i][j] = rand() % 100;
        }
    }
    return rows;
}

void strassenMult(int **aArray, int **bArray, int ***aResultMatrix, int aSize){
  if (aSize == 2){

  }
}

int main(int argc, char* argv[]){
  int m1, m2, m3, m4 , m5, m6, m7;
  int c[2][2];
  srand(time(NULL));
  int** a = createArray(gMatrixSize);
  int** b = createArray(gMatrixSize);

  printf("Array a is at address: %p\n", *a);
  printf("Array b is at address: %p\n", *b);

  printf("\nThe first matrix is\n");
  for(int i = 0; i < 2; i++){
      printf("\n");
      for(int j = 0; j < 2; j++)
           printf("%d\t", a[i][j]);
  }

  printf("\nThe second matrix is\n");
  for(int i = 0;i < 2; i++){
      printf("\n");
      for(int j = 0;j < 2; j++)
           printf("%d\t", b[i][j]);
  }

  m1= (a[0][0] + a[1][1]) * (b[0][0] + b[1][1]);
  m2= (a[1][0] + a[1][1]) * b[0][0];
  m3= a[0][0] * (b[0][1] - b[1][1]);
  m4= a[1][1] * (b[1][0] - b[0][0]);
  m5= (a[0][0] + a[0][1]) * b[1][1];
  m6= (a[1][0] - a[0][0]) * (b[0][0]+b[0][1]);
  m7= (a[0][1] - a[1][1]) * (b[1][0]+b[1][1]);

  c[0][0] = m1 + m4- m5 + m7;
  c[0][1] = m3 + m5;
  c[1][0] = m2 + m4;
  c[1][1] = m1 - m2 + m3 + m6;

   printf("\nAfter multiplication using Strassen's algorithm \n");
   for(int i = 0; i < 2 ; i++){
      for(int j = 0;j < 2; j++){
           printf("%d\t", c[i][j]);
         }
       printf("\n");
   }
   // free(a);
   // free(b);
   // free(c);
   return 0;
}