#ifndef EVAL_H
#define EVAL_H

typedef literal int;
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

//We got this baby here so that we can trace back up the
//tree from any node so that we can look at scoped bindings
//from parent binding nodes
  eval_node* parent_node;

} eval_node;

typedef literal (*operator_call)(literal left, literal right);

literal op_add(litaral left, literal right);
literal op_sub(litaral left, literal right);
literal op_mult(litaral left, literal right);
literal op_div(litaral left, literal right);
literal op_eq(litaral left, literal right);
literal op_gt(litaral left, literal right);
literal op_lt(litaral left, literal right);
literal op_not(litaral left, literal right);
literal op_and(litaral left, literal right);
literal op_or(litaral left, literal right);

#endif //EVAL_H
