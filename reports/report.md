## Optimat: Exploring High-Performance Matrix Multiplication in C
#### Authors: Enmo Ren, Cassandra Overney, Hwei-Shin Harriman

### Project Vision
For this project, we implement matrix multiplication using high-performance scientific computation. Our MVP is to use caching and simple matrix operations (i.e. transpose) to optimize matrix multiplication. Our medium range goal is to apply a divide and conquer algorithm with parallelization. Our stretch goal is to apply our matrix multiplication algorithms to mesh networking. Another stretch goal involves implementing the Coppersmith-Winograd algorithm, which is currently one of the most efficient algorithms for matrix multiplication.
### Learning Goals
**Our collective learning goals include:**
- Improve our C programming skills (designing small tools)
- Gain exposure in optimization methods

**Our individual learning goals include:**
- **Cassandra:** I love linear algebra, so I just want to apply what we are learning in class to my favorite math topic. I am also interested in tiling multiplication with loop nest optimization and potentially learning about mesh networks.
- **Enmo**: I want to get better at programming in C, as well as familiarize myself with shared memory parallelism and distributed computing for optimization. By potentially implementing multi-thread processing for our program, I will gain a deeper understanding of the operating system.
- **Hwei-Shin**: I am interested in learning about memory parallelism and the tradeoffs of different matrix multiplication algorithms. I am also interested in gaining an intuitive, low-level understanding of why specific algorithms and optimization techniques are preferred. I want to gain experience writing clean and robust code in C. I would also like to gain some exposure to manipulating meshes using linear algebra if we have the time.

### Resources
A full list of the resources we used can be found [here](https://github.com/Enmoren/SoftSysOptimat/blob/master/reports/resources.md). We have highlighted a few that we found to be the most helpful below:

To understand different matrix multiplication optimization techniques, we went through a Github tutorial, [How to Optimize Gemm](https://github.com/flame/how-to-optimize-gemm). The tutorial contains not only a step-by-step optimization workflow but also all of the supporting test functions, including gflops computation, Matlab performance plotting, random matrix generation, and matrix comparison. The different optimization techniques range from using regular registers, pointers, indirect addressing, vector registers, and blocking. More specific notes from this tutorial can be found [here](https://docs.google.com/document/d/1UOtbZU6rpAvis5PiRWvT3FCVGhryVNJl_j-GeoD48tg/edit?usp=sharing).

Another great resource that helped us gain a basic understanding of the Strassen algorithm, which uses a divide-and-conquer algorithm to reduce the number of operations, is  [Distributed Algorithms and Optimization](https://stanford.edu/~rezab/classes/cme323/S16/notes/Lecture03/cme323_lec3.pdf). We created a multi-threading implementation of the Strassen algorithm based on this paper, which can be found [here](https://github.com/Enmoren/SoftSysOptimat/blob/master/strassen-thread2.c)
Another Github [repo](https://github.com/sangeeths/stanford-algos-1/blob/master/strassen-recursive-matrix-multiplication.c) offers us a general overview of how recursive Strassen works.

### Results
#### Optimization Tutorial
After understanding the techniques within the How to Optimize Gemm tutorial, we implemented them and compared their performances. The code can be found [here](https://github.com/Enmoren/SoftSysOptimat/tree/master/HowToOptimizeGemm). The graphs below show comparisons between the non-optimized approach and several optimization techniques.

<p align="center"> <img src ="https://github.com/Enmoren/SoftSysOptimat/blob/master/HowToOptimizeGemm/Figures/performance_update.png"/> </p>

The first graph was generated right after finishing the tutorial. The leading dimension refers to the size of the matrix. Gflops corresponds to 10^6 floating point operations per second, so the higher the number, the better the performance is. (The y-axis label should be Gflop/sec not Gflops/sec.) Compared to the tutorial, we did not observe a significant change in gflops. We originally thought it was a hardware problem or caused by inconsistent testing conditions. After taking a closer look at the code and attempting to run it on another machine, we found that the problem was a lack of a `make clean`. Basically, the same object code was being reused even after changing the matrix multiplication function. After adding a simple `make clean` that deletes all of the object code (ran before every make), we obtained a very different performance figure:

<p align="center"> <img src ="https://github.com/Enmoren/SoftSysOptimat/blob/master/HowToOptimizeGemm/Figures/performance2.png"/> </p>

MMult0 corresponds to the naive/ brute-force approach. MMult_1x4 corresponds to using regular registers and pointers to compute 4 elements of the matrix C (AxB=C) at once. Since four inner products are computed at the same time, a loop counter only needs to be updated once every 8 floating point operations (each inner product involves an addition and multiplication resulting in 2 floating point operations). MMult_4x4 corresponds to the most optimized approach which includes 4x4 blocking, vector registers, pointers, and packing. It makes sense that MMult_4x4 has the best performance followed by MMult_1x4 and then MMult0. MMult_4x4 is almost twice as fast as MMult_1x4. We used an optimization flag of `-O0` because we did not want to introduce unknown optimizations when testing our matrix multiplication functions.

In order to utilize special instructions that are part of the Streaming SIMD Extensions 3 instruction set, we needed to place the data for each 4x4 block in vector registers. The special instructions involve performing two inner products (two multiplications and two additions) per clock cycle, resulting in four floating point operations per clock cycle. Each vector register can hold two double precision numbers. Using them drastically improved the performance.

[`MMult_4x4_10.c`](https://github.com/Enmoren/SoftSysOptimat/blob/master/HowToOptimizeGemm/MMult_4x4_10.c) is the program where vector registers and vector operations are first introduced. To use these features, several C libraries need to be included such as `mmintrin.h`, `xmmintrin.h`, `pmmintrin.h`, and `emmintrin.h`. Additionally, the flag, `-msse3`, has to be included at compile time.

A vector register is defined as the following union:
```c
typedef union {
  __m128d v;
  double d[2];
} v2df_t;
```
The double array allows us to access the two double precision numbers stored by the vector register. `__m128d` is a data type that the compiler uses to store data in a XMM 128 bit register. It is part of the Streaming SIMD Extensions libraries. In `MMult_4x4_10.c`, vector registers are created when computing every 4x4 block of matrix C. If our 4x4 block matrix looks like:
```
C(0,0), C(0,1), C(0,2), C(0,3).
C(1,0), C(1,1), C(1,2), C(1,3).
C(2,0), C(2,1), C(2,2), C(2,3).
C(3,0), C(3,1), C(3,2), C(3,3).
```
If we want to find C(0,0) and C(1,0) using vector registers, we would execute the following procedure:
1. Instantiate vector register variables
```c
v2df_t c_00_c_10_vreg;
v2dd_t a_0p_a_1p_vreg;
v2dd_t b_p0_vreg;
```
2. Initialize vector register variable for matrix C
```c
c_00_c_10_vreg.v = _mm_setzero_pd();
```
`_mm_setzero_pd` sets the vector register to 0. This variable stores the results of C(0,0) and C(1,0).

3. Compute `c_00_c_10_vreg` using `a_0p_a_1p_vreg` and `b_p0_vreg`
```c
for (p=0; p<k; p++){
    a_0p_a_1p_vreg.v = _mm_load_pd( (double *) &A( 0, p ) );
    b_p0_vreg.v = _mm_loaddup_pd( (double *) b_p0_pntr++ );
    c_00_c_10_vreg.v += a_0p_a_1p_vreg.v * b_p0_vreg.v;
}
```
The `for` loop iterates through the size of the matrices. ` _mm_loaddup_pd` loads and duplicates the inputted double. `b_p0_pntr` points to B(0,0). `c_00_c_10_vreg.v` is computed using the inner product approach to matrix multiplication in which the first two rows of A are dotted with the first two columns of B.
4. Lastly, we update C(0,0) and C(1,0) using `c_00_c_10_vreg`
```c
C(0,0) += c_00_c_10_vreg.d[0];
C(1,0) += c_00_c_10_vreg.d[1];
```
We use the double array in the union defined above to access the two values stored in the vector register.

#### Implementing the Strassen Algorithm

An alternative to the naive approach of matrix multiplication is the Strassen algorithm. This approach makes use of divide and conquers to recursively divide large, square matrices into subsections which can then be used to calculate the product of the two matrices. A visual representation of how the operations required for each level of recursion relate is shown below. [(source)](https://github.com/sangeeths/stanford-algos-1/blob/master/strassen-recursive-matrix-multiplication.c)

```
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
```
If the original matrices being multiplied are of dimensions `N x N`, then each level of recursion splits the two matrices into four subsections,  each of dimensions `N/2 x N/2`. These four submatrices are then used to calculate 7 matrix products. Once the submatrices are of size `2 x 2` (in which case the 7 products are no longer matrix multiplications but simple arithmetic multiplications), the results are passed to the previous recursive call, until the original 7 original `N/2 x N/2`-sized matrix products have been completed.

After, these 7 products are used to calculate each of the four quadrants of the final `N x N`-sized matrix. This matrix is the final answer and should result in the same matrix as a brute-force matrix multiplication using nested `for` loops.

The nice thing about the Strassen algorithm is that the seven multiplication operations can be calculated independent from one another. This means that the algorithm is parallelizable. For each level of recursion, we created 7 threads, each one configured to calculate one of the seven multiplications. We defined `strass_arg` as a `typedef struct` as follows:

```c
typedef struct {
  int** a;
  int** b;
  int size;
} strass_arg;
```
where `a` and `b` are pointers to 2D matrices and `size` is the dimension of `a` and `b`.

The code below initiates the 7 threads.
```c
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
```
`make_struct()` is a function that, when passed the iterable variable `n`, returns a `strass_arg` struct which points to copies of the upper left, upper right, lower left, or lower right quadrants of the input matrices respectively. Therefore, the `subdivided` array contains 8 matrices (the 2 input matrices split into 4 quadrants each). `make_args()` takes these 8 matrices and uses them to calculate the pairs of matrices required for each of the 7 multiplications (14 matrices total), and returns them in the form of 7 `strass_arg` objects, one for each multiplication operation.
After waiting for all of the threads to complete, we move on to the final stage of calculating the entries in each of the four quadrants and piecing the final matrix back together. This approach helps to improve the runtime of the algorithm when compared to the naive approach or even the non-threaded, recursive version of the Strassen algorithm.

We decided to use threads instead of subprocesses to solve the 7 matrix multiplications for a couple of reasons. First, threads are generally faster than processes to initialize, and the speed of our program is an important factor in any of our design decisions. However, threads slow down if they have to deal with too much data, so we decided that 7 new threads would be added at each recursive call to our multiplication function. Only after all of these threads have completed do we move on to the combining step.

##### Performance of our Strassen algorithm
The graph below shows the performance of our implementation of the Strassen algorithm.

<p align="center"> <img src ="https://github.com/Enmoren/SoftSysOptimat/blob/master/HowToOptimizeGemm/Figures/performance_strassen.png"/> </p>

As we can observe from the graph, the performance of our implemented Strassen algorithm decays drastically once the size of the matrix is bigger than 8. This is different from our expectations. Originally, we thought the performance of the Strassen algorithm would grow as the size of the matrix increases since the algorithm executes fewer arithmetic operations than the naive approach. However, we observed that the gflops for the Strassen algorithm were much lower than the gflops for the naive approach (reference MMult0  in above performance figure).  One possible explanation for the unexpected performance is the structure of how our code works. For our implementation of the Strassen algorithm, most of the functions dynamically allocate space in the heap to store their resulting matrices. Because of the recursive structure of the code, it was extremely difficult to know when to free these temporary matrices (free-ing immediately after the base case caused a segmentation fault), which may have interfered with the runtime of the algorithm. Another problem we noticed is that the multithreading Strassen algorithm caused a segmentation fault error with a matrix larger than 64 x 64. We think the problem was because the large dimensions introduce multiple levels of recursion, and for each recursive call we created 7 additional threads. So, for one level of recursion there are 7 threads, but 2 levels of recursion creates 49 threads, since each thread also initializes 7 additional threads. Once the number of threads reaches the limit of hardware, the operating system will actually run only a small number of threads at a given point of time, which introduced a segmentation fault in our code.

### Reflection
We achieved our MVP as well as our medium stretch goal. We were able to perform matrix multiplication more efficiently using register blocking, subroutine, and vector registers. We also implemented multithreading Strassen algorithm, which utilizes an algorithms-approach to improve the performance of matrix multiplication. We did not implement our stretch goal of applying our matrix multiplication algorithms to mesh networking nor implementing the Coppersmith-Winograd algorithm. Overall, we all got the opportunity to work on our learning goals (which were to improve our C programming skills and learn optimization techniques).

**Our individual reflections include:**
- **Cassandra:** I was able to learn about a lot of different optimization techniques including blocking, vector registers and contiguously packing data with pointers. I didn’t end up doing loop nest optimization or mesh networks, but that could be a potential idea for project 2. It was interesting to review different ways to do matrix multiplication (dot product method versus linear combination method) while thinking about ways to optimize the naive approach. I was also able to practice writing more complicated Makefiles by introducing a `make clean` and variables. With a Makefile, I realized the importance of splitting different functionalities into small tools. For example, we have separate programs to generate random matrices, compare matrices, and time functions.

- **Enmo**: I gained a lot of experience writing code in C. Through implementing matrix multiplication algorithm, I familiarized myself with working with arrays and pointers. I also learned about high-performance techniques in matrix-multiplication such as register blocking, tiling, and multithreading. Besides, I learned about visualization of program’s performance. I would love to try memory mountain for a future project. I would like to obtain an efficient GPU implementation if I had more time.

- **Hwei-Shin**: Through tackling this project, I feel like I have become more comfortable programming in C. By working on the matrix multiplication functions, I found that using a combination of pointers and arrays, as well as passing them between functions has become a lot more intuitive for me. Also, I was able to implement the Strassen algorithm using both the simple approach and a multi-threaded approach. Refactoring my code so that the primary function could meet the requirements necessary to use the `pthreads` library I found to be particularly good practice. If I had more time, I would have liked to refactor my code even more to make it require less dynamic memory. I also learned about tiling, caching, register blocking, and several options for high-performance matrix multiplication, including libraries that account for hardware specifications of specific computers (such as the Intel library).

### Potential Extensions
1. Make the multithreading Strassen algorithms work for more leading dimension, validate our implementation to see if we can improve the performance
2. Develop GPU implementation of Strassen algorithms
3. Compare optimization techniques with various optimization flags (-O0 vs -O2 vs -O3)
4. Apply our matrix multiplication algorithms to mesh networking
5. Implement the Coppersmith-Winograd algorithm
