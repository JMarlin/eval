typedef literal int;
typedef literal (*lo_op)(literal input);
typedef literal (*lr_op)(literal left, literal right);
typedef token char*;

typedef enum node_type {
	LITERAL,
	REFERENCE,
	BINDING,
	OPERATION
} node_type;

//Data common to all eval tree nodes
typedef struct eval_node {

//Common to all node types
	node_type type;

//Specific to literals
	literal value;

//For a reference node, this is the name of the target binding
//For a binding node, this is the bound name
//For an operation, this is the name to look up in the operation function table.
	token name;

//Specific to references, a

//First child node:
// 	The first input node to an operation
// 	The bound node when node is a binding
//	The first assigned child tree when node is a reference
//Second child node:
//	The second input node to a binary operation
//	The second assigned child tree when node is a reference
//Ensuing child nodes:
//	Any remaining child trees when node is a reference
	eval_node* child_nodes[];

//For operations, this can only be 1 or 2
//For functions, this specifies how many input bindings
//are taken by the function and can be arbitrary
	literal input_count;

//Specific to operations
	eval_node* right_node; //Not used by unary operations

//Specific to function bindings
	token input_bindings[]; //The names of each input variable


} eval_node;

token readT() {} //fuck i dunno
treeCollection buildTrees(char* source) {} //set up trees for evaluatin' later on down the road.

int parse(char* source) {
	//Read a token
	//Any literal, bound reference or anything else concrete:
			//Build an anonymous tree starting at the token just read
			//Collapse that tree
	//Any unbound token:
		//Read next token
			//Literal, operation or already bound symbol: Syntax error, fail
			//Unbound symbol: increase the input_count on the node-in-progress and add the token to the input_bindings[], return to read next token
			//"=": Break read token loop
		//Build a tree, plug it into the bound_node and return the binding node
}

treeNode buildTree(char* source) {
	//Read a token
	//Determine what type it is
		//Literal, build and return a literal node
		//Operation,
			//create an operation node with the given operation type
			//build a tree, plug it into the left side of the node
			//build a tree, plug it into the right side of the node
			//return the operation node
		//Something already bound at the top of the trees collection
			//Build a reference node (obviously, do not evaluate it but do look up the referenced binding to see how many parameters to skip) !!THIS ONE IS REAL TRICKY FOR FUNCTIONS!!
		//Something bound in a parent binding (go up the current tree and look at all of the input_bindings[]. If not matched, repeat process) (this should only happen once if we're building)
			//Create a binding node (remember that we're not evaluating yet, just building)
		//Unmatched, syntax error, fail

}

literal collapseTree(eval_node* root) {

	//First, dupe the provided tree

}

int main(int argc, char* argv[]) {

		show(eval(readT()));
		return 0;

}
