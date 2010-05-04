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