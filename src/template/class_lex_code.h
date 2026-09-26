#pragma once

namespace PlaceHolder{
constexpr const char* RuleDefine = "  @RuleName@@SpaceAfterRuleName@= @RegexPattern@;";
constexpr const char* RuleName = "@RuleName@";
constexpr const char* SpaceAfterRuleName = "@SpaceAfterRuleName@";
constexpr const char* RegexPattern = "@RegexPattern@";
constexpr const char* ArrayOfRuleDefine = "@ArrayOfRuleDefine@";
constexpr const char* TokenName = "@TokenName@";
constexpr const char* ActionReturnToken = R"(  @RuleName@ { 
    token.setTypeAndText(Token::Type::@TokenName@, {tok, static_cast<unsigned>(cursor - tok)});
    token.setEndPos(pos);
    return Token::Type::@TokenName@;
  }
)";
constexpr const char* ActionSkipToken = R"(  @RuleName@ { 
    goto @ClassName@_lex_start;
  }
)";
constexpr const char* ArrayOfAction = "@ArrayOfAction@";
} // end namespace PlaceHolder

//====================================================
// Class Lex Code Template
//====================================================
constexpr const char* ClassLexCodeTemplate = R"~(
// re2c <thisFile> -o @ClassName@.cpp

#include "@ClassName@.h"

//===============================
// MyLexer implementation.
//===============================

@ClassName@::TokenType @ClassName@::lex(Token &token) {
@ClassName@_lex_start:
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

@ArrayOfRuleDefine@

@ArrayOfAction@

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
)~";
