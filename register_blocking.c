/* performs matrix multiplication using a nested for loop */
#include <stdio.h>
#include <stdlib.h>
#include "generate_matrix.h"
// #include <cstdlib>

#include "libjit_defs.h"

// constexpr int regsA = 4;
// constexpr int regsB = 3;

// template <size_t regsA, size_t regsB>
// void libjit_matmul_dot(size_t k, const float *a, size_t lda, const float *b,
//                        size_t ldb, float *c, size_t ldc) {
//   float8 csum[regsA][regsB] = {{0.0}};
//   for (size_t p = 0; p < k; p++) {
//     // Perform the DOT product.
//     for (size_t ai = 0; ai < regsA; ai++) {
//       float8 aa = LoaduFloat8(&A(ai * 8, p));
//       for (size_t bi = 0; bi < regsB; bi++) {
//         float8 bb = BroadcastFloat8(B(p, bi));
//         csum[ai][bi] += aa * bb;
//       }
//     }
//   }

//   // Accumulate the results into C.
//   for (size_t bi = 0; bi < regsB; bi++) {
//     for (size_t ai = 0; ai < regsA; ai++) {
//       AdduFloat8(&C(ai * 8, bi), csum[ai][bi]);
//     }
//   }
// }

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

  // libjit_matmul_dot(size, (float)random_matrix1, 0, (float)random_matrix2, 0, (float)res, 0)

  // Print matrix to Standard Output
  // fprintf(stdout, "Matrix: \n");
  // for (int i = 0; i < size; i++) {
  //   for (int j = 0; j < size; j++) {
  //     fprintf(stdout, "%i ", *(res + i*size + j));
  //   }
  //   fprintf(stdout, "\n");
  // }
  return 0;
}
