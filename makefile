RM := rm -rf

# Compiler that we are using
CC=icpc

# Optimization levels and any other flags
CFLAGS=-O3 -xT

# Standard libraries that we will use for all the files
LIBS := -openmp -openmp-task intel -lm

# Any specific libraries needed by the particular executable
FJLT_LIBS := -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core

# All executable specific object files that need to be compiled.
FJLT_OBJS := fjtest.o fjlt.o io.o
KTRE_OBJS := kdtest.o parSort.o rerank.o build_tree.o myRNG.o
MAIN_OBJS := main.o fjlt.o io.o parSort.o rerank.o build_tree.o myRNG.o

# All objects files for easier cleanup
OBJS := $(FJLT_OBJS) $(KTRE_OBJS) $(MAIN_OBJS)

# Executables we are producing
EXECUTABLES := fjlt kdtree main

all: $(EXECUTABLES)

# Linking step for each executable
fjlt: $(FJLT_OBJS)
	@echo 'Building target: $@'
	$(CC) -o $@ $(FJLT_OBJS) $(FJLT_LIBS) $(LIBS)

kdtree: $(KTRE_OBJS)
	@echo 'Building target: $@'
	$(CC) -o $@ $(KTRE_OBJS) $(KTRE_LIBS) $(LIBS)

main: $(MAIN_OBJS)
	@echo 'Building target: $@'
	$(CC) -o $@ $(MAIN_OBJS) $(FJLT_LIBS) $(KTRE_LIBS) $(LIBS)

# Rules to build the .o files
%.o: %.cpp
	$(CC) $(CFLAGS) -c -o "$@" "$<" $(LIBS)

clean:
	-$(RM) $(OBJS) $(EXECUTABLES)

.PHONY: all clean dependents
