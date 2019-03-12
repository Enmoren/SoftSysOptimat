## Optimat Project Update
#### Authors: Enmo Ren, Cassandra Overney, Hwei-Shin Harriman

### Project Vision
For this project, we will be implementing matrix multiplication using high-performance scientific computation. Our MVP is to use caching and simple matrix operations (i.e. transpose) to optimize matrix multiplication. Our medium range goal is to apply a divide and conquer algorithm with parallelization. Our stretch goal is to apply our matrix multiplication algorithms to mesh networking. Another stretch goal involves implementing the Coppersmith- Winograd algorithm, which is currently one of the most efficient algorithms for matrix multiplication.
### Learning Goals
**Our collective learning goals include:**
- Improve our C programming skills (designing small tools)
- Gain exposure in optimization methods

**Our individual learning goals include:**
- **Cassandra:** I love linear algebra, so I just want to apply what we are learning in class to my favorite math topic. I am also interested in tiling multiplication with loop nest optimization and potentially learning about mesh networks.
- **Enmo**: I want to get better at programming in C, as well as familiarize myself with shared memory parallelism and distributed computing for optimization. By potentially implementing multi-thread processing for our program, I will gain a deeper understanding of the operating system.
- **Hwei-Shin**: I am interested in learning about memory parallelism and the tradeoffs of different matrix multiplication algorithms. I am also interested in gaining an intuitive, low-level understanding of why specific algorithms and optimization techniques are preferred. I want to gain experience writing clean and robust code in C. I would also like to gain some exposure to manipulating meshes using linear algebra if we have the time.
### Resources and Current Progress
A great resource that helped us gain a basic understanding of the Strassen algorithm, which uses a divide-and-conquer algorithm to reduce the number of operations, is  [Distributed Algorithms and Optimization](https://stanford.edu/~rezab/classes/cme323/S16/notes/Lecture03/cme323_lec3.pdf). We created a basic implementation of the Strassen algorithm based on this paper, which can be found [here](https://github.com/Enmoren/SoftSysOptimat/blob/master/strassen.c)
To understand different matrix multiplication optimization techniques, we went through a Github tutorial, [How to Optimize Gemm](https://github.com/flame/how-to-optimize-gemm). The tutorial contains not only a step-by-step optimization workflow but also all of the supporting test functions, including gflops computation, Matlab performance plotting, random matrix generation, and matrix comparison. The different optimization techniques range from using regular registers, pointers, indirect addressing, vector registers, and blocking. More specific notes from this tutorial can be found [here](https://docs.google.com/document/d/1UOtbZU6rpAvis5PiRWvT3FCVGhryVNJl_j-GeoD48tg/edit?usp=sharing). After understanding these techniques, we implemented them and compared their performance. The graph below shows a comparison between several optimization techniques.

<p align="center"> <img src ="https://github.com/Enmoren/SoftSysOptimat/blob/master/HowToOptimizeGemm/performance2.png"/> </p>

MMult0 corresponds to the naive approach. MMult_1x4_9 corresponds to using regular registers and pointers to compute 4 elements of the matrix C (AxB=C) at once. MMult_4x4_5 corresponds to computing a 4x4 block of C at a time without using registers or pointers. MMult_4x4_15 corresponds to the most optimized approach which includes 4x4 blocks, vector registers, pointers, and packing. For smaller matrices, the optimized matrix multiplication algorithms have better performance. Gflops corresponds to 10^6 floating point operations per second, so the higher the number, the better the performance is. Taking a closer look at the figure, it seems like MMult_4x4_15 does not have better performance compared to MMult_1x4_9 or MMult_4x4_5. According to the tutorial, a significant change in gflops should be observed. Possible explanations for this discrepancy include:
- the Makefile (we wrote our own instead of directly copying the tutorial's, and there may be some important features that we did not use)
- the way we run the tests (the gflops fluctuated a lot when we tested our functions under different conditions)
- our processor (tutorial also uses Intel processor, so it shouldn't be the problem)

Currently, we are working on our medium range goal, which is to implement a divide and conquer the matrix multiplication algorithm using threads. Hwei-Shin and Enmo have implemented the Strassen algorithm, which makes use of the concepts of MergeSort. This task will be considered complete when we have refactored the code to use threads. Cassandra has been working through the tutorial mentioned above on various optimization techniques and implemented matrix multiplication using register blocking (whether it completely works is another question, reference figure). Lastly, Enmo wrote a script that allows us to plot the performance of different matrix multiplication algorithms and library functions and used it to compare our various implementations (reference figure above).

### Next Steps
1. Figure out why the performances of the optimization methods do not agree with the tutorial's (Cassandra)
2. Read Head First C chapter 12 on multithreading and review the content together (Cassandra, Enmo, Hwei-Shin)
3. Implement parallelization for Strassen (using multithreading)
4. Compare the performance of Strassen to Intel’s matrix multiplication library for small matrices (and possibly Python `numpy` library) using our plotting script
