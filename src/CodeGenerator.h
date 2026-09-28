#pragma once

#include <string>

struct LexerConfig;
class CodeGenerator{
public:
  std::string genTokenHeaderCode(const LexerConfig& cfg);
  std::string genClassHeaderCode(const LexerConfig& cfg);
  std::string genClassLexCode(const LexerConfig& cfg);
  std::string genCmakeCode(const LexerConfig& cfg);

  bool hasError(){
    return !mLastError.empty();
  }

  std::string getLastError(){
    return mLastError;
  }

private:
  std::string mLastError;
};
