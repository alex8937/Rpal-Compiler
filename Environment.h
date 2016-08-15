# ifndef ENVIRONMENT_H
# define ENVIRONMENT_H

# include <unordered_map>
# include "Lambda_element.h"
# include "Control_element.h"

using std::unordered_map; 

struct ENV{
	ENV* parent_ENV;
	int envir_Num;
	unordered_map <string, string> dict;
	unordered_map <string, Control_element*> dict_lambda;
  ENV(ENV* parent,int n):parent_ENV(parent),envir_Num(n){
		if(parent !=NULL ) {
			dict = parent -> dict;
			dict_lambda = parent -> dict_lambda;
		}
	}
};

#endif