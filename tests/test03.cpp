#include <iostream>
#include <string>
#include <cassert>

std::string trimTailComment(const std::string& str){
  int depthParentheses = 0;
  int depthSquareBracket = 0;
  int depthCurlyBrackets = 0;
  bool inStringA = false;
  bool inStringB = false;

  auto isCommentChar = [](const char& c){
    return c == ';' || c == '#';
  };

  auto isComment = [&](const char& c){
    if(!isCommentChar(c)) return false;
    if(depthParentheses > 0) return false;
    if(depthSquareBracket > 0) return false;
    if(depthCurlyBrackets > 0) return false;
    if(inStringA) return false;
    if(inStringB) return false;
    return true;
  };

  std::size_t sz = str.size();
  for(std::size_t i = 0; i < sz; ++i){
    auto c = str[i];
    if(c == '\\'){
      if(i + 1 < sz){
        i += 1;
        continue;
      }
    }else if(c == '('){
      depthParentheses += 1;
    }else if(c == ')'){
      depthParentheses -= 1;
    }else if(c == '['){
      depthSquareBracket += 1;
    }else if(c == ']'){
      depthSquareBracket -= 1;
    }else if(c == '{'){
      depthCurlyBrackets += 1;
    }else if(c == '}'){
      depthCurlyBrackets -= 1;
    } else if (c == '"') {
      if(depthSquareBracket == 0 || !inStringB){
        inStringA = !inStringA;
      }
    } else if (c == '\'') {
      if(depthSquareBracket == 0 || !inStringA){
        inStringB = !inStringB;
      }
    }else if(isComment(c)){
      return str.substr(0, i);
    }
  }

  return str;
}

int main(){
  std::string str1 = R"~~~(MultiLineMacro: '#' (~[\n]*? '\\' '\r'? '\n')+ ~ [\n] ;comment + -> channel (HIDDEN))~~~";
  std::cout << "input  = {" << str1 << "}\n";
  std::string result1 = trimTailComment(str1);
  std::cout << "result = {" << result1 << "}\n\n";

  std::string str2 = R"~~~(Foo: '#' bar ; this is comment)~~~";
  std::cout << "input  = {" << str2 << "}\n";
  std::string result2 = trimTailComment(str2);
  std::cout << "result = {" << result2 << "}\n\n";

  std::string str3 = R"~~~(Rule: (a ; b) ; real comment)~~~";
  std::cout << "input  = {" << str3 << "}\n";
  std::string result3 = trimTailComment(str3);
  std::cout << "result = {" << result3 << "}\n\n";

  std::string str4 = R"~~~(NoCommentHere: 'abc' [xyz])~~~";
  std::cout << "input  = {" << str4 << "}\n";
  std::string result4 = trimTailComment(str4);
  std::cout << "result = {" << result4 << "}\n\n";

  std::string str5 = R"~~~(Rule: 'a' 'b' # hash comment here)~~~";
  std::cout << "input  = {" << str5 << "}\n";
  std::string result5 = trimTailComment(str5);
  std::cout << "result = {" << result5 << "}\n\n";

  return 0;
}
