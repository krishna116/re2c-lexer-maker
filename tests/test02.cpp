#include <iostream>

#include "LexerConfigParser.h"
#include "CodeGenerator.h"

int main(){
  std::string text = R"~(  
  LexerClassName = MyLexer;
  LexerCharType  = char;
  [Key]
    RegexPattern    = [a-zA-Z_][a-zA-Z0-9_]{0,30};
    ReturnTokenType = KEY;
  [Equal]
    RegexPattern    = "=";
  [Value]
    RegexPattern    = [0-9]+;
    ReturnTokenType = VALUE;
  [Spaces]
    RegexPattern    = [ \t\v\f\r\n]+;
)~";

  LexerConfigParser parser;
  auto cfg = parser.parse(text);

  if(parser.hasError()){
    std::cout<<parser.getLastError()<<"\n";
    return 1;
  }

  CodeGenerator codeGenerator;
  std::string tokenHeaderCode;
  std::string classHeaderCode; 
  std::string classLexCode;
  std::string cmakeCode;
  if(codeGenerator.genCode(cfg, tokenHeaderCode, classHeaderCode, classLexCode, cmakeCode)){
    std::cout << "//[tokenHeaderCode]";
    std::cout << tokenHeaderCode << std::endl;
    std::cout << "------------------------------------------------------------------\n";

    // std::cout << "//[classHeaderCode]";
    // std::cout << classHeaderCode << std::endl;
    // std::cout << "------------------------------------------------------------------\n";

    // std::cout << "//[classLexCode]";
    // std::cout << classLexCode << std::endl;
    // std::cout << "------------------------------------------------------------------\n";
  }else{
    std::cout <<codeGenerator.getLastError() <<std::endl;
  }

  return 0;
}