# ifndef CONTROL_ELEMENT_H
# define CONTROL_ELEMENT_H

# include "AST_node.h"
# include "Lambda_element.h"

enum elementType {DELTA, LAMBDA, BETA, GAMMA, ID, INT, BINOP, UNOP, TAU, ENVIRONMENT, TUPLE, BOOL, Y, ETA};

class Control_element{
  private:  
    elementType type;
    string content="";
	  vector<Control_element*>* Control_ptr = nullptr;
    Lambda_element* lambda_ptr = nullptr; 
  public:
	  Control_element(elementType ty, string ct = "",vector<Control_element*>* ptr = nullptr):\
										type(ty), content(ct), Control_ptr(ptr) {}
		Control_element(Lambda_element* ptr, elementType ty = ETA, string ct ="eta"):\
										type(ty), content(ct), lambda_ptr(ptr) {}
    void Initial_Lambda(AST_Node*, int);
    string get_content();
		elementType get_type();
		Lambda_element* get_lambda_ptr();
		vector<Control_element*> get_Control_ptr();
    ~Control_element(){}    
};

#endif