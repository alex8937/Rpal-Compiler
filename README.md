# Rpal-Compiler

The objectives of this project are:

(1) to implement a lexical analyzer, and a recursive descent parser for the RPAL language 

Lexical analysis is implemented as a Finite-State Automaton using its lexcial rules .

(2) Implement support module for trees, and stack of trees.

First-child, next-sibling binary tree representation of n-ary trees is used.
Preorder traversal of this tree is the same as for n-ary tree.
Popping n trees from the stack, and building a parent node on top of them, is easy.

(3) to implement the sub-tree (partial) standardizer, and the 13-rule CSE machine. 

