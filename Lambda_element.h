# ifndef LAMBDA_ELEMENT_H
# define LAMBDA_ELEMENT_H

# include <string>
# include <vector>

using std::string; 
using std::vector; 

struct Lambda_element{
  int envir_Num = -1;
  int delta_Num = -1;
  vector<string> variables;
  Lambda_element(vector<string> vars, int del_N) : variables(vars), delta_Num(del_N) {}
};

#endif