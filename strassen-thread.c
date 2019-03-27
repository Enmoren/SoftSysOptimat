/* Parallelized implementation of Strassen Matrix Multiplication Algorithm

gcc strassen-thread.c -lpthread -o threaded && ./threaded
Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
Copyright: MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* Visual representation of Strassen algorithm for reference:
X           Y                X*Y
 +-------+   +-------+     +-------+-------+
 | A | B |   | E | F |     | AE+BG | AF+BH |
 +---+---+ * +---+---+  =  +-------+-------+
 | C | D |   | G | H |     | CE+DG | CF+DH |
 +---+---+   +---+---+     +---------------+
 Seven products:
 P1 = A(F-H)
 P2 = (A+B)H
 P3 = (C+D)E
 P4 = D(G-E)
 P5 = (A+D)(E+H)
 P6 = (B-D)(G+H)
 P7 = (A-C)(E+F)

         +-------------+-------------+
         | P5+P4-P2+P6 |    P1+P2    |
 X * Y = +-------------+-------------+
         |    P3+P4    | P1+P5-P3+P7 |
         +-------------+-------------+
*/

/*
gMatrixSize is the dimension (aka N).
NOTE: This code works _only_ on NxN matrix where N is a power of 2
*/

static int gMatrixSize = 4;

/*
  a, b = ptr to 2D array that contains matrix elements
  size = dimension of matrix
*/
typedef struct {
  int** a;
  int** b;
  int size;
} strass_arg;

/*
  Takes pointer to strass_arg struct and frees
*/
void freeStruct(strass_arg* m){
  free(m->a);
  free(m->b);
  free(m);
}

/*
  Utility function, prints the entries of a matrix
  m: ptr to 2D array containing matrix values
  dim: dimension of matrix m
*/
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
  Utility function to print contents of a strass_arg struct
  m: ptr to strass_arg struct
*/
void printargs(strass_arg* args){
  printf("\nargs->size = %d\n", args->size);
  printf("address of struct %p\n", args);
  printf("args->a: \n");
  printmat(args->a, args->size);
  printf("args->b: \n");
  printmat(args->b, args->size);
}

/*
  mallocs and generates 2D array containing pseudo-random integers
  returns: ptr to 2D array
  matrixSize: dimension of matrix to be generated
*/
int** createArray(int matrixSize){
    int** rows = malloc(matrixSize * sizeof(int*));
    for (int i=0; i<matrixSize; ++i){
        rows[i] = malloc(matrixSize * sizeof(int*));
        for (int j =0; j<matrixSize; ++j){
          rows[i][j] = rand() % 100;
        }
    }
    return rows;
}

/*
  Adds two (size x size) matrices together
  a, b: ptr to 2D array containing matrix values
  size: dimension of a, b, and res
  returns: ptr to malloc'd array containing result
*/
int** plus(int** a, int** b, int size){
  int** res = createArray(size);
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      res[i][j] = a[i][j] + b[i][j];
    }
  }
  return res;
}

/*
  Subtracts one (size x size) matrice from the other
  a, b: ptr to 2D array containing matrix values
  size: dimension of a, b, and res
  returns: ptr to malloc'd array containing result
*/
int** minus(int** a, int** b, int size){
  int** res = createArray(size);
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      res[i][j] = a[i][j] - b[i][j];
    }
  }
  return res;
}

/*
  Base case for Strassen algorithm
  a, b: ptr to 2x2 matrices
  returns ptr to malloc'd result matrix
*/
int** base(int** a ,int** b){
  int dim = 2;
  //create result matrix
  int** res = malloc(dim * sizeof(int*));
  for (int i=0; i<dim; i++){
    res[i] = malloc(dim * sizeof(int*));
  }

  int p1, p2, p3, p4 , p5, p6, p7;

  p1= a[0][0]*(b[0][1]-b[1][1]);  //A(F-H)
  p2= (a[0][0]+a[0][1])*b[1][1];  //(A+B)H
  p3= (a[1][0]+a[1][1])*b[0][0];  //(C+D)E
  p4= a[1][1]*(b[1][0]-b[0][0]);  //D(G-E)
  p5= (a[0][0]+a[1][1])*(b[0][0]+b[1][1]);  //(A+D)(E+H)
  p6= (a[0][1]-a[1][1])*(b[1][0]+b[1][1]);  //(B_D)(G+H)
  p7=(a[0][0]-a[1][0])*(b[0][0]+b[0][1]);   //(A-C)(E+F)

  res[0][0] = p5 + p4 - p2 + p6;
  res[0][1] = p1 + p2;
  res[1][0] = p3 + p4;
  res[1][1] = p1 + p5 - p3 - p7;

  return res;
}

//Take the 4 submatrices (size n/2 x n/2) and pieces them into a newly allocated
// n x n matrix
/*
  Take 4 submatrices of dimensions (n/2 x n/2) and fills into malloc'd
  n x n matrix
  m11: upper left submatrix
  m12: upper right submatrix
  m21: lower left submatrix
  m22: lower right submatrix
  matrixSize: size of result matrix
  returns: ptr to 2D array containing full matrix
*/
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

/*create matrix of dimensions (size x size) by splitting mat
  Called by parallel_strassen()
  mat = matrix to split
  startrow = begin bound of mat to be copied into res
  startcol = begin bound of mat to be copied into res
  res = n/2 x n/2 matrix with appropriate vals copied in.
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

/*
  Splits two matrices into 8 submatrices of size (half x half)
  a, b: ptr to original ((half*2) x (half*2)) matrices
  half: dimension of submatrices
  quadrant: int from 0->3
  returns: ptr to newly malloc'd strass_arg struct
*/
strass_arg* make_struct(int** a, int** b, int half, int quadrant){
  strass_arg* args = malloc(sizeof(strass_arg));
  args->size = half;
  switch (quadrant){
    case 0: //upper left
      args->a = subdivide(a, 0, 0, half);  //A
      args->b = subdivide(b, 0, 0, half);  //E
      break;
    case 1: //upper right
      args->a = subdivide(a, 0, half, half);  //B
      args->b = subdivide(b, 0, half, half);  //F
      break;
    case 2: //lower left
      args->a = subdivide(a, half, 0, half);  //C
      args->b = subdivide(b, half, 0, half);  //G
      break;
    case 3: //lower right
      args->a = subdivide(a, half, half, half);  //D
      args->b = subdivide(b, half, half, half);  //H
      break;
    default:
      fprintf(stderr, "Invalid case\n");
      break;
  }
  return args;
}

/*
  Calculates matrices for each of the 7 multiplication operations that happen
  per level of recursion in Strassen algorithm

  subs: ptr to array of strass_arg ptrs, each entry points to strass_arg
  struct containing either upper left, upper right, lower left, or lower
  right quadrant of matrix a or b
  n: int from 0->6 that represents which of the 7 multiplications' input
  matrices to calculate
  returns: ptr to malloc'd strass_arg struct containing input matrices to
  appropriate one of the seven multiplcations
*/
strass_arg* make_args(strass_arg** subs, int n){
  strass_arg* args = malloc(sizeof(strass_arg));
  args->size = subs[0]->size;
  int size = args->size;
  int** A = subs[0]->a;
  int** B = subs[1]->a;
  int** C = subs[2]->a;
  int** D = subs[3]->a;
  int** E = subs[0]->b;
  int** F = subs[1]->b;
  int** G = subs[2]->b;
  int** H = subs[3]->b;

  switch (n){
    case 0:
      args->a = A;
      args->b = minus(F, H, size);
      break;
    case 1:
      args->a = plus(A, B, size);
      args->b = H;
      break;
    case 2:
      args->a = plus(C, D, size);
      args->b = E;
      break;
    case 3:
      args->a = D;
      args->b = minus(G, E, size);
      break;
    case 4:
      args->a = plus(A, D, size);
      args->b = plus(E, H, size);
      break;
    case 5:
      args->a = minus(B, D, size);
      args->b = plus(G, H, size);
      break;
    case 6:
      args->a = minus(A, C, size);
      args->b = plus(E, F, size);
      break;
    default:
      fprintf(stderr, "Invalid multiplication case.\n");
      break;
  }
  return args;
}

/*
  Combine step for Strassen algorithm. Takes result of the 7 multiplication
  operations, use to compute the entries of the quadrants of the final result matrix
  p: array of ptrs to 2D arrays containing the results of the 7 multiplication
  operations
  size: dimension of result matrix
  returns: array of ptrs to 2D arrays, one for each quadrant of result matrix.
*/
int*** make_q(int*** p, int size){
  int*** q = malloc(sizeof(int**) * 4);
  for (int i=0; i<4; i++){
    q[i] = createArray(size);
    switch (i){
      case 0:
        q[i] = plus(minus(plus(p[4], p[3], size), p[1], size), p[5], size);
        break;
      case 1:
        q[i] = plus(p[0], p[1], size);
        break;
      case 2:
        q[i] = plus(p[2], p[3], size);
        break;
      case 3:
        q[i] = minus(minus(plus(p[0], p[4], size), p[2], size), p[6], size);
        break;
    }
  }
  return q;
}

/*
  Recursive, parallelized implementation of Strassen algorithm
  ptr_array: ptr to strass_arg containing input matrices a and b
  returns: ptr to 2D array containing entries of final matrix
*/
void* parallel_strassen(void* ptr_array){
  strass_arg *args = ptr_array;
  int** a = args->a;
  int** b = args->b;
  int matrixSize = args->size;
  int** res;

  //base case
  if (matrixSize <= 2){
    res = base(a, b);

  } else {
    int half = matrixSize/2;
    strass_arg* args_in[7];
    strass_arg* subdivided[4];
    pthread_t threads[7];

    //Split a and b into 4 submatrices each
    for (int n=0; n<4; n++){
      subdivided[n] = make_struct(a, b, half, n);
    }
    //Initialize the threads, generate p1 -> p7
    for (int n=0; n<7; n++){
      //prepare inputs for the 7 multiplications
      args_in[n] = make_args(subdivided, n);
      if (pthread_create(&threads[n], NULL, parallel_strassen, args_in[n]) == -1){
        fprintf(stderr, "Can't create thread t[%d]\n", n);
      }
    }
    //Wait for threads to complete, store p1 -> p7 in res_array
    void* res_array[7];
    for (int i=0; i<7; i++){
      if (pthread_join(threads[i], &res_array[i]) == -1){
        fprintf(stderr, "Can't join thread t[%d]\n", i);
      }
    }

    //combine step for strassen, calculate Q1 -> Q4
    int*** q = make_q((int***)res_array, half);
    //reassemble matrix
    res = merge(q[0], q[1], q[2], q[3], matrixSize);
  }
  return (void*)res;
}

/*
  version of Strassen algorithm to be compatible with our benchmarking code
  size: dimension of square matrices to multiply (must be power of 2)
*/
void MY_STRASSEN(int size){
    srand(time(NULL));
    strass_arg in;
    in.a = createArray(size);
    in.b = createArray(size);
    in.size = size;

    int** res = (int**)parallel_strassen((void*)&in);
}

int main(int argc, char* argv[]){
  srand(time(NULL)); //randomize seed <-- should only be called once

  //initialize original struct
  strass_arg in;
  in.a = createArray(gMatrixSize);
  in.b = createArray(gMatrixSize);
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
