#pragma once

#include "LexerConfig.h"

class LexerConfigParser {
public:
  LexerConfig parse(const std::string &iniText);

  bool hasError() { return !mLastError.empty(); }

  std::string getLastError() { return mLastError; }

private:
  std::string mLastError;

  struct Line {
    size_t id;
    std::string text;
  };
  using LineArray = std::vector<Line>;

  LineArray readLines(const std::string &str);
  std::string trimSpace(const std::string &str);
  std::string getLexerClassName(const Line& line);
  std::string getLexerCharType(const Line& line);
  std::string getTokenTypeName(const Line& line);
  std::string getIdentifier(const Line& line);
  std::string getRuleName(const Line& line);
  std::string getRegexPattern(const Line& line);
  bool isValidCName(const std::string &name);
};
