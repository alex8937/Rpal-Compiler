# include "Control_element.h"

string Control_element::get_content(){
  return content;
}

elementType Control_element::get_type(){
  return type;
}

Lambda_element* Control_element::get_lambda_ptr(){
	return lambda_ptr;
}

vector<Control_element*> Control_element::get_Control_ptr(){
	return *Control_ptr;
}


void Control_element::Initial_Lambda(AST_Node* Node, int Level){
  vector<string> variables;
  if(Node -> content != ","){
    variables.push_back(Node -> content);
  }
  else{
    auto it = Node  -> child;
    while(it -> sibling != NULL){
      variables.push_back(it -> content);
      it = it -> sibling;
    }
    variables.push_back(it -> content);
  }
  lambda_ptr = new Lambda_element(variables,Level);
  
}