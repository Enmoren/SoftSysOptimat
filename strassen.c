#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generate_matrix.h"

//TODO: free allocated matrices, consider int*** of int** (list of pointers to matrices)
        //Added calls to free, but gives error of multiple attempts to free matrices
//TODO: structs?
//TODO: wrong answer in base case
//TODO: Free allocated matrice
//TODO:
static int gMatrixSize = 800;

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

int** createArray(int matrixSize)
{
    // int* values = calloc(matrixSize*matrixSize, sizeof(int));
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


// int *generateArrayMatrix (int matrixSize){
//   // int *matrix = (int*)malloc(matrixSize * matrixSize * sizeof(int));
//   int *matrix = (int *)malloc(matrixSize * matrixSize * sizeof(int));
//   srand((unsigned)time(NULL));
//   for(int i = 0; i<matrixSize; i++){
//       for(int j = 0; j<matrixSize; j++){
//           matrix[i][j] = rand() % 100;
//       }
//   }
//   return matrix;
// }

/*create matrix of size (n[start:end] x n[start:end]) by splitting mat
//Called by recursive_strassen()
// mat = matrix to split
// startrow, endrow = begin and end bounds of mat to be copied into res
// startcol, endcol = begin and end bounds of mat to be copied into res
// res = n/2 x n/2 matrix with appropriate vals copied in. Caller frees
*/
int** subdivide(int** mat, int startrow, int endrow, int startcol, int endcol){
  int dimrow =  endrow - startrow;
  int dimcol = endcol - startcol;
  int** res = malloc(sizeof(int) * dimrow);

  for (int i=startrow; i<endrow; ++i){
      res[i-startrow] = malloc(dimcol*sizeof(int*));
      for (int j=startcol; j<endcol; ++j){
        int resi = i - startrow;
        int resj = j - startcol;
        res[resi][resj] = mat[i][j];
      }
  }
  return res;
}

//Base case for calculating Strassen algorithm
//Caller frees
int** base(int** a ,int** b){
  int dim = 2;
  int m1, m2, m3, m4 , m5, m6, m7;
  //create result matrix
  int** res = malloc(dim * sizeof(int));
  for (int i=0; i<dim; i++){
    res[i] = malloc(dim * sizeof(int));
  }

  m1= (a[0][0] + a[1][1]) * (b[0][0] + b[1][1]);
  m2= (a[1][0] + a[1][1]) * b[0][0];
  m3= a[0][0] * (b[0][1] - b[1][1]);
  m4= a[1][1] * (b[1][0] - b[0][0]);
  m5= (a[0][0] + a[0][1]) * b[1][1];
  m6= (a[1][0] - a[0][0]) * (b[0][0]+b[0][1]);
  m7= (a[0][1] - a[1][1]) * (b[1][0]+b[1][1]);

  res[0][0] = m1 + m4 - m5 + m7;
  res[0][1] = m3 + m5;
  res[1][0] = m2 + m4;
  res[1][1] = m1 - m2 + m3 + m6;

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
  if (matrixSize == 2){
    res = base(a, b);
    int** mats[] = {a, b};
    // freeMatrix(mats);
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
    // int** mats[] = {m11, m12, m21, m22};
    // freeMatrix(mats);
  }
  return res;
}

int main(int argc, char* argv[]){
  srand(time(NULL)); //randomize seed <-- should only be called once
  int** b = createArray(gMatrixSize);

  // int m1, m2, m3, m4 , m5, m6, m7;
  // int c[2][2];
  int** a = createArray(gMatrixSize);
  // printf("%")
  // printf("0x%" PRIXPTR "\n", (uintptr_t)a);
  // printf("0x%" PRIXPTR "\n", (uintptr_t)b);

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

  // m1= (a[0][0] + a[1][1]) * (b[0][0] + b[1][1]);
  // m2= (a[1][0] + a[1][1]) * b[0][0];
  // m3= a[0][0] * (b[0][1] - b[1][1]);
  // m4= a[1][1] * (b[1][0] - b[0][0]);
  // m5= (a[0][0] + a[0][1]) * b[1][1];
  // m6= (a[1][0] - a[0][0]) * (b[0][0]+b[0][1]);
  // m7= (a[0][1] - a[1][1]) * (b[1][0]+b[1][1]);
  //
  // c[0][0] = m1 + m4- m5 + m7;
  // c[0][1] = m3 + m5;
  // c[1][0] = m2 + m4;
  // c[1][1] = m1 - m2 + m3 + m6;
  // int** res = base(a, b);
  int** res1 = recursive_strassen(a, b, 4);

   printf("\nAfter multiplication using Strassen's algorithm \n");
   for(int i = 0; i < gMatrixSize ; i++){
      for(int j = 0;j < gMatrixSize; j++){
           printf("%d\t", res1[i][j]);
         }
       printf("\n");
   }

   return 0;
}
