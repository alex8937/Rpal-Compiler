# include <iostream>
# include <cmath>
# include "parser.h"
# include "inline_fun.h"

using std::cout; 
using std::to_string;

extern bool IF_DEBUG;

vector<string> reserved_words = {"let","where","in","fn","and","rec","within","aug","not","or","gr","ge","ls","le","eq","ne","true","false","nil","dummy"};

vector<string> split_string(string str, char delimiter = ','){
	vector<string> nums;
	string num;
	int j = 0;
	for(int i = 0; i < str.size(); ++i){
		if(str[i] == '(') j++;
		if(str[i] == ')') j--;
		if(j > 0){
			num + =  str[i];
		}
		else{
			if(str[i] ! =  delimiter){
				num + =  str[i];
			}
			else{
				nums.push_back(num);
				num.clear();
			}
		}
	}
	nums.push_back(num);
	return nums;
}

parser::parser(string FILE_NAME){   
    FILE.open(FILE_NAME.c_str(),ifstream::in);
    next_token = scan();
    while(next_token -> get_type() == DELETE) {
      delete next_token;
      next_token = NULL;
      next_token = scan();      
    }
    proc_E();
    print_AST(AST_stack);
  cout << " == == == == == == == == == == == ==  " << '\n';
  standardize(AST_stack.top());
  
  print_AST(AST_stack);
  cout << " == == == == == == == == == == == ==  " << '\n';
  vector<Control_element*> new_Control_structure;
  Control_structures.push_back(new_Control_structure);
  ENV_list.push_back(Current_env);
  CS_builder(AST_stack.top(),0);

  if(IF_DEBUG){
    for (int i = 0; i<Control_structures.size(); ++i){
			cout << "DEL[" << i << "]: ";
      for (auto it : Control_structures[i]){
        cout << it -> get_content() << ' ';
      }
      cout << '\n';
    }    
  }
	
	CSE_process();
  exit(0);
}

token* parser::scan(){
    if(IF_DEBUG) cout << "At line " << line << '\n';
    char ch;
    string token_value;
    token *temp = NULL;
    if(ch_last == ' '){    
        FILE.get(ch);
    }
    else{
        ch = ch_last;
    }
    //if(IF_DEBUG) cout << "This character is scanned: " << "*" << ch << "*" << '\n';
    if(ch == '\n') {
        line++;
        if(IF_DEBUG) cout << "Come to line " << line << '\n';
    }
        
    if(FILE.eof()){
        temp = new token("",END_OF_FILE);
        if(IF_DEBUG) cout << "Read until the end of file" << '\n';
        return temp;
    }
    
    if(ch == '/'){
        token_value.push_back(ch);
        FILE.get(ch);
        if(ch == '/'){
            token_value.push_back(ch);
            if(IF_DEBUG) cout << "Here begins comment." << '\n';
            while(ch! = '\n') {
        if(FILE.eof()) break;
                FILE.get(ch);
                token_value.push_back(ch);
            }   
            if(IF_DEBUG) cout << token_value << '\n';           
            temp = new token(token_value,DELETE);         
            FILE.get(ch);
            
        }
        else {
            if(IF_DEBUG) cout << "Here begins operator." << '\n';
            while(IS_OPERATOR_SYMBOL(ch)){
        if(FILE.eof()) break;
                token_value.push_back(ch);
                FILE.get(ch);
            }
            if(IF_DEBUG) cout << token_value << '\n';       
            temp = new token(token_value,OPERATOR);   
        }
    }
    
    else if(ch == '\''){
        if(IF_DEBUG) cout << "Here begins string." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        while(ch! = '\''){
      if(FILE.eof()) break;
            token_value.push_back(ch);
            FILE.get(ch);               
            if(ch == '\n'){
                cout << "Unable parse near the following:" << '\n';
                cout << "Line " << line-1 << ": " << token_value;
                exit(0);
            }
        }
        token_value.push_back(ch);
        if(IF_DEBUG) cout << token_value << '\n';
        temp = new token(token_value,STRING);
        FILE.get(ch);
    }
    
    else if(isspace(ch)){
        if(IF_DEBUG) cout << "Here begins space." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        while(isspace(ch)){
      if(FILE.eof()) break;
            token_value.push_back(ch);
            FILE.get(ch);
        }
        if(IF_DEBUG) cout << token_value << '\n';
        temp = new token(token_value,DELETE);
        
    }
    else if(IS_DIGIT(ch)){
        if(IF_DEBUG) cout << "Here begins integer." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        while(IS_DIGIT(ch)){
      if(FILE.eof()) break;
            token_value.push_back(ch);
            FILE.get(ch);
        }
        if(IF_DEBUG) cout << token_value << '\n';       
        temp = new token(token_value,INTEGER);    
    }
    else if(IS_LETTER(ch)){
        if(IF_DEBUG) cout << "Here begins identifier." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        while(IS_DIGIT(ch)||IS_LETTER(ch)||ch == '_'){
      if(FILE.eof()) break;
            token_value.push_back(ch);
            FILE.get(ch);
        }
        if(IF_DEBUG) cout << token_value << '\n';
        temp = new token(token_value,IDENTIFIER); 
    }
    else if(IS_OPERATOR_SYMBOL(ch)){
        if(IF_DEBUG) cout << "Here begins operator." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        while(IS_OPERATOR_SYMBOL(ch) && ch ! =  '-'){
					  if(FILE.eof()) break;
            token_value.push_back(ch);
            FILE.get(ch);
        }
        if(IF_DEBUG) cout << token_value << '\n';       
        temp = new token(token_value,OPERATOR);   
    }
    else if(IS_PUNCTION(ch)){
        if(IF_DEBUG) cout << "Here begins punctuation." << '\n';
        token_value.push_back(ch);
        FILE.get(ch);
        if(IF_DEBUG) cout << token_value << '\n';       
        temp = new token(token_value,PUNCTUATION);            
    }
    ch_last = ch;
    for(int i = 0;i<reserved_words.size();++i){
        if(token_value == reserved_words[i]) {
            if(IF_DEBUG) cout << token_value << " is a RESERVED_WORD." << '\n';
            temp = new token(token_value,RESERVED_WORD);
            break;
        }
    }
    //if(IF_DEBUG) cout << "Read token in scanning function *" << temp -> get_value() << "*" << '\n';
    //if(IF_DEBUG) cout << "Type is " << temp -> get_type() << '\n';  
    
    return temp;
}

void parser::proc_E(){
    //if(IF_DEBUG) cout << "Processing E: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_value() == "let"){
        read("let");
        proc_D();
        read("in");
        proc_E();
        build_tree("let",2);
    }
    else if(next_token -> get_value() == "fn"){
        read("fn");
        int N = 0;
        while(next_token -> get_type() == IDENTIFIER||next_token -> get_value() == "("){
            proc_Vb();
            N++;
        }
        read(".");
        proc_E();
        build_tree("lambda",N+1);
    }
    else{
        proc_Ew();
    }
    //if(IF_DEBUG) cout << "Finishing E: " << '\n';
}

void parser::proc_Ew(){  //common prefix
    //if(IF_DEBUG) cout << "Processing Ew: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_T();
    if(next_token -> get_value() == "where"){
        read("where");
        proc_Dr();
        build_tree("where",2);
    }
    //if(IF_DEBUG) cout << "Finishing Ew: " << '\n';
}

void parser::proc_T(){  //common prefix
    //if(IF_DEBUG) cout << "Processing T: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Ta();
    int N = 0;
    while(next_token -> get_value() == ","){
        read(",");
        proc_Ta();
        N++;
    }
    if(N>0) build_tree("tau",N+1);
    //if(IF_DEBUG) cout << "Finishing T: " << '\n';
}

void parser::proc_Ta(){  //left recurrsion
    //if(IF_DEBUG) cout << "Processing Ta: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Tc();
    while(next_token -> get_value() == "aug"){
        read("aug");
        proc_Tc();
        build_tree("aug",2);
    }
    //if(IF_DEBUG) cout << "Finishing Ta: " << '\n';
}

void parser::proc_Tc(){  //common prefix
    //if(IF_DEBUG) cout << "Processing Tc: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_B();
    if(next_token -> get_value() == "->"){
        read("->");
        proc_Tc();
        read("|");
        proc_Tc();
        build_tree("->",3);
    }
    //if(IF_DEBUG) cout << "Finishing Tc: " << '\n';
}

void parser::proc_B(){  //common prefix
    //if(IF_DEBUG) cout << "Processing B: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Bt();
    if(next_token -> get_value() == "or"){
        read("or");
        proc_Bt();
        build_tree("or",2);
    }
    //if(IF_DEBUG) cout << "Finishing B: " << '\n';
}

void parser::proc_Bt(){  //left recurrsion
    //if(IF_DEBUG) cout << "Processing Bt: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Bs();
    while(next_token -> get_value() == "&"){
        read("&");
        proc_Bs();
        build_tree("&",2);
    }
    //if(IF_DEBUG) cout << "Finishing Bt: " << '\n';
}

void parser::proc_Bs(){
    //if(IF_DEBUG) cout << "Processing Bs: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_value() == "not"){
        read("not");
        proc_Bp();
        build_tree("not",1);
    }
    else{
        proc_Bp();
    }
    //if(IF_DEBUG) cout << "Finishing Bs: " << '\n';
}

void parser::proc_Bp(){  //common prefix
    //if(IF_DEBUG) cout << "Processing Bp: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_A();
    if(next_token -> get_value() == "gr"){
        read("gr");
        proc_A();
        build_tree("gr",2);
    }
    else if(next_token -> get_value() == ">"){
        read(">");
        proc_A();
        build_tree("gr",2);
    }
    else if(next_token -> get_value() == "ge"){
        read("ge");
        proc_A();
        build_tree("ge",2);
    }
    else if(next_token -> get_value() == ">="){
        read(">=");
        proc_A();
        build_tree("ge",2);
    }
    else if(next_token -> get_value() == "ls"){
        read("ls");
        proc_A();
        build_tree("ls",2);
    }
    else if(next_token -> get_value() == "<"){
        read("<");
        proc_A();
        build_tree("ls",2);
    }
    else if(next_token -> get_value() == "le"){
        read("le");
        proc_A();
        build_tree("le",2);
    }
    else if(next_token -> get_value() == "<="){
        read("<=");
        proc_A();
        build_tree("ls",2);
    }
    else if(next_token -> get_value() == "eq"){
        read("eq");
        proc_A();
        build_tree("eq",2);
    }
    else if(next_token -> get_value() == "ne"){
        read("ne");
        proc_A();
        build_tree("ne",2);
    }
    //if(IF_DEBUG) cout << "Finishing Bp: " << '\n';
}

void parser::proc_A(){  //left recurrsion + common prefix
    //if(IF_DEBUG) cout << "Processing A: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_value() == "+"){
        read("+");
        proc_At();
    }
    else if(next_token -> get_value() == "-"){
        read("-");
        proc_At();
        build_tree("neg",1);
    }
    else{
        proc_At();
    }
    while(next_token -> get_value() == "+" || next_token -> get_value() == "-"){
        if(next_token -> get_value() == "+" ) {
            read("+");
            proc_At();
            build_tree("+",2);
        }
        else if(next_token -> get_value() == "-"){
            read("-");
            proc_At();
            build_tree("-",2);          
        }
    }
    //if(IF_DEBUG) cout << "Finishing A: " << '\n';
}

void parser::proc_At(){  //left recurrsion + common prefix
    //if(IF_DEBUG) cout << "Processing At: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Af();
    while(next_token -> get_value() == "*"||next_token -> get_value() == "/"){
        if(next_token -> get_value() == "*"){
            read("*");
            proc_Af();
            build_tree("*",2);
        }
        else if(next_token -> get_value() == "/"){
            read("/");
            proc_Af();
            build_tree("/",2);
        }
    }
    //if(IF_DEBUG) cout << "Finishing At: " << '\n';
}

void parser::proc_Af(){  //common prefix
    //if(IF_DEBUG) cout << "Processing Af: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Ap();
    if(next_token -> get_value() == "**"){
        read("**");
        proc_Af();
        build_tree("**",2);
    }
    //if(IF_DEBUG) cout << "Finishing Af: " << '\n';
}

void parser::proc_Ap(){  //left recurrsion
    //if(IF_DEBUG) cout << "Processing Ap: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_R();
    while(next_token -> get_value() == "@"){
        read("@");
        read(IDENTIFIER);
        proc_R();
        build_tree("@",3);
    }
    //if(IF_DEBUG) cout << "Finishing Ap: " << '\n';
}

void parser::proc_R(){ //left recurrsion
    //if(IF_DEBUG) cout << "Processing R: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Rn();
    while(next_token -> get_type() == IDENTIFIER || next_token -> get_type() == INTEGER  ||
          next_token -> get_type() == STRING   || next_token -> get_value() == "true"  ||
          next_token -> get_value() == "false"   || next_token -> get_value() == "nil"   ||
          next_token -> get_value() == "("       || next_token -> get_value() == "dummy" ){
        proc_Rn();
        build_tree("gamma",2);
    }
    //if(IF_DEBUG) cout << "Finishing R: " << '\n';
}

void parser::proc_Rn(){ 
    //if(IF_DEBUG) cout << "Processing Rn: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_type() == IDENTIFIER){
        read(IDENTIFIER);
    }
    else if(next_token -> get_type() == INTEGER){
        read(INTEGER);
    }
    else if(next_token -> get_type() == STRING){
        read(STRING);
    }
    else if(next_token -> get_value() == "true"){
        read("true");
        build_tree("true",0);
    }
    else if(next_token -> get_value() == "false"){
        read("false");
        build_tree("false",0);
    }
    else if(next_token -> get_value() == "nil"){
        read("nil");
        build_tree("nil",0);
    }
    else if(next_token -> get_value() == "("){
        read("(");
        proc_E();
        read(")");
    }
    else{
        read("dummy");
        build_tree("dummy",0);
    }
    //if(IF_DEBUG) cout << "Finishing Rn: " << '\n';
}

void parser::proc_D(){  //common prefix
    //if(IF_DEBUG) cout << "Processing D: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Da();
    if(next_token -> get_value() == "within"){
        read("within");
        proc_D();
        build_tree("within",2);
    }
    //if(IF_DEBUG) cout << "Finishing D: " << '\n';
}

void parser::proc_Da(){  //common prefix
    //if(IF_DEBUG) cout << "Processing Da: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    proc_Dr();
    int N = 0;
    while(next_token -> get_value() == "and"){
        read("and");    
        proc_Dr();
        N++;
    }
    if(N>0) build_tree("and",N+1);
    //if(IF_DEBUG) cout << "Finishing Da: " << '\n';
}

void parser::proc_Dr(){ 
    //if(IF_DEBUG) cout << "Processing Dr: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_value() == "rec"){
        read("rec");    
        proc_Db();
        build_tree("rec",1);
    }
    else{
        proc_Db();
    }
    //if(IF_DEBUG) cout << "Finishing Dr: " << '\n';
}

void parser::proc_Db(){  // common prefix in rule 1 and rule 2
    //if(IF_DEBUG) cout << "Processing Db: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_type() == IDENTIFIER){
        read(IDENTIFIER);
        if(next_token -> get_value() == ","){
            proc_Vl();
            read("=");
            proc_E();
            build_tree("=",2);
        }
        else if(next_token -> get_value() == "="){
            read("=");
            proc_E();
            build_tree("=",2);                  
        }
        else{
            int N = 0;
            while(next_token -> get_type() == IDENTIFIER || next_token -> get_value() == "("){
                proc_Vb();
                N++;
            }   
            read("=");
            proc_E();
            build_tree("function_form",N+2);
            
        }
        
    }
    else if(next_token -> get_value() == "("){
        read("(");
        proc_D();
        read(")");
    }
    //if(IF_DEBUG) cout << "Finishing Db: " << '\n';
}

void parser::proc_Vb(){  // common prefix
    //if(IF_DEBUG) cout << "Processing Vb: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';
    if(next_token -> get_type() == IDENTIFIER){
        read(IDENTIFIER);
    }
    else if(next_token -> get_value() == "(") {
        read("(");
        if(next_token -> get_type() == IDENTIFIER) {
            read(IDENTIFIER);
            int N = 0;
            while(next_token -> get_value() == ","){
                read(",");
                read(IDENTIFIER);
                N++;            
            }
            if(N>0) build_tree(",",N+1);
        }
        read(")");
    }
    //if(IF_DEBUG) cout << "Finishing Vb: " << '\n';
}

void parser::proc_Vl(){ 
    //if(IF_DEBUG) cout << "Processing Vl: " << next_token -> get_value() << "/" << next_token -> get_type() << '\n';

        //read(IDENTIFIER); Done in proc_Db
    int N = 0;
    while(next_token -> get_value() == ","){
        read(",");
        read(IDENTIFIER);
        N++;            
    }
    if(N>0) build_tree(",",N+1);

    //if(IF_DEBUG) cout << "Finishing Vl: " << '\n';
}

void parser::read(string value){
    bool IF_RESERVED = false;
    if(IF_DEBUG) cout << "Read token value as " << value << '\n';
    if(next_token -> get_value()! = value){
        cout << "Expected " << value << " but found " << next_token -> get_value() << '\n';
        cout << "Line " << line << '\n';
        exit(0);
    }
    if(next_token -> get_type() == IDENTIFIER||next_token -> get_type() == INTEGER||next_token -> get_type() == STRING){
        for(int i = 0;i<reserved_words.size();++i){
            if(next_token -> get_value() == reserved_words[i]) {
                if(IF_DEBUG) cout << next_token -> get_value() << " is a RESERVED_WORD." << '\n';
                IF_RESERVED = true;
                break;
            }
        }
        if(!IF_RESERVED) {
            build_tree(next_token -> get_value(),0,next_token -> get_type());
            if(IF_DEBUG) cout << "Finish build the tree node." << '\n';
        }
        
    }

    delete next_token;
    next_token = NULL;
    next_token = scan();  

    while(next_token -> get_type() == DELETE) {
        delete next_token;
        next_token = NULL;
        next_token = scan();      
    }

    if(IF_DEBUG) cout << "Next token value as " << next_token -> get_value() << '\n';
    
}

void parser::read(tokenType type){
    bool IF_RESERVED = false;
    if(IF_DEBUG) cout << "Read token value as " << next_token -> get_value() << '\n';
    if(next_token -> get_type()! = type){
        cout << "Expected " << type << " but found " << next_token -> get_type() << '\n';
        cout << "Line " << line;
        exit(0);
    }
    if(next_token -> get_type() == IDENTIFIER||next_token -> get_type() == INTEGER||next_token -> get_type() == STRING){
        for(int i = 0;i<reserved_words.size();++i){
            if(next_token -> get_value() == reserved_words[i]) {
                if(IF_DEBUG) cout << next_token -> get_value() << " is a RESERVED_WORD." << '\n';
                IF_RESERVED = true;
                break;
            }
        }
        if(!IF_RESERVED) {
            build_tree(next_token -> get_value(),0,next_token -> get_type());
            if(IF_DEBUG) cout << "Finish build the tree node." << '\n';
        }

    }

    delete next_token;
    next_token = NULL;
    next_token = scan();


    while(next_token -> get_type() == DELETE) {

        delete next_token;
        next_token = NULL;
        next_token = scan();      
    }

    if(IF_DEBUG) cout << "Next token value as " << next_token -> get_value() << '\n';
}

void parser::build_tree(string value,int child_num, tokenType type){
    if(IF_DEBUG) 
        cout << "Build a AST_Node: value: " << value << " with type: " << type << " having " << child_num << " children" << '\n';
    if(IF_DEBUG) cout << "Before this, AST_stack has a size of " << AST_stack.size() << '\n';
    AST_Node* new_node = new AST_Node;
    if(child_num == 0){
        if(type == INTEGER){
            new_node -> content = "<INT:"+value+">";
            AST_stack.push(new_node);
        }
        else if(type == IDENTIFIER){
            new_node -> content = "<ID:"+value+">";
            AST_stack.push(new_node);
        }
        else if(type == STRING){
            new_node -> content = "<STR:"+value+">";
            AST_stack.push(new_node);
        }
        else{
            new_node -> content = "value";
            AST_stack.push(new_node);       
        }
    }
    else{
        new_node -> content = value;
    
        for(int i = 1;i< = child_num-1;++i){
            AST_Node* temp = new AST_Node;
            temp = AST_stack.top();
            AST_stack.pop();
            AST_stack.top() -> sibling = temp;
            
        }
        AST_Node* temp = new AST_Node;
        temp = AST_stack.top();
        AST_stack.pop();
        new_node -> child = temp;
        AST_stack.push(new_node);
    }
    if(IF_DEBUG) 
        cout << "After this, AST_stack has a size of " << AST_stack.size() << '\n';
    if(IF_DEBUG) 
        cout << " ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==" << '\n';
}

void parser::print_helper(AST_Node* Node,int level){
    for(int i = level;i>0;--i) cout << ".";
    cout << Node -> content;
    if(IF_DEBUG) cout << " at level " << level;
    cout << '\n';
    if(Node -> child! = NULL){
        print_helper(Node -> child,level+1);
    }
    if(Node -> sibling! = NULL){
        print_helper(Node -> sibling,level);
    }
}

void parser::print_AST(stack <AST_Node*> AST_stack){
    if(IF_DEBUG){
        if(AST_stack.size() == 1){
            cout << "Succeed to shrink AST_stack to size 1" << '\n';
        }
        else{
            cout << "Fail to shink AST_stack to size 1" << '\n';
            cout << "Current size of stack is " << AST_stack.size() << '\n';
            exit(0);
        }
    }
    print_helper(AST_stack.top(),0);
}

void parser::standardize(AST_Node* Node){
  if(Node -> child! = NULL){
    standardize(Node -> child);
  }
  if(Node -> sibling! = NULL){
    standardize(Node -> sibling);
  }
  if( Node -> content == "let" && Node -> child -> content == "=" ){   
    AST_Node* E = new AST_Node(Node -> child -> child -> sibling);
    AST_Node* P = new AST_Node(Node -> child -> sibling);
    AST_Node* X = new AST_Node(Node -> child -> child,P);
    Node -> content = "gamma";
    Node -> child = new AST_Node("lambda", X, E);
  }
  else if( Node -> content == "where" && Node -> child -> sibling -> content == "=" ){
    AST_Node* P = new AST_Node(Node -> child);
    AST_Node* X = new AST_Node(Node -> child -> sibling -> child, P);
    AST_Node* E = new AST_Node(Node -> child -> sibling -> child -> sibling);
    Node -> content = "gamma";
    Node -> child = new AST_Node("lambda", X, E);   
  }
  else if(Node -> content == "and" && Node -> child -> content == "="){
    AST_Node* it = Node -> child;
    vector<AST_Node*> X,E;
    while (it){
      X.push_back(new AST_Node(it -> child));
      E.push_back(new AST_Node(it -> child -> sibling));
      it = it -> sibling;
    }
    for(int i = X.size()-2; i> = 0; --i){
      X[i] -> sibling = X[i+1];
      E[i] -> sibling = E[i+1];
    }
    Node -> content = "=";   
    Node -> child = new AST_Node(",", X[0], new AST_Node("tau", E[0]));    
  }
  else if(Node -> content  == "@"){
    AST_Node* E1 = new AST_Node(Node -> child);
    AST_Node* N = new AST_Node(Node -> child -> sibling, E1);
    AST_Node* E2 = new AST_Node(Node -> child -> sibling -> sibling);
    Node -> content = "gamma";
    Node -> child = new AST_Node("gamma", N, E2);
  }
  else if(Node -> content == "within" && Node -> child -> content == "=" && Node -> child -> sibling -> content == "="){
    AST_Node* E1 = new AST_Node(Node -> child -> child -> sibling);
    AST_Node* E2 = new AST_Node(Node -> child -> sibling -> child -> sibling); 
    AST_Node* X1 = new AST_Node(Node -> child -> child, E2);
    AST_Node* X2 = new AST_Node(Node -> child -> sibling -> child);
    Node -> content = "=";
    Node -> child = X2;
    X2 -> sibling = new AST_Node("gamma", new AST_Node("lambda", X1, E1) );
  }
  else if(Node -> content == "function_form"){
    AST_Node* P = new AST_Node(Node -> child);    
    vector<AST_Node*> V;
    AST_Node* it = Node -> child -> sibling;
    while(it -> sibling ! =  NULL){
      AST_Node* temp = new AST_Node(it);
      V.push_back(temp);
      it = it -> sibling;
    }
    AST_Node* E = new AST_Node(it);
    Node -> content = "=";  
    Node -> child = P;
        
    for(int i = 0; i<V.size(); ++i){
      if(i == 0) {
        P -> sibling = new AST_Node("lambda", V[i]);
        it = P -> sibling -> child;
      }
      else {
        it -> sibling = new AST_Node("lambda", V[i]);
        it = it -> sibling -> child;
      }
      it -> sibling = E;
    }
  }
  else if(Node -> content == "rec" && Node -> child -> content == "="){
    AST_Node* E = new AST_Node(Node -> child -> child -> sibling);
    AST_Node* X1 = new AST_Node(Node -> child -> child);
    AST_Node* X2 = new AST_Node(X1, E);
    Node -> content = "=";
    new AST_Node("gamma", new AST_Node("Y", new AST_Node("lambda", X2)));
    Node -> child = new AST_Node(X1, new AST_Node("gamma", new AST_Node("Y", new AST_Node("lambda", X2))));
  }
	else if(Node -> content == "lambda" && Node -> child -> sibling -> sibling ! =  NULL){
		AST_Node* new_lambda = new AST_Node("lambda", new AST_Node(Node -> child -> sibling, Node -> child -> sibling -> sibling));
		AST_Node* V1 = new AST_Node(Node -> child, new_lambda);
		Node -> child = V1;		
	}
  
}

void parser::CS_builder(AST_Node* Node,int level){
  if(IF_DEBUG) cout <<  Node -> content << " at Del[" <<  level << "]"  << '\n';
  if(Node -> content == "gamma"){
    Control_element* CE = new Control_element(GAMMA, "gamma");
    Control_structures[level].push_back(CE);
  }
  else if(Node -> content == "lambda"){
    Control_element* CE = new Control_element(LAMBDA, "lambda");
		int max_level = Control_structures.size() - 1;
    CE -> Initial_Lambda(Node -> child, max_level + 1);
    Control_structures[level].push_back(CE);
    vector<Control_element*> new_Control_structure;
    Control_structures.push_back(new_Control_structure);
		
    CS_builder(Node -> child -> sibling, max_level + 1);
    Node -> child = NULL;
  }
  else if(IS_ID_TYPE(Node -> content)){
    Control_element* CE = new Control_element(ID, Node -> content);
    Control_structures[level].push_back(CE);
  }
  else if(IS_UNOP_TYPE(Node -> content)){
    Control_element* CE = new Control_element(UNOP, Node -> content);
    Control_structures[level].push_back(CE);
  }
  else if(IS_BINOP_TYPE(Node -> content)){
    Control_element* CE = new Control_element(BINOP, Node -> content);
    Control_structures[level].push_back(CE);
  }
  else if(IS_INT_TYPE(Node -> content)){
    Control_element* CE = new Control_element(INT, Remove_Bracket(Node -> content));
    Control_structures[level].push_back(CE);
  }
	else if(Node -> content == "->"){
		Control_element* CE = new Control_element(BETA, "beta");
		vector<Control_element*> new_Control_structure;
    Control_structures.push_back(new_Control_structure);	
		Control_structures.push_back(new_Control_structure);
		Node -> child -> sibling -> cond_flag = true;
		Node -> child -> sibling -> sibling -> cond_flag = true;
		CS_builder(Node -> child -> sibling, level+1);			
		CS_builder(Node -> child -> sibling -> sibling, level+2);
		
		Control_element* CE_then = new Control_element(DELTA, "then",&Control_structures[level+1]);

	  Control_element* CE_else = new Control_element(DELTA, "else",&Control_structures[level+2]);

		Control_structures[level].push_back(CE_then);
		Control_structures[level].push_back(CE_else);
		Control_structures[level].push_back(CE);
	}
	else if(Node -> content == "tau"){
		int num_under_tau = 1;
		auto tuple_node = Node -> child;
		while(tuple_node -> sibling ! =  NULL){
			tuple_node = tuple_node -> sibling;
			num_under_tau++;
		}
		Control_element* CE = new Control_element(TAU, to_string(num_under_tau));
		Control_structures[level].push_back(CE);
	}
	else if(Node -> content == "Y"){
    Control_element* CE = new Control_element(Y, "Y");
    Control_structures[level].push_back(CE);		
	}
	else{
		cout << "Exception in Control structures builder;" << '\n';
		exit(0);
	}
  if(Node -> child! = NULL){
    CS_builder(Node -> child, level);
  }
  if(Node -> sibling! = NULL && Node -> sibling -> cond_flag == false){
    CS_builder(Node -> sibling, level);
  }  
}

void parser::CONTROL_STACK_Initializer(){
	Control_element* CE = new Control_element(ENVIRONMENT, "e0");
	CONTROL.push_back(CE);
	for (auto obj : Control_structures[0]){
		CONTROL.push_back(obj);
	}
	STACK.push_back(CE);
	
}

void parser::CSE_process(){
	CONTROL_STACK_Initializer();
	int line = 0;
	while(!CONTROL.empty()){		
		if(IF_DEBUG){
			cout << '\n' << ">>>>>>>Line " << ++line << '\n';
			cout << "CONTROL: ";
			for (auto it : CONTROL){
				cout << it -> get_content() << ",";
			}
			cout << '\n' << "STACK: ";
			for (auto it : STACK){
				cout << it -> get_content() << ",";
			}
			cout << '\n';
		}
		if(CONTROL.back() -> get_type() == INT || CONTROL.back() -> get_type() == ID  || CONTROL.back() -> get_type() == Y) {
			if(IF_DEBUG) cout << "Use CSE_rule1: " << '\n' ;
			CSE_rule1();
		}
		else if(CONTROL.back() -> get_type() == LAMBDA){
			if(IF_DEBUG) cout << "Use CSE_rule2: " << '\n' ;
			CSE_rule2();
		}
		else if(CONTROL.back() -> get_type() == GAMMA && STACK.back() -> get_type() == LAMBDA){
			if(IF_DEBUG) cout << "Use CSE_rule4: " << '\n' ;
			CSE_rule4();
		}
		else if(CONTROL.back() -> get_type() == ENVIRONMENT){
			if(IF_DEBUG) cout << "Use CSE_rule5: " << '\n' ;
			CSE_rule5();
		}
		else if(CONTROL.back() -> get_type() == BINOP){
			if(IF_DEBUG) cout << "Use CSE_rule6: " << '\n' ;
			CSE_rule6();
		} 
		else if(CONTROL.back() -> get_type() == UNOP){
			if(IF_DEBUG) cout << "Use CSE_rule7: " << '\n' ;
			CSE_rule7();
		} 
		else if(CONTROL.back() -> get_type() == BETA){
			if(IF_DEBUG) cout << "Use CSE_rule8: " << '\n' ;
			CSE_rule8();
		} 
		else if(CONTROL.back() -> get_type() == TAU){
			if(IF_DEBUG) cout << "Use CSE_rule9: " << '\n' ;
			CSE_rule9();
		}
		else if(CONTROL.back() -> get_type() == GAMMA && STACK.back() -> get_type() == TUPLE){
			if(IF_DEBUG) cout << "Use CSE_rule10: " << '\n' ;
			CSE_rule10();			
		}
		else if(CONTROL.back() -> get_type() == GAMMA && STACK.back() -> get_type() == Y){
			if(IF_DEBUG) cout << "Use CSE_rule12: " << '\n' ;
			CSE_rule12();				
		}
		else if(CONTROL.back() -> get_type() == GAMMA && STACK.back() -> get_content() == "eta"){
			if(IF_DEBUG) cout << "Use CSE_rule13: " << '\n' ;
			CSE_rule13();				
		}
		else{
			cout << "NO rule to process this CONTROL STACK" << '\n' ;
			cout << "On stack: " << STACK.back() -> get_type() << '\n';
			cout << "On control: " << CONTROL.back() -> get_type() << '\n';
			exit(0);
		}
	}
	cout << STACK.back() -> get_content() ;
}

void parser::CSE_rule1(){
	if(CONTROL.back() -> get_type() == INT){
		STACK.push_back(CONTROL.back());
		CONTROL.pop_back();
	}
	else if(CONTROL.back() -> get_type() == ID && \
					CONTROL.back() -> get_content() == "<ID:Print>" &&\
					CONTROL[CONTROL.size()-2] -> get_type() == GAMMA){
		CONTROL.pop_back();
		CONTROL.pop_back();
	}
	else if(STACK.back() -> get_type() == TUPLE && \
					CONTROL.back() -> get_content() == "<ID:Order>" &&\
					CONTROL[CONTROL.size()-2] -> get_type() == GAMMA){

		CONTROL.pop_back();
		CONTROL.pop_back();
		string tuple_string = STACK.back() -> get_content	();
		tuple_string.erase(tuple_string.begin());
		tuple_string.erase(tuple_string.size()-1);
		int order = split_string(tuple_string,',').size();
		Control_element* CE = new Control_element(INT, to_string(order));
		STACK.push_back(CE);		
	}
	else if(CONTROL.back() -> get_type() == ID ){
		
		string Name = CONTROL.back() -> get_content();
		if(IF_DEBUG){
			cout << "In ENV" << Current_env -> envir_Num << ", " << Name << " is " ;
			if(Current_env -> dict.count(Name) == 0 && Current_env -> dict_lambda.count(Name) == 0){
				cout << "NOT FOUND" << '\n';
				exit(0);
			}
			else{
				if(Current_env -> dict.count(Name)) cout << Current_env -> dict[Name] << " in ob." << '\n';
				if(Current_env -> dict_lambda.count(Name)) cout << Current_env -> dict_lambda[Name] << " in ob_lambda." << '\n';
			}
		}
		string Ob;
		if(Current_env -> dict.count(Name)) Ob = Current_env -> dict[Name];
		Control_element* Ob_eta_or_lambda;
		if(Current_env -> dict_lambda.count(Name)) Ob_eta_or_lambda = Current_env -> dict_lambda[Name];

		if(Current_env -> dict.count(Name)){
			if(Ob[0] == '(') {
				Control_element* CE = new Control_element(TUPLE, Ob);
				STACK.push_back(CE);
			}
			else{
				Control_element* CE = new Control_element(INT, Ob);
				STACK.push_back(CE);
			}			
		}
		else if(Current_env -> dict_lambda.count(Name)){
			STACK.push_back(Ob_eta_or_lambda);  
		}
		CONTROL.pop_back();
	}
  else if(CONTROL.back() -> get_type() == Y){
		STACK.push_back(CONTROL.back());
		CONTROL.pop_back();
	}
}

void parser::CSE_rule2(){
	auto CE = CONTROL.back();
	CE -> get_lambda_ptr() -> envir_Num = Current_env -> envir_Num;
  STACK.push_back(CE);
	CONTROL.pop_back();	
}

void parser::CSE_rule4(){
	MAX_ENV_NUM++;
	Lambda_element*  lambda = STACK.back() -> get_lambda_ptr();	
	STACK.pop_back();
	int k = lambda -> delta_Num;	
	int c = lambda -> envir_Num;
	
	ENV* new_env = new ENV(ENV_list[c], MAX_ENV_NUM);
	
	vector<string> vars = lambda -> variables;
	auto Rand = STACK.back();
	

	
	if(vars.size()>1 && Rand -> get_type()! = TUPLE){
		cout << "Error occurred in CSE rule 4" << '\n';
		cout << "Expected stack top to be TUPLE but found " <<  STACK.back() -> get_type() << '\n';
		exit(0);
	}
	if(vars.size() == 1){
		if (Rand -> get_type() == INT || Rand -> get_type() == TUPLE){
			new_env -> dict[vars[0]] = Rand -> get_content();
			cout <<  vars[0]  << " : " << Rand -> get_content()  << " in ENV" << MAX_ENV_NUM << " for ob."  << '\n';
		}
		else {
			new_env -> dict_lambda[vars[0]] = Rand ;
			cout <<  vars[0]  << " : " << Rand -> get_content()  << " in ENV" << MAX_ENV_NUM << " for ob_lambda."  << '\n';
		}		
		if(IF_DEBUG){
			cout << "e" + to_string(MAX_ENV_NUM) << " = [" << Rand -> get_content() \
			 <<  "/" << vars[0] << "]e" + to_string(c) << '\n';
		}
		
	}
	else if(vars.size() > 1){
		int n = vars.size();
		string tuple_content = Rand -> get_content();
		tuple_content.erase(tuple_content.size()-1);
		tuple_content.erase(tuple_content.begin());
		if(IF_DEBUG) cout <<  "The tuple contents are: " << tuple_content << '\n';
		vector<string> values = split_string(tuple_content,',');
		for(int i = 0; i<n; ++i){
			new_env -> dict[vars[i]] = values[i];
			if(IF_DEBUG){
				cout << "e"+to_string(MAX_ENV_NUM) << " = [" << values[i] \
				 <<  "/" << vars[i] << "]e" + to_string(c) << '\n';
			}			
		}
	}
	else {
		cout << "Error occurred in CSE rule 4" << '\n';
		cout << "Lambda_element's variables vector size < 1 " << '\n';
	}	
	Current_env = new_env;
	ENV_list.push_back(Current_env);
  if(IF_DEBUG){
  	cout << "Now in ENV" + to_string(Current_env -> envir_Num) << '\n';
  }	
	STACK.pop_back();
	CONTROL.pop_back();
	Control_element* CE = new Control_element(ENVIRONMENT, "e" + to_string(MAX_ENV_NUM));
	STACK.push_back(CE);
	CONTROL.push_back(CE);	
	vector<Control_element*> DEL_k = Control_structures[k];
	for (auto CE_it : DEL_k){
		CONTROL.push_back(CE_it);	
	}
}

void parser::CSE_rule5(){
	auto CE = STACK.back();
	CONTROL.pop_back();
	STACK.pop_back();
	if(STACK.back() -> get_type() ! =  ENVIRONMENT) {
		cout << "Error occurred in CSE rule 5" << '\n';
		cout << "Expected stack top to be environment but found " <<  STACK.back() -> get_content() << '\n';
		exit(0);
	}
	else{
		STACK.pop_back();
		STACK.push_back(CE);
	}
	ENV* Current_env_finder;
	for(auto CE_it = STACK.rbegin(); CE_it ! =  STACK.rend(); ++CE_it){
		if((*CE_it) -> get_type() == ENVIRONMENT){
			string i = (*CE_it) -> get_content();	
			i.erase(i.begin());		
			Current_env_finder = ENV_list[stoi(i)];
			break;
		}
	}
	Current_env = Current_env_finder;
}

void parser::CSE_rule6(){
	auto RAND1 = STACK.back();
	STACK.pop_back();
	auto RAND2 = STACK.back();
	STACK.pop_back();
	
	if(RAND1 -> get_type() ! =  INT && RAND1 -> get_type() ! =  BOOL ){
		cout << "Error occurred in CSE rule 6" << '\n';
		cout << "Expected RAND1 to be INT or BOOL but found " <<  RAND1 -> get_content() << '\n';
		exit(0);
	}
	if(RAND2 -> get_type() ! =  INT && RAND2 -> get_type() ! =  BOOL ){
		cout << "Error occurred in CSE rule 6" << '\n';
		cout << "Expected RAND2 to be INT or BOOL but found " <<  RAND2 -> get_content() << '\n';
		exit(0);
	}
	if(CONTROL.back() -> get_content() == "+"){
		Control_element* CE = new Control_element(INT, \
		to_string(stoi(RAND1 -> get_content()) + stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}
	else 	if(CONTROL.back() -> get_content() == "-"){
		Control_element* CE = new Control_element(INT, \
		to_string(stoi(RAND1 -> get_content()) - stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}
	else 	if(CONTROL.back() -> get_content() == "*"){
		Control_element* CE = new Control_element(INT, \
		to_string(stoi(RAND1 -> get_content()) * stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}
	else 	if(CONTROL.back() -> get_content() == "/"){
		Control_element* CE = new Control_element(INT, \
		to_string(stoi(RAND1 -> get_content()) / stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}
	else 	if(CONTROL.back() -> get_content() == "**"){
		Control_element* CE = new Control_element(INT, \
		to_string(pow(stoi(RAND1 -> get_content()), stoi(RAND2 -> get_content()))));
		STACK.push_back(CE);
	}
	else 	if(CONTROL.back() -> get_content() == "gr"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) > stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "ge"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) > =  stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "ls"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) < stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "le"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) < =  stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "eq"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) == stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "ne"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stoi(RAND1 -> get_content()) ! =  stoi(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "or"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stob_CONV(RAND1 -> get_content()) || stob_CONV(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	else 	if(CONTROL.back() -> get_content() == "&"){
		Control_element* CE = new Control_element(BOOL, \
		btos_CONV(stob_CONV(RAND1 -> get_content()) && stob_CONV(RAND2 -> get_content())));
		STACK.push_back(CE);
	}	
	CONTROL.pop_back();		
}

void parser::CSE_rule7(){
	if(STACK.back() -> get_type() ! =  INT && STACK.back() -> get_type() ! =  BOOL) {
		cout << "Error occurred in CSE rule 7" << '\n';
		cout << "Expected STACK.top to be INT or BOOL but found " <<  STACK.back() -> get_content() << '\n';
		exit(0);		
	}
	if(STACK.back() -> get_type() == INT){
		if (CONTROL.back() -> get_content() ! =  "neg"){
			cout << "Error occurred in CSE rule 7" << '\n';
			cout << "STACK.top is INT, but ";
			cout << "CONTROL.top is " <<  CONTROL.back() -> get_content() << '\n';
			exit(0);				
		}
		int num = -1*stoi(STACK.back() -> get_content());
		CONTROL.pop_back();
		STACK.pop_back();
		Control_element* CE = new Control_element(INT, to_string(num));
		STACK.push_back(CE);
	}
	if(STACK.back() -> get_type() == BOOL){
		if (CONTROL.back() -> get_content() ! =  "not"){
			cout << "Error occurred in CSE rule 7" << '\n';
			cout << "STACK.top is BOOL, but ";
			cout << "CONTROL.top is " <<  CONTROL.back() -> get_content() << '\n';
			exit(0);				
		}
		if(STACK.back() -> get_content() ! =  "true" && STACK.back() -> get_content() ! =  "false") {
			cout << "Error occurred in CSE rule 7" << '\n';
			cout << "Invalid bool string: ";
			cout << "STACK.top is " <<  STACK.back() -> get_content() << '\n';
			exit(0);					
		}
		string boolean = (STACK.back() -> get_content() == "true")? "false" : "true";
		CONTROL.pop_back();
		STACK.pop_back();
		Control_element* CE = new Control_element(BOOL, boolean);
		STACK.push_back(CE);
	}
}

void parser::CSE_rule8(){
	if(STACK.back() -> get_type() ! =  BOOL){
		cout << "Error occurred in CSE rule 8" << '\n';
		cout << "Expected STACK.top to be BOOL but found " <<  STACK.back() -> get_content() << '\n';
		exit(0);			
	}
	CONTROL.pop_back();
	if(STACK.back() -> get_content() == "true"){
		STACK.pop_back();
		CONTROL.pop_back();
		
		auto del_then = CONTROL.back() -> get_Control_ptr();
		CONTROL.pop_back();
		for (auto CE : del_then){
			CONTROL.push_back(CE);
		}
	}
	else{
		STACK.pop_back();
		auto del_else = CONTROL.back() -> get_Control_ptr();
		CONTROL.pop_back();
		CONTROL.pop_back();
		for (auto CE : del_else){
			CONTROL.push_back(CE);
		}
	}
}

void parser::CSE_rule9(){
	int num_under_tau = stoi(CONTROL.back() -> get_content());
	CONTROL.pop_back();
	string tuple_content;
	for(int i = 1; i< = num_under_tau; ++i){
		if(STACK.back() -> get_type() ! =  INT && STACK.back() -> get_type() ! =  TUPLE) {
			cout << "Error occurred in CSE rule 9" << '\n';
			cout << "Expected STACK.top to be INT or TUPLE as tuple part but found " <<  STACK.back() -> get_content() << '\n';
			exit(0);				
		}
		tuple_content + =  STACK.back() -> get_content() +",";
		STACK.pop_back();
	}
	tuple_content.erase(tuple_content.length()-1);
	tuple_content = "(" + tuple_content + ")";
	Control_element* CE = new Control_element(TUPLE, tuple_content);
	STACK.push_back(CE);
}

void parser::CSE_rule10(){
	CONTROL.pop_back();
	string tuple_content = STACK.back() -> get_content();
	tuple_content.erase(tuple_content.begin());
	tuple_content.erase(tuple_content.size()-1);
	vector<string> tuple_elements = split_string(tuple_content);

	STACK.pop_back();
	if(STACK.back() -> get_type() ! = INT){
		cout << "Error occurred in CSE rule 10" << '\n';
		cout << "Expected STACK second to TOP to be INT as tuple index but found " <<  STACK.back() -> get_content() << '\n';
		exit(0);		
	}
	int I = stoi(STACK.back() -> get_content());
	
	
	if(I > tuple_elements.size()){
		cout << "Error occurred in CSE rule 10" << '\n';
		cout << "Exceed size of the tuple." << '\n';
		cout << "I = " << I << "and the size of tuple is " << tuple_elements.size() << '\n';
		exit(0);		
	}
	STACK.pop_back();
	if(tuple_elements[I-1][0] == '('){
		Control_element* CE = new Control_element(TUPLE, tuple_elements[I-1]);
	  STACK.push_back(CE);
	}
	else{
		Control_element* CE = new Control_element(INT, tuple_elements[I-1]);
	  STACK.push_back(CE);		
	}
}

void parser::CSE_rule12(){
	CONTROL.pop_back();
	STACK.pop_back();
	Lambda_element* lambda = STACK.back() -> get_lambda_ptr();	
	Control_element* CE_eta = new Control_element(lambda);

	STACK.pop_back();
	STACK.push_back(CE_eta);
}

void parser::CSE_rule13(){
	Control_element* CE_gamma = new Control_element(GAMMA, "gamma");
	CONTROL.push_back(CE_gamma);
	Lambda_element* lambda = STACK.back() -> get_lambda_ptr();

	Control_element* CE_lambda = new Control_element(lambda, LAMBDA, "lambda");
	STACK.push_back(CE_lambda);
}