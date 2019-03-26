NEW  := strassen-thread2

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
  $(BLAS_LIB) -lpthread -o $(TEST_BIN) $@

test_MMult: test_MMult.x $(TEST_OBJS) $(UTIL) parameters.h
	./test_MMult.x

clean :
	rm *.o
