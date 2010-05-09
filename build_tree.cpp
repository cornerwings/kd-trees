#include <cstdlib>
#include <limits>
#include <float.h>
#include <omp.h>

#include <stdio.h>


#include "rerank.h"
#include "parSort.h"
#include "build_tree.h"


//#define TASKQ_DEPTH	3
int lol = 0;
double** Pv;
int** Iall;
int* flag;
int* aux_global; 
int* rerankspace;
int num_leaf_node = 0;
int S;
int D;


int select_dim( int* aux_global, int size, int D) {
	int d;
	double diff = -DBL_MAX;
	int i;
	for(i = 0; i < D; i++) {
		double*  P = Pv[i];
		double max = - DBL_MAX;
		double min = DBL_MAX;
		for(int j = 0; j < size; j++) {
			if(P[aux_global[j]] < min) min = P[aux_global[j]];
			if(P[aux_global[j]] > max) max = P[aux_global[j]];
		
			if((max - min) > diff){
				diff = max - min;
				d = i;
			}
		}
	}
	return d;
}


 void split(tree_node* parent, int* aux_global, int* flag_space, int* rerankspace, int size) {
	//printf("Size = %d\n", size);
	if(size > S) {
		int pivot = (size + 1) / 2;	
		//int dim = select_dim(aux_global, size, D) ;
		int dim = (lol++) % D;
		int* Id = Iall[dim];
		int i;
		for(i = 0; i < size; i++) {
			rerankspace[i] = Id[aux_global[i]];
		}
		rerank(rerankspace, size);
		int j = 0;
		int k =0;
		
		for( i = 0; i < size; i++) {
			if(rerankspace[i] < pivot) {
				{
					flag_space[k] = aux_global[i];
					k++;
				}
			} else {
				{
					flag_space[size - size/2 + j] = aux_global[i];
					j++;
				}
			}
		}
		for(int i = 0; i < size; i++) {
			aux_global[i] = flag_space[i];
		}
		parent->points = NULL;
		parent->dimension = dim;
		tree_node* left = (tree_node*) malloc(sizeof(tree_node));
		tree_node* right = (tree_node*) malloc(sizeof(tree_node));
		if(!left) {
			printf("Memory could not be allocated for children \n");
			exit(-1);
		}
		if(!right) {
			printf("Memory could not be allocated for children \n");
			exit(-1);
		}
		parent->left_child = left;
		parent->right_child = right;
		left->parent = parent;
		right->parent = parent;
		 #pragma intel omp taskq
		{
			//building let child
			#pragma intel omp task
			{
				split( left, aux_global, flag_space, rerankspace, size - (size / 2));
					
			}
			//building right child
			#pragma intel omp task
			{
				split( right, aux_global + size - (size / 2), flag_space + size - (size / 2), rerankspace + size - (size / 2), size / 2);		
			}
		}
	} else {
		pts* leaf_points = (pts*) malloc(sizeof(pts));
		if(!leaf_points) {
			printf("memory couldnot be allocated for leafnode \n");
			exit(-1);
		}
		leaf_points->point_ids = aux_global;
		leaf_points->size = size;
		parent->points = leaf_points;
		parent->left_child = NULL;
		parent->right_child = NULL;
	}
}

void print_tree (tree_node* parent) {
	if((parent-> left_child)) {
		print_tree(parent-> left_child);
	}
	if((parent-> right_child)) {
		print_tree(parent-> right_child);
	}
	if(parent->points) {
		int size = parent->points->size;
		int* points =  parent->points->point_ids;
		for(int i =0; i < size; i++) {
			printf("%d \n", *(points + i));
		}
	}
}

tree_node* build_tree(int size) {
	flag = new int[size];
	aux_global = new int[size];
	rerankspace = new int[size];
	tree_node* head = (tree_node*) malloc(sizeof(tree_node));
	if(!head) {
		printf("Memory could not be allocated for tree head\n");
		exit(-1);
	}
	head->parent = NULL;
	int i;
	#pragma omp parallel for private(i)
	for(i = 0 ; i < size; i++) {
		aux_global[i] = i;
	}
	
	int j = 0;
	int k = 0;
	#pragma intel omp parallel taskq
    {
        #pragma intel omp task
        split(head, aux_global, flag, rerankspace, size);
    }
	//print_tree(head);
	return head;
}

void delete_tree(tree_node* parent) {
	if(parent->right_child)
		delete_tree(parent->right_child);
	if(parent->left_child)
		delete_tree(parent->left_child);
	if(parent->points)
		free(parent->points);
	free(parent);
}
