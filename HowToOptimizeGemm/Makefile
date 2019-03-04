test_MMult.o: parameters.h test_MMult.c compare_matrices.c REF_MMult.c MMult_4x4_5.c copy_matrix.c random_matrix.c dclock.c
	gcc -g -c test_MMult.c compare_matrices.c REF_MMult.c MMult_4x4_5.c copy_matrix.c random_matrix.c dclock.c

test_MMult: test_MMult.o compare_matrices.o REF_MMult.o MMult_4x4_5.o copy_matrix.o random_matrix.o dclock.o
	gcc -g test_MMult.o compare_matrices.o REF_MMult.o MMult_4x4_5.o copy_matrix.o random_matrix.o dclock.o -o test_MMult
