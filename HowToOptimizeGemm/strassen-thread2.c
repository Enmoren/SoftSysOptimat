/* Parallelized implementation of Strassen Matrix Multiplication Algorithm

gcc strassen-thread.c -lpthread -o threaded && ./threaded
Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
Copyright: MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

//TODO: More header comments for each function

static int gMatrixSize = 2;

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

int** createArray(int matrixSize){
    // int* values = calloc(matrixSize*matrixSize, sizeof(int));
    int** rows = malloc(matrixSize * sizeof(int*));
    for (int i=0; i<matrixSize; ++i){
        rows[i] = malloc(matrixSize * sizeof(int*));
        for (int j =0; j<matrixSize; ++j){
          rows[i][j] = rand() % 100;
        }
    }
    return rows;
}

//Base case for calculating Strassen algorithm
//Caller frees
int** base(int** a ,int** b){
  int dim = 2;
  //create result matrix
  int** res = malloc(dim * sizeof(int*));
  for (int i=0; i<dim; i++){
    res[i] = malloc(dim * sizeof(int*));
  }

  int m1, m2, m3, m4 , m5, m6, m7;
  // m1= (a[0][0] + a[1][1]) * (b[0][0] + b[1][1]);
  // m2= (a[1][0] + a[1][1]) * b[0][0];
  // m3= a[0][0] * (b[0][1] - b[1][1]);
  // m4= a[1][1] * (b[1][0] - b[0][0]);
  // m5= (a[0][0] + a[0][1]) * b[1][1];
  // m6= (a[1][0] - a[0][0]) * (b[0][0]+b[0][1]);
  // m7= (a[0][1] - a[1][1]) * (b[1][0]+b[1][1]);
  //
  // res[0][0] = m1 + m4 - m5 + m7;
  // res[0][1] = m3 + m5;
  // res[1][0] = m2 + m4;
  // res[1][1] = m1 - m2 + m3 + m6;

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
      args->a = subdivide(a, 0, 0, half); //A
      args->b = subdivide(b, 0, 0, half);  //E
      break;
    case 1:
      args->a = subdivide(a, 0, half, half);  //B
      args->b = subdivide(b, 0, half, half);  //F
      break;
    case 2:
      args->a = subdivide(a, half, 0, half);  //C
      args->b = subdivide(b, half, 0, half);  //G
      break;
    case 3:
      args->a = subdivide(a, half, half, half);  //D
      args->b = subdivide(b, half, half, half);  //H
      break;
    default:
      fprintf(stderr, "Invalid case\n");
      break;
  }
  // printargs(args);
  return args;
}

int** plus(int** a, int** b, int size){
  int** res = createArray(size);
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      res[i][j] = a[i][j] + b[i][j];
    }
  }
  return res;
}

int** minus(int** a, int** b, int size){
  int** res = createArray(size);
  for (int i=0; i<size; i++){
    for (int j=0; j<size; j++){
      res[i][j] = a[i][j] - b[i][j];
    }
  }
  return res;
}

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
      puts("Invalid multiplication case.");
      break;
  }
  return args;
}

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

//psuedo Mergesort implementation of Strassen algorithm
void* parallel_strassen(void* ptr_array){
  strass_arg *args = ptr_array;
  int** a = args->a;
  int** b = args->b;
  int matrixSize = args->size;
  int** res;

  if (matrixSize <= 2){
    res = base(a, b);

  } else {  //split up matrix into quadrants
    int half = matrixSize/2;
    strass_arg* args_in[7];
    strass_arg* subdivided[4];
    pthread_t threads[7];

    //Split a and b into 4 submatrices
    for (int n=0; n<4; n++){
      subdivided[n] = make_struct(a, b, half, n);
    }
    //Initialize the threads
    for (int n=0; n<7; n++){
      args_in[n] = make_args(subdivided, n);
      if (pthread_create(&threads[n], NULL, parallel_strassen, args_in[n]) == -1){
        fprintf(stderr, "Can't create thread t[%d]\n", n);
      }
    }
    //Wait for threads to complete
    void* res_array[7];
    for (int i=0; i<7; i++){
      if (pthread_join(threads[i], &res_array[i]) == -1){
        fprintf(stderr, "Can't join thread t[%d]\n", i);
      }
    }

    //combine step for strassen.
    int*** q = make_q((int***)res_array, half);
    //reassemble matrix
    res = merge(q[0], q[1], q[2], q[3], matrixSize);
  }
  return (void*)res;
}

void MY_STRASSEN(int size){
    srand(time(NULL));
    strass_arg in;
    in.a = createArray(size);
    in.b = createArray(size);
    in.size = size;

    int** res = (int**)parallel_strassen((void*)&in);
}

// int main(int argc, char* argv[]){
//   srand(time(NULL)); //randomize seed <-- should only be called once
//
//   //initialize original struct
//   strass_arg in;
//   in.a = createArray(gMatrixSize);
//   in.b = createArray(gMatrixSize);
//   in.size = gMatrixSize;
//
//   printf("Array a is at address: %p\n", &in.a);
//   printf("Array b is at address: %p\n", &in.b);
//   printargs(&in); //print values of initial struct
//
//   //Recursive, parallelized implementation of Strassen matmul
//   int** res1 = (int**)parallel_strassen((void*)&in);
//
//    printf("\nAfter multiplication using Strassen's algorithm \n");
//    printmat(res1, gMatrixSize);
//
//    return 0;
// }
