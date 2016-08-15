# ifndef INLINE_FUN_H
# define INLINE_FUN_H

# include <string>

using std::string; 

inline bool IS_LETTER(char a){
    return ((a >= 'A') && (a <= 'Z')) || ((a >= 'a') && (a <= 'z'))? true: false;
}
inline bool IS_DIGIT(char a){
    return (a >= '0' && a <= '9')? true: false;
}
inline bool IS_PUNCTION(char a){
    return (a == '(' || a == ')'|| a == ';' || a== ',')? true: false;
}
inline bool IS_OPERATOR_SYMBOL(char a){
    return (a == '+' || a == '-'|| a == '*' || a== '<' || a== '>' || a== '&' || a== '.' || \
            a == '@' || a == '/'|| a == ':' || a== '=' || a== '~' || a== '|' || a== '$' || \
            a == '!' || a == '#'|| a == '%' || a== '^' || a== '_' || a== '[' || a== ']' || \
            a == '{' || a == '}'|| a == '"' || a== '`' || a== '?' )? true: false;
}
/*inline bool IS_SPACE(char a){
     return (a == ' ' || a == '\t' || a == '\n' )? true:false;
}*/ //replaced by in-built isspace function
inline bool IS_ID_TYPE(string a){
  return (a.substr(0,4) == "<ID:")? true : false;
}
inline bool IS_INT_TYPE(string a){
  return (a.substr(0,5) == "<INT:")? true : false;
}
inline bool IS_BINOP_TYPE(string a){
    return (a == "+" || a == "-" || a == "*" || a == "/" || a == "**" || a == "gr" || a== "ge" || \
						a == "ls" || a == "le" || a == "eq" || a == "ne" || a == "or" || a == "&" )? true: false;
}
inline bool IS_UNOP_TYPE(string a){
    return (a == "not" || a == "neg")? true: false;
}
inline string Remove_Bracket(string a){
  string ans;
  int pos = 0;
  while(a[pos] != ':') pos++;
  for(int i = pos+1; a[i] != '>'; ++i) ans.push_back(a[i]);
  return ans;
}

inline string btos_CONV(bool s){
	return (s == true)? "true" : "false";
}

inline bool stob_CONV(string s){
	return (s == "true")? true : false;
}


#endif