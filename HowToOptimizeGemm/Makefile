NEW  := MMult_4x4_5

CC         := gcc
LINKER     := $(CC)
CFLAGS     := -O2 -Wall -msse3
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

# Old makefile
# test_MMult.o: parameters.h test_MMult.c compare_matrices.c REF_MMult.c MMult_4x4_10.c copy_matrix.c random_matrix.c dclock.c
# 	gcc -g -c test_MMult.c compare_matrices.c REF_MMult.c MMult_4x4_10.c copy_matrix.c random_matrix.c dclock.c
#
# test_MMult: test_MMult.o compare_matrices.o REF_MMult.o MMult_4x4_10.o copy_matrix.o random_matrix.o dclock.o
# 	gcc -g test_MMult.o compare_matrices.o REF_MMult.o MMult_4x4_10.o copy_matrix.o random_matrix.o dclock.o -o test_MMult
