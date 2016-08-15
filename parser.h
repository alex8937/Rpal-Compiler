# include <fstream>
# include <stack>
# include <vector>

# include "Token.h"
# include "Environment.h"
# include "AST_node.h"
# include "Control_element.h"

using std::fstream;
using std::ifstream;
using std::stack;

class parser {

  private:
    token* next_token;
    ifstream FILE;
    int line=1;
    char ch_last=' ';
    stack <AST_Node*> AST_stack;
    vector<vector<Control_element*>> Control_structures;
    vector <Control_element*> CONTROL;
		vector <Control_element*> STACK;
		ENV* Current_env = new ENV(NULL, 0);
		vector<ENV*> ENV_list;
		int MAX_ENV_NUM = 0;
		
    token* scan();
  
    void proc_E();
    void proc_Ew();
    void proc_T();      
    void proc_Ta();     
    void proc_Tc();     
    void proc_B();      
    void proc_Bt();     
    void proc_Bs();
    void proc_Bp();
    void proc_A();      
    void proc_At();     
    void proc_Af();
    void proc_Ap();
    void proc_R();
    void proc_Rn();     
    void proc_D();      
    void proc_Da();     
    void proc_Dr();
    void proc_Db();     
    void proc_Vb();
    void proc_Vl(); 

    void print_AST(stack <AST_Node*>);
    void print_helper(AST_Node*,int);    
    void read(string);
    void read(tokenType);
    void build_tree(string,int,tokenType type=UNKNOWN);
  
    void standardize(AST_Node*); 
    void CS_builder(AST_Node*,int);
		void CSE_process();
		void CONTROL_STACK_Initializer();
		void CSE_rule1();
		void CSE_rule2();
		void CSE_rule3();
		void CSE_rule4();
		void CSE_rule5();
		void CSE_rule6();
		void CSE_rule7();
		void CSE_rule8();
		void CSE_rule9();
		void CSE_rule10();
		void CSE_rule11();
		void CSE_rule12();
		void CSE_rule13();
  public:

    parser(string);
        
};