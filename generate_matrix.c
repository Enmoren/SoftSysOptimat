#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
  // Make sure size argument inputed
  if (argc != 2) {
    fprintf(stderr, "You need to give 1 argument for matrix size\n");
    return 1;
  }

  // Create random matrix
  int size = atoi(argv[1]);
  printf("Size is: %i\n", size );
  int random[size][size];
  int i, j;

  srand(time(NULL)); //randomize seed
  for(i = 0; i<size; i++){
      for(j = 0; j<size; j++){
          random[i][j] = rand() % 100;
      }
  }

  // Print matrix to Standard Output
  fprintf(stdout, "Matrix: \n");
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      fprintf(stdout, "%i ", random[i][j]);
    }
    fprintf(stdout, "\n");
  }

  return 0;
}
