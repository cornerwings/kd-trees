

typedef struct{
	int* point_ids;
	int size;
}pts;

typedef struct node {
	struct node* left_child;
	struct node* right_child;
	struct node* parent;
	int dimension;
	pts* points;
} tree_node;

int select_dim( int* aux_global, int size, int D);
 void split(tree_node* parent, int* aux_global, int* flag_space, int* rerankspace, int size);
void print_tree (tree_node* parent);
tree_node* build_tree(int size);
void delete_tree(tree_node* parent);
