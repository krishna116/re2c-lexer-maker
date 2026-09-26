#pragma once

#include <string>

struct LexerConfig;
class CodeGenerator{
public:
  bool genCode(const LexerConfig& cfg,
                std::string& tokenHeaderCode,
                std::string& classHeaderCode,
                std::string& classLexCode,
                std::string& cmakeCode);

  bool hasError(){
    return !mLastError.empty();
  }

  std::string getLastError(){
    return mLastError;
  }

private:
  std::string mLastError;
};