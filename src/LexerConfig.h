#pragma once

#include <string>
#include <vector>

struct LexerConfig {
  std::string lexerClassName;   // Lexer class name for example: "AnyLexer", "MyLexer".
  std::string lexerCharType;    // Char type may be: char, char8_t, wchar_t, char16_t, char32_t.
  struct Rule {
    std::string ruleName;       // ruleName cannot be empty, and ruleName must be unique.
    std::string regexPattern;   // regexPattern cannot be empty.
    std::string tokenTypeName;  // tokenTypeName may be empty, and it may not be unique(reference by others).
  };
  using RuleArray = std::vector<Rule>;
  RuleArray ruleArray;
};