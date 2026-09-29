# re2c lexer maker

It is used to generate C++ lexer class, and you need to install [re2c](https://re2c.org/index.html) in advance.

## Usage
```BASH
re2c-lexer-maker -i <config.ini>
re2c-lexer-maker -d <directory> -t <type> -i <config.ini>

Options:
-i <config.ini>     Input config file.
-x                  Input config file from stdin.
-d <directory>      Specify output file directory.
-t <type>           Specify output file type, and the file type can be
                    one of [token.h, lexer.h, lexer.l, cmake.txt, all].
-h,--help           Print this help.
-?,--help-example   Print config.ini example.
-v,--version        Print version.
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
mkdir MyLexerLib && cd MyLexerLib && re2c-lexer-maker -? >> config.ini && re2c-lexer-maker -i config.ini -t all -d . && cmake -S . -B build && cmake --build build
```
