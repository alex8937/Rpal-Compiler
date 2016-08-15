# ifndef TOKEN_H
# define TOKEN_H

# include <string>
using std::string; 

enum tokenType  {IDENTIFIER, INTEGER, STRING, RESERVED_WORD, OPERATOR, PUNCTUATION, DELETE, UNKNOWN, END_OF_FILE};

class token {
  private:
    string value;
    tokenType type;
  public:
    token(string vl,tokenType tp):value(vl),type(tp){}
    string get_value();
    tokenType get_type();
};
#endif