Overall build

1. Type make
2. Produces 3 executables: fjlt, kdtree, main

3. fjlt - runs a FJLT transform on randomly 
            generated nxd to nxk
4. kdtree - builds a kd-tree from nxd dataset
5. main - Generates random nxd data, and transforms it to nxk 
            using FJLT and then builds the tree



Parallel sort:

To compile on Hogwarts:

1.  run this line in the shell
    source /opt/intel/Compiler/11.0/084/bin/iccvars.sh intel64
    source /opt/intel/Compiler/11.1/064/bin/ifortvars.sh intel64

2.  use ICC to compile it:
    icc -openmp proj.cpp myRNG.cpp -o parallelSort.exe

3.  executable takes 3 inputs:
	i. log(# elements in array)
       ii. print input array?  (1 = Yes)
      iii. print output array? (1 = Yes)

       ex. OMP_NUM_THREADS=4 ./parallelSort.exe 21 0 0
           This will generate a random array of floats with 2^21 elements
           and sort it in parallel using 4 threads.  The input nor the
           output will be printed to the screen.


Fast Johnson-Lindenstrauss Transform:

FJLT implementation complete

To compile:

1. g++ main.cc fjlt.cc io.cc -o fjlt -fopenmp -lfftw3 -lblas -lm

2. As of now executable doesnt take any inputs.
   Just creates some data randomly and does the projections on those

Building kd-tree

Compiling:
icc -openmp -openmp-task intel proj.cpp

Executing:
OMP_NUM_THREADS=<N> ./a.out n d
where, N = number of openmp threads
	   n = log of dataset size
	   d = log of dimension size

Example:
OMP_NUM_THREADS=4 ./a.out 8 7

The above command does:
1. Generate a random data points of size 2^8. Each point will belong to a 2^7 dimension space.
2. Run the program with 4 openmp threads

IMPORTANT:
It is important to compile the program as instructed above, because the program uses Intel task-queuing pragma
to parallelize recursive functions. This is not a standard OpenMP feature and other compiles cannot compile the pragmas.
If a different compiler is used, the pragmas will be ignored and the performance of the program will be badly affected.
