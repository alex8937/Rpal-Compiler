# include <iostream>

# include <string>
# include <stdlib.h> 
# include <stdio.h>

//# include <sys/stat.h>
//# include <unistd.h>

# include "Lambda_element.h"
# include "Environment.h"
# include "AST_node.h"
# include "Token.h"
# include "Control_element.h"
# include "parser.h"

using std::cout; 

bool IF_DEBUG=false;

int main(int argc, char *argv[])
{
  bool IF_PARSE=false;
  // Loop through each argument and print its number and value
    
  if(argc<3){
    cout<<"Wrong format inputs."<<'\n';
    cout<<"Format: pl [-ast] [-debug] [dirctory]"<<'\n';
    exit(0);
  }
    
  for (int i=1; i < argc; ++i){
    if((string)argv[i]=="-ast") IF_PARSE=true;  
    else if((string)argv[i]=="-debug") IF_DEBUG=true;   
  }
    
/*  
  struct stat buffer;   
  if(stat (argv[argc-1], &buffer) != 0) {
        cout<<"Input Error: Rpal file not exist"<<'\n';
        exit(0);
    }
*/

  if(IF_PARSE) parser:parser((string)argv[argc-1]);
  
  return 0;
}








