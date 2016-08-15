# ifndef AST_NODE_H
# define AST_NODE_H


# include <string>
using std::string; 

struct AST_Node{
    AST_Node(string ct = "", AST_Node* ch = NULL, AST_Node* sb = NULL): content(ct), child(ch), sibling(sb)  {}
    AST_Node(AST_Node* Copy, AST_Node* sb = NULL) :content(Copy -> content), child(Copy -> child), sibling(sb){}
  // sb is usually not copied
  
    ~AST_Node(){}
    string content;
    AST_Node* child;
    AST_Node* sibling;
		bool cond_flag = false;
};

# endif