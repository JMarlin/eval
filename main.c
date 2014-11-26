#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "eval.h"

const cmd_count = 10;

//Need to make a header so that we can declare these functions
//prior to adding their addresses to the call table
const operator_call op_calls[] = {
	&op_add,
	&op_sub,
	&op_mult,
	&op_div,
	&op_eq,
	&op_gt,
	&op_lt,
	&op_not,
	&op_and,
	&op_or
}

const char* op_names[] = {
	"add",
	"sub",
	"mult",
	"div",
	"eq",
	"gt",
	"lt",
	"not",
	"and",
	"or"
}

const int op_inputs[] = {
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	1,
	2,
	2
}

eval_node* boundForest[];
int forestSize = 0;

token readT(FILE* sourceFile) {} //fuck i dunno

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
		//Important to note that literals are always leaves and tree building
		//will always terminate here.
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

	//Change this so that it just checks to see if it's not an operation or a literal so that we can do
	//binding in any order and then only worry about resolving references to bindings on evaluation
	//Because right now, you'll have to define things in order. Which to my taste might be better, really.
	//NOTE: As I started filling this out, it became apparent that we're going to have to know how many
	//arguments the reference takes in order to properly build the tree below it, and as such
	if(isBound(tNext) || inScope(tNext)) { //This works for either because references will be able to be references to local or global bindings.
		return newReference(tNext); //Does what it says on the tin. Critically, it names the referenced binding,
																//then looks up the binding in the current forest and gets its count of arguments
																//and uses this to build a node for each required entry
	}

	//This is my first instance of the error printer, so I want to note
	//that I want this to be able to take a string argument to shoehorn
	//into the message, for pretty obvious reasons.
	error("Unbound reference '%s'", tNext);
	return (eval_node*)0; //We pass a null node pointer back up the build
												//chain to indicate an erroneous halt condition

}

literal collapseTree(eval_node* root) {

	//First, dupe the provided tree

}

//NOTE: This might need to be rethought, because using this method of
//evaluating the left and right of an operation node and then feeding
//those values into a function precludes early solving, which is kind
//of one of the main tenets of this language, since, as mentioned,
//we're evaluating both sides before applying the operation when we
//should evaluate the left side and then return an early value if the
//left value causes such a condition. (e.g.: mult 0 x -> 0 every time)
literal op_add(litaral left, literal right) { return left + right; }
literal op_sub(litaral left, literal right) { return left - right; }
literal op_mult(litaral left, literal right) { return left * right; }
literal op_div(litaral left, literal right) { return left / right; }
literal op_eq(litaral left, literal right) { return left == right ? 1 : 0; }
literal op_gt(litaral left, literal right) { return left > right ? 1 : 0; }
literal op_lt(litaral left, literal right) { return left < right ? 1 : 0; }
literal op_not(litaral left, literal right) { return left == 0 ? 1 : 0; }
literal op_and(litaral left, literal right) { return left & right; }
literal op_or(litaral left, literal right) { return left | right; }

int main(int argc, char* argv[]) {

		show(eval(readT()));
		return 0;

}
