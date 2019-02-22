#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int *random_matrix1;
int *random_matrix2;

int* generate_matrix(int size)
{

  // Create random matrix
  int *random_matrix = (int *)malloc(size * size * sizeof(int));
  int i, j;

  srand(time(NULL)); //randomize seed
  for(i = 0; i<size; i++){
      for(j = 0; j<size; j++){
          *(random_matrix + i*size + j) = rand() % 100;
      }
  }
  return random_matrix;
}

// int main(int argc, char* argv[]) {
//   // Make sure size argument inputed
//   if (argc != 2) {
//     fprintf(stderr, "You need to give 1 argument for matrix size\n");
//     return 1;
//   }
//   int size = atoi(argv[1]);
//   int *random_matrix;
//   random_matrix = generate_matrix(size);
//   // Print matrix to Standard Output
//   fprintf(stdout, "Matrix: \n");
//   for (int i = 0; i < size; i++) {
//     for (int j = 0; j < size; j++) {
//       fprintf(stdout, "%i ", *(random_matrix + i*size + j));
//     }
//     fprintf(stdout, "\n");
//   }
//   return 0;
// }
