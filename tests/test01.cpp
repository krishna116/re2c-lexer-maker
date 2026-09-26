#include <iostream>
#include "LexerConfigParser.h"

int main() {
  std::string text = R"~(
// Config lexer.
LexerClassName = MyLexer;
LexerCharType = char;

// Define rules.
[Spaces]
  RegexPattern = [ \t\v\f\r\n]+;

[Key]
  RegexPattern = [a-zA-Z_][a-zA-Z0-9_]{0,30};
  ReturnTokenType = KEY;

[Equal]
  RegexPattern = [=];
  ReturnTokenType = EQUAL;

[Value]
  RegexPattern = [0-9]+;
  ReturnTokenType = VALUE;
)~";

  LexerConfigParser parser;
  auto cfg = parser.parse(text);

  if(parser.hasError()){
    std::cout<<parser.getLastError()<<"\n";
  }

  std::cout << "Lexer class name  = " << cfg.lexerClassName << "\n";
  std::cout << "Lexer char type   = " << cfg.lexerCharType << "\n";
  std::cout << "\n";
  
  for(auto& rule : cfg.ruleArray){
    std::cout << "rule name       = " << rule.ruleName << "\n";
    std::cout << "regex pattern   = " << rule.regexPattern << "\n";
    std::cout << "token type name = " << rule.tokenTypeName << "\n";
    std::cout << "\n";
  }

  printf("done\n");
  return 0;
}
