<<<<<<< HEAD
NEW  := MMult_1x4_9
=======
NEW  := MMult_4x4_5
>>>>>>> 6a036a4c2e1a4c59f2d23e86d3d4cf4c86000397

CC         := gcc
LINKER     := $(CC)
CFLAGS     := -O0 -Wall -msse3
LDFLAGS    := -lm

UTIL       := copy_matrix.o \
              compare_matrices.o \
              random_matrix.o \
              dclock.o \
              REF_MMult.o \
              print_matrix.o

TEST_OBJS  := test_MMult.o $(NEW).o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test_MMult.x: $(TEST_OBJS) $(UTIL) parameters.h
	$(LINKER) $(TEST_OBJS) $(UTIL) $(LDFLAGS) \
  $(BLAS_LIB) -o $(TEST_BIN) $@

test_MMult: test_MMult.x $(TEST_OBJS) $(UTIL) parameters.h
	./test_MMult.x

clean :
	rm *.o
