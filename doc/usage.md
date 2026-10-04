# A minimal lexer example.

## Generate a config.ini example:
./re2c-lexer-maker -? >> config.ini

## Generate the lexer source code:
./re2c-lexer-maker -i config.ini -t all -d MyLexerLib

## Build it.
cmake -S MyLexerLib -B MyLexerLib/build
cmake --build MyLexerLib/build
