/* Parallelized implementation of Strassen Matrix Multiplication Algorithm

gcc strassen-thread.c -lpthread -o threaded && ./threaded
Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
Copyright: MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "generate_matrix.h"

//TODO: wrong answer in base case
//TODO: Test cases? and implement command line args to determine gMatrixSize?

static int gMatrixSize = 8;

typedef struct {
  int** a;
  int** b;
  int size;
} strass_arg;

//Frees the allocated matrices in the list
void freeStruct(strass_arg* m){
  free(m->a);
  free(m->b);
  free(m);
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

//Utility function, prints contents of strass_arg struct
void printargs(strass_arg* args){
  printf("\nargs->size = %d\n", args->size);
  printf("address of struct %p\n", args);
  printf("args->a: \n");
  printmat(args->a, args->size);
  printf("args->b: \n");
  printmat(args->b, args->size);
}

int** createArray(int matrixSize)
{
    // int* values = calloc(matrixSize*matrixSize, sizeof(int));
    int** rows = malloc(matrixSize * sizeof(int*));
    for (int i=0; i<matrixSize; ++i)
    {
        rows[i] = malloc(matrixSize * sizeof(int*));
        for (int j =0; j<matrixSize; ++j)
        {
          rows[i][j] = rand() % 100;
        }
    }
    return rows;
}

//Base case for calculating Strassen algorithm
//Caller frees
int** base(int** a ,int** b){
  int dim = 2;
  int m1, m2, m3, m4 , m5, m6, m7;
  //create result matrix
  int** res = malloc(dim * sizeof(int*));
  for (int i=0; i<dim; i++){
    res[i] = malloc(dim * sizeof(int*));
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
  int** res = malloc(sizeof(int*) * matrixSize);
  int half = matrixSize/2;
  for (int i=0; i<matrixSize; i++){
    res[i] = malloc(sizeof(int*) * matrixSize);
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

/*create matrix of size (n[start:end] x n[start:end]) by splitting mat
//Called by parallel_strassen()
// mat = matrix to split
// startrow, endrow = begin and end bounds of mat to be copied into res
// startcol, endcol = begin and end bounds of mat to be copied into res
// res = n/2 x n/2 matrix with appropriate vals copied in. Caller frees
*/
int** subdivide(int** mat, int startrow, int startcol, int size){
  int** res = malloc(sizeof(int*) * size);

  for (int i=0; i<size; i++){
      res[i] = malloc(size * sizeof(int*));
      for (int j=0; j<size; j++){
        int mati = i + startrow;
        int matj = j + startcol;
        res[i][j] = mat[mati][matj];
      }
  }
  return res;
}

strass_arg* make_struct(int** a, int** b, int half, int quadrant){
  strass_arg* args = malloc(sizeof(strass_arg));
  args->size = half;
  switch (quadrant){
    case 0:
      args->a = subdivide(a, 0, 0, half);
      args->b = subdivide(b, 0, 0, half);
      break;
    case 1:
      args->a = subdivide(a, 0, half, half);
      args->b = subdivide(b, 0, half, half);
      break;
    case 2:
      args->a = subdivide(a, half, 0, half);
      args->b = subdivide(b, half, 0, half);
      break;
    case 3:
      args->a = subdivide(a, half, half, half);
      args->b = subdivide(b, half, half, half);
      break;
    default:
      fprintf(stderr, "Invalid case\n");
      break;
  }
  // printargs(args);
  return args;
}
//psuedo Mergesort implementation of Strassen algorithm
void* parallel_strassen(void* ptr_array){
  strass_arg *args = ptr_array;
  int** a = args->a;
  int** b = args->b;
  int matrixSize = args->size;
  int** res;

  if (matrixSize == 2){
    res = base(a, b);
    freeStruct(ptr_array);

  } else {  //split up matrix into quadrants
    int half = matrixSize/2;
    strass_arg* args_in[4];
    pthread_t threads[4];

    //Initialize threads
    for (int n=0; n<4; n++){
      args_in[n] = make_struct(a, b, half, n);

      if (pthread_create(&threads[n], NULL, parallel_strassen, args_in[n]) == -1){
        fprintf(stderr, "Can't create thread t[%d]\n", n);
      }
    }

    //Wait for threads to complete
    void* res_array[4];
    for (int i=0; i<4; i++){
      if (pthread_join(threads[i], &res_array[i]) == -1){
        fprintf(stderr, "Can't join thread t[%d]\n", i);
      }
    }

    //reassemble matrix
    res = merge((int**)res_array[0], (int**)res_array[1], (int**)res_array[2], (int**)res_array[3], matrixSize);
  }
  return (void*)res;
}

int main(int argc, char* argv[]){
  srand(time(NULL)); //randomize seed <-- should only be called once

  //initialize original struct
  strass_arg in;
  in.b = createArray(gMatrixSize);
  in.a = createArray(gMatrixSize);
  in.size = gMatrixSize;

  printf("Array a is at address: %p\n", &in.a);
  printf("Array b is at address: %p\n", &in.b);
  printargs(&in); //print values of initial struct

  //Recursive, parallelized implementation of Strassen matmul
  int** res1 = (int**)parallel_strassen((void*)&in);

   printf("\nAfter multiplication using Strassen's algorithm \n");
   printmat(res1, gMatrixSize);

   return 0;
}
