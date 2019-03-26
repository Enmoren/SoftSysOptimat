/*Implementation of Strassen algorithm (old version, not parallelized)

gcc strassen.c -o strassen && ./strassen
Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
Copyright: MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generate_matrix.h"

static int gMatrixSize = 2;

//Frees the allocated matrices in the list
void freeMatrix(int*** m){
  int i = 0;
  while (m[i] != NULL){
    free(m[i]);
  }
}

//prints matrix
void printmat(int** m, int dim){
  printf("\n");
  for(int i = 0; i < dim ; i++){
     for(int j = 0;j < dim; j++){
          printf("%d\t", m[i][j]);
        }
      printf("\n");
  }
}

/*
  mallocs and generates 2D array containing pseudo-random integers
  returns: ptr to 2D array
  matrixSize: dimension of matrix to be generated
*/
int** createArray(int matrixSize){
    int** rows = malloc(matrixSize*sizeof(int*));
    for (int i=0; i<matrixSize; ++i)
    {
        rows[i] = malloc(matrixSize*sizeof(int*));
        for (int j =0; j<matrixSize; ++j)
        {
          rows[i][j] = rand() % 100;
        }
    }
    return rows;
}


/*create matrix of size (n[start:end] x n[start:end]) by splitting mat
//Called by recursive_strassen()
// mat = matrix to split
// startrow, endrow = begin and end bounds of mat to be copied into res
// startcol, endcol = begin and end bounds of mat to be copied into res
// res = n/2 x n/2 matrix with appropriate vals copied in.
*/
int** subdivide(int** mat, int startrow, int endrow, int startcol, int endcol){
  int dimrow =  endrow - startrow;
  int dimcol = endcol - startcol;
  int** res = malloc(sizeof(int) * dimrow);

  for (int i=startrow; i<endrow; ++i){
      res[i-startrow] = malloc(sizeof(int*) * dimcol);
      for (int j=startcol; j<endcol; ++j){
        int resi = i - startrow;
        int resj = j - startcol;
        res[resi][resj] = mat[i][j];
      }
  }
  return res;
}

//Base case for calculating Strassen algorithm
int** base(int** a ,int** b){
  int dim = 2;
  int m1, m2, m3, m4 , m5, m6, m7;

  //create result matrix
  int** res = malloc(dim * sizeof(int));
  for (int i=0; i<dim; i++){
    res[i] = malloc(dim * sizeof(int));
  }

  m1= a[0][0]*(b[0][1]-b[1][1]);
  m2= (a[0][0]+a[0][1])*b[1][1];
  m3= (a[1][0]+a[1][1])*b[0][0];
  m4= a[1][1]*(b[1][0]-b[0][0]);
  m5= (a[0][0]+a[1][1])*(b[0][0]+b[1][1]);
  m6= (a[0][1]-a[1][1])*(b[1][0]+b[1][1]);
  m7=(a[0][0]-a[1][0])*(b[0][0]+b[0][1]);

  res[0][0] = m5 + m4 - m2 + m6;
  res[0][1] = m1 + m2;
  res[1][0] = m3 + m4;
  res[1][1] = m1 + m5 - m3 - m7;

  return res;
}

//Take the 4 submatrices (size n/2 x n/2) and pieces them into a newly allocated
// n x n matrix
int** merge(int** m11, int** m12, int** m21, int** m22, int matrixSize){
  int** res = malloc(sizeof(int)*matrixSize);
  int half = matrixSize/2;
  for (int i=0; i<matrixSize; i++){
    res[i] = malloc(sizeof(int)*matrixSize);
    for (int j=0; j<matrixSize; j++){
      if (i < half && j < half){  //m11 vals
        res[i][j] = m11[i][j];
      }
      else if (i < half && j >= half){  //m12 vals
        res[i][j] = m12[i][j-half];
      }
      else if (i >= half && j < half){  //m21 vals
        res[i][j] = m21[i-half][j];
      }
      else if (i >= half && j>= half){  //m22 vals
        res[i][j] = m22[i-half][j-half];
      }
    }
  }
  return res;
}

//psuedo Mergesort implementation of Strassen algorithm
int** recursive_strassen(int** a, int** b, int matrixSize){
  int ** res;
  if (matrixSize <= 2){
    res = base(a, b);
    int** mats[] = {a, b};
    return res;

  } else {  //split up matrix
    int half = matrixSize/2;
    int** m11 = recursive_strassen(subdivide(a, 0, half, 0, half),
                                  subdivide(b, 0, half, 0, half),
                                  half);
    printf("\nAfter subdivision algorithm, m11 \n");
    printmat(m11, half);

    int** m12 = recursive_strassen(subdivide(a, 0, half, half, matrixSize),
                                  subdivide(b, 0, half, half, matrixSize),
                                  half);
    printf("\nAfter subdivision algorithm, m12 \n");
    printmat(m12, half);

    int** m21 = recursive_strassen(subdivide(a, half, matrixSize, 0, half),
                                  subdivide(b, half, matrixSize, 0, half),
                                  half);
    printf("\nAfter subdivision algorithm, m21 \n");
    printmat(m21, half);

    int** m22 = recursive_strassen(subdivide(a, half, matrixSize, half, matrixSize),
                                  subdivide(b, half, matrixSize, half, matrixSize),
                                  half);
    printf("\nAfter subdivision algorithm, m22 \n");
    printmat(m22, half);

    //reassemble matrix
    res = merge(m11, m12, m21, m22, matrixSize);
  }
  return res;
}

int main(int argc, char* argv[]){
  srand(time(NULL)); //randomize seed <-- should only be called once
  int** a = createArray(gMatrixSize);
  int** b = createArray(gMatrixSize);

  printf("Array a is at address: %p\n", *a);
  printf("Array b is at address: %p\n", *b);

  printf("\nThe first matrix is\n");
  for(int i = 0; i < gMatrixSize; i++){
      printf("\n");
      for(int j = 0; j < gMatrixSize; j++)
           printf("%d\t", a[i][j]);
  }

  printf("\nThe second matrix is\n");
  for(int i = 0;i < gMatrixSize; i++){
      printf("\n");
      for(int j = 0;j < gMatrixSize; j++)
           printf("%d\t", b[i][j]);
  }

  int** res1 = recursive_strassen(a, b, gMatrixSize);

   printf("\nAfter multiplication using Strassen's algorithm \n");
   for(int i = 0; i < gMatrixSize ; i++){
      for(int j = 0;j < gMatrixSize; j++){
           printf("%d\t", res1[i][j]);
         }
       printf("\n");
   }

   return 0;
}
