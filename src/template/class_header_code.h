#pragma once

//===================================================================
// Class Header Code Template
//===================================================================
constexpr const char* ClassHeaderCodeTemplate = R"~(
#ifndef @CLASSNAME@_H
#define @CLASSNAME@_H

#include "Token.h"

//===============================
// @ClassName@ declaration.
//===============================
class @ClassName@ {
public:
  using char_t = Token::char_t;
  using TokenType = Token::Type;
  using Position = Token::Position;

  /**
   * Construct lexer with data.
   * 
   * Note: The lexical analyzer only references data; it does not manage 
   * the data buffer. You must ensure that the data buffer lifecycle is 
   * longer than that of the lexical analyzer and the tokens it outputs.
   * 
   * @param data  Input data.
   * @param size  Input data size.
   */
  @ClassName@(const char_t *data, size_t size)
  : cursor{data}, limit{data + size}, marker{}, tok{} {};

  /**
   * Get next token.
   * 
   * It is finished if it returns TokenType::LEXER_END_OF_FILE, and
   * you can use @ref hasError to check if any error happened.
   * 
   * @param token         Output next token.
   * @return TokenType    Output next token type.
   */
  TokenType lex(Token &token);

  /**
   * Check if lexer has error.
   * 
   * If lexer has error, you can use @ref getLastError to get the error.
   * 
   * @return true     Has error.
   * @return false    Has no error.
   */
  bool hasError() const { return !lastError.empty(); }

  /**
   * Get last error.
   * 
   * @return std::string  Last error.
   */
  std::string getLastError() const { return lastError; }

private:
  const char_t *cursor;   // re2c api.
  const char_t *limit;    // re2c api.
  const char_t *marker;   // re2c api.
  const char_t *tok;      // next token start.
  Position pos;           // next token start position.
  std::string lastError;  // last error.

  // re2c api.
  inline char_t yypeek(){ return *cursor; }

  // re2c api.
  inline void yyskip(){
    ++cursor;
    if(*cursor == '\n'){  // Record the position of the character.
      pos.y += 1;
      pos.x = 1;
    }else{
      pos.x += 1;
    }
  }

  // re2c api.
  inline void yybackup(){ marker = cursor; }

  // re2c api.
  inline void yyrestore(){ cursor = marker; }
};

#endif // #define @CLASSNAME@_H
)~";
