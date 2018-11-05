# Rpal-Compiler

The objectives of this project are:
- To implement a lexical analyzer, and a recursive descent parser for the RPAL language 

Lexical analysis is implemented as a Finite-State Automaton using its lexcial rules .

Get Token: string, numeric, comment, operator, punctuation or reserved word;


- Implement support module for trees, and stack of trees.

First-child, next-sibling binary tree representation of n-ary trees is used.
Preorder traversal of this tree is the same as for n-ary tree.
Popping n trees from the stack, and building a parent node on top of them, is easy.

Parser: build First-child, next-sibling binary tree;

 - To implement the sub-tree (partial) standardizer, and the 13-rule CSE machine. 

Standardizer: Generatre binary standardized tree: either lambda (definition) or gamma (application) expresion

e.g.

let x = 3 in let y = 4 in x + y;

(x + y where x =3) where y = 4;

become same lambda expression

CSE (Control stack environment)

A mapping from names to objects and operations

Tree -> Flattened control structure -> CSE machine

We use a single symbol to represent a lambda - expression, both on the control, and on the stack.  The symbol is <i lambda k x>.

i: environment,

k: CS of the function's body,

x: the function's bound variable

Environment structure is a tree: Every environment is linked to a previously created (but not necessarily currently active) environment
