#include <iostream>
#include "../src/Re2c.h"

std::string str = R"(int lex(const char* YYCURSOR) {
    /*!re2c
        re2c:yyfill:enable = 0;
        re2c:YYCTYPE = "char";

        [1-9][0-9]* { return 0; }
        *           { return 1; }
    */
}
)";

// Cannot use this string.
std::string str2 = R"(
// re2c <thisFile> -o MyLexer.cpp

#include "MyLexer.h"

//===============================
// MyLexer implementation.
//===============================

MyLexer::TokenType MyLexer::lex(Token &token) {
MyLexer_lex_start:
  tok = cursor;
  token.setStartPos(pos);

/*!re2c
  re2c:YYCTYPE = "char_t";
  re2c:yyfill:enable = 0;
  re2c:api = generic;
  re2c:api:style = free-form;
  re2c:YYPEEK       = "yypeek();";
  re2c:YYSKIP       = "yyskip();";
  re2c:YYBACKUP     = "yybackup();";
  re2c:YYRESTORE    = "yyrestore();";

  Key    = [a-zA-Z_][a-zA-Z0-9_]{0,30};
  Equal  = "=";
  Value  = [0-9]+;
  Spaces = [ \t\v\f\r\n]+;

  Key {
    token.setTypeAndText(Token::Type::KEY, {tok, static_cast<unsigned>(cursor - tok)});
    token.setEndPos(pos);
    return Token::Type::KEY;
  }

  Equal {
    goto MyLexer_lex_start;
  }

  Value {
    token.setTypeAndText(Token::Type::VALUE, {tok, static_cast<unsigned>(cursor - tok)});
    token.setEndPos(pos);
    return Token::Type::VALUE;
  }

  Spaces {
    goto MyLexer_lex_start;
  }


  [\x00] {
    token.setTypeAndText(Token::Type::LEXER_END_OF_FILE, {});
    token.setEndPos(pos);
    return Token::Type::LEXER_END_OF_FILE;
  }

  * {
    token.setTypeAndText(Token::Type::LEXER_END_OF_FILE, {});
    token.setEndPos(pos);

    lastError = "[Error] ";
    lastError += "unknown char [" + std::to_string(yych) + "] ";
    lastError += "at position " + token.getStartPos().str();

    cursor = limit; // stop parsing.
    return Token::Type::LEXER_END_OF_FILE;
  }
*/
}
)";

int main(){
  std::string out;
  Re2c::run(str2, out);

  std::cout << out <<std::endl;
}