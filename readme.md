# re2c lexer maker

It is used to generate C++ lexer class, and you need to install [re2c](https://re2c.org/index.html) in advance.

## Usage
```BASH
re2c-lexer-maker -i <file.ini> -o <directory>

Options:
-i,--input  <file.ini>    Specify config file.
-o,--output <directory>   Specify output code directory.
-h,--help                 Print this help.
-v,--version              Print version.

# Start example of file.ini =====================
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
# End example of file.ini =======================
```

## Public interface of the generated lexer
```C++

//===============================
// MyLexer declaration.
//===============================
class MyLexer {
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
  MyLexer(const char_t *data, size_t size)
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
};
```

The generated code can be build with cmake:
```BASH
cmake -S . -B build
cmake --build build.
```
