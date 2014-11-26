#include <stdio.h>
#include <string.h>
#include <memory.h>

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

//Specific to function bindings
	token input_bindings[]; //The names of each input variable

} eval_node;

eval_node* boundForest[];
int forestSize = 0;

token readT(FILE* sourceFile) {} //fuck i dunno
treeCollection buildTrees(char* source) {} //set up trees for evaluatin' later on down the road.

//Process the equation given in the file handle and retrieve the
//calculated value in retVal
int parse(FILE* sourceFile, literal* retVal) {

	eval_node* newTree;
	token tNext

	while(1) {

		tNext = readT(sourceFile);
		if(!tNext)
				return 0;

		if(!(isLiteral(tNext) || isOperation(tNext) || isBound(tNext))) { //We'll let the tree builder take care of the invalid token case. Also, isBound will be for global binds, inScope will be for binds in the tree.

				//Make us a new bound tree
				newTree = (eval_node*)malloc(sizeof(eval_node));
				newTree->type = BINDING;

				//Copy the token into the new node's name
				newTree->name = (token)malloc(sizeof(unsigned char)*(strlen(tNext) + 1));
				strcpy(newTree->name, tNext);

				//Read the ensuing tokens until we error or read a "=" and get the list of tokens and their count
				newTree->input_bindings = collectParams(sourceFile, &(newTree->input_count)); //Collect params will do the check and error on file termination or invalid token prior to "="
				//Clean up and fail out if collectParams encountered an error
				if(!newTree->input_bindings) {
					free(newTree->name);
					free(newTree);
					return 0;
				}

				//Make room for a single child node entry (the top of the tree we're binding to)
				//and build a tree into it from the ensuing tokens
				//Clean up and fail out if building the tree fails
				newTree->child_nodes = (eval_node**)malloc(sizeof(eval_node*));
				newTree->child_nodes[0] = buildTree((token)0, sourceFile);
				if(!newTree->child_nodes[0]) {
					free(newTree->child_nodes);
					free(newTree->name);
					free(newTree);
					return 0;
				}

				//Our binding node is finished, insert it into the forest
				//Keep it secret
				//Keep it safe
				boundForest = (eval_node**)realloc(boundForest, sizeof(eval_node*)*(forestSize+1));
				if(!boundForest) {
					freeChildren(newTree->child_nodes); //Should replace all of this shit with freeTree
					free(newTree->child_nodes);
					free(newTree->name);
					free(newTree);
					return 0;
				}
				boundForest[forestSize++] = newTree;

		} else {

			//Build and evaluate the anonymous tree
			newTree = buildNode(tNext, FILE* source);
			if(!newTree)
				return 0;
			*retVal = collapseTree(newTree); //We never have to worry about collapseTree failing because we're guaranteed a valid tree after building it
			return 1;

		}
	}
}

//NOTE: buildTree just builds the logical tree as presented and does not give a single shit about
//whether the tokens following the end of the logical equation are valid. We will deal with that
//in the calling function.
eval_node* buildNode(token startToken, FILE* sourceFile) { //We pass startToken in case we have a calling function which needs to read the first token in a line before deciding to build a tree

	token tNext;
	eval_node thisNode;

	//Read the next token
	//Make sure we read the startToken first if passed
	if(startToken)
		tNext = startToken;
	else
		tNext = readT(sourceFile);

	if(isLiteral(tNext)) {
		//newLiteral parses the input token into a literal value,
		//then mallocs and inits a new literal node and gives it that value
		return newLiteral(tNext);
	}

	if(isOperation(tNext)) {
		//newOperation works the same way as newLiteral, but it not only sets the
		//name based on the operation but also allocates pointers in the inputs
		//array based on the number of args that operation takes and stores that
		//count as well
		thisNode = newOperation(tNext);

		//Op is guaranteed to at least have a right side
		thisNode->inputs[0] = buildNode((token)0, sourceFile);
		if(!thisNode->inputs[0])
			return (eval_node)0; //Should also call freeTree

		if(thisNode->input_count == 2) {
			thisNode->inputs[1] = buildNode((token)0, sourceFile);
			if(!thisNode->inputs[1])
				return (eval_node)0; //Should also call freeTree
		}

		return thisNode;

	}

	if(isBound(tNext)) {

	}

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
