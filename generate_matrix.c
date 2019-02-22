#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int *random_matrix1;
int *random_matrix2;

void generate_matrix(int size)
{
  // Make sure size argument inputed
  // if (argc != 2) {
  //   fprintf(stderr, "You need to give 1 argument for matrix size\n");
  //   return 1;
  // }

  // Create random matrix
  // int size = atoi(argv[1]);
  //printf("Size is: %i\n", size );
  random_matrix1 = (int *)malloc(size * size * sizeof(int));
  random_matrix2 = (int *)malloc(size * size * sizeof(int));
  int i, j;

  srand(time(NULL)); //randomize seed
  for(i = 0; i<size; i++){
      for(j = 0; j<size; j++){
          *(random_matrix1 + i*size + j) = rand() % 100;
          *(random_matrix2 + i*size + j) = rand() % 100;
      }
  }

  // Print matrix to Standard Output
  // fprintf(stdout, "Matrix: \n");
  // for (int i = 0; i < size; i++) {
  //   for (int j = 0; j < size; j++) {
  //     fprintf(stdout, "%i ", *(random_matrix + i*size + j));
  //   }
  //   fprintf(stdout, "\n");
  // }
}

// int main() {
//   generate_matrix(3);
//   return 0;
// }
