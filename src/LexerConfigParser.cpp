#include <cctype>
#include <cstring>
#include <sstream>

#include "LexerConfigParser.h"

using Rule = LexerConfig::Rule;

inline bool is_az_AZ_(const char& ch){
  return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

inline bool is_az_AZ_09_(const char& ch){
  return (is_az_AZ_(ch)  || '0' <= ch && ch <= '9');
}

inline bool lineStartWith(const std::string& line, const std::string& token){
  auto tokenSize = token.size();
  return tokenSize <= line.size() && line.substr(0, tokenSize) == token;
}

LexerConfig LexerConfigParser::parse(const std::string &iniText) {
  LexerConfig cfg;

  auto resetRule = [](Rule& rule, const std::string& ruleName){
    rule.ruleName = ruleName;
    rule.regexPattern.clear();
    rule.tokenTypeName.clear();
  };

  auto addRule = [&](const Rule& rule)->bool{
    // No duplicate rule.
    for(auto& thisRule: cfg.ruleArray){
      if(thisRule.ruleName == rule.ruleName){
        mLastError += "[Error] Found duplicate rule name [" + rule.ruleName + "]\n";
        return false;
      }
    }
    // Rule's regex pattern cannot be empty.
    if(rule.regexPattern.empty()){
      mLastError += "[Error] Rule[" + rule.ruleName + "]'s regex pattern cannot be empty.\n";
      return false;
    }
    cfg.ruleArray.push_back(rule);
    return true;
  };

  auto isLexerConfigValid = [&](){
    return !cfg.lexerClassName.empty() && !cfg.lexerCharType.empty() && !cfg.ruleArray.empty();
  };

  auto lineArray = readLines(iniText);
  std::size_t lineArraySize = lineArray.size();
  std::size_t i = 0;

  Rule rule;
  while(i < lineArraySize){
    const auto& line = lineArray[i];
    if(lineStartWith(line.text, "LexerClassName")){
      cfg.lexerClassName = getLexerClassName(line);
    }else if(lineStartWith(line.text, "LexerCharType")){
      cfg.lexerCharType = getLexerCharType(line);
    }else if(lineStartWith(line.text, "[")){
      if(!rule.ruleName.empty() && !addRule(rule)) return {};
      resetRule(rule, getRuleName(line));
    }else if(lineStartWith(line.text, "RegexPattern")){
      rule.regexPattern = getRegexPattern(line);
    }else if(lineStartWith(line.text, "ReturnTokenType")){
      rule.tokenTypeName = getTokenTypeName(line);
    }else{
      mLastError += "[Error] parser error at line [\n" + std::to_string(line.id) + "]\n";
      return {};
    }
    ++i;
  }

  // Get the last rule.
  if(!rule.ruleName.empty() && !addRule(rule)){
    return {};
  }

  // Final check.
  if(!isLexerConfigValid()){
    mLastError += "[Error] Input lexer config is invalid.\n";
    return {};
  }

  return cfg;
}

LexerConfigParser::LineArray LexerConfigParser::readLines(const std::string &str) {
  LineArray lineArray;
  std::stringstream ss{str};
  std::string text;
  size_t i = 0;
  while (std::getline(ss, text)) {
    ++i;
    auto trimmed = trimSpace(text);
    if (trimmed.empty() || (trimmed[0] == ';' || trimmed[0] == '#' || trimmed[0] == '/')) {
      continue;
    }
    lineArray.push_back({i, trimmed});
  }
  return lineArray;
}

std::string LexerConfigParser::trimSpace(const std::string &str) {
  auto begin = str.find_first_not_of(" \t\v\f\r\n");
  if (begin != std::string::npos) {
    auto end = str.find_last_not_of(" ;\t\v\f\r\n");
    if(end != std::string::npos){
      return str.substr(begin, end - begin + 1);
    }
  }
  return {};
}

std::string LexerConfigParser::getTokenTypeName(const Line& line){
  auto identifier = getIdentifier(line);
  auto begin = identifier.find_first_not_of("\"'");
  if(begin!= std::string::npos){
    auto end = identifier.find_last_not_of("\"'");
    if(end != std::string::npos){
      return identifier.substr(begin,end-begin + 1);
    }
  }
  return identifier;
}

std::string LexerConfigParser::getRegexPattern(const Line& line){
  auto first = line.text.find_first_of('=');
  if(first != std::string::npos){
    first += 1;
    auto size = line.text.size();
    while(first < size && std::isspace(line.text[first])) ++first;
    if(first < size){
      auto last = line.text.find_last_not_of(" ;\t\v\f\r\n");
      if(last != std::string::npos){
        if(first <= last){
          return line.text.substr(first, last - first + 1);
        }
      }
    }
  }
  mLastError += "[Error] Cannot get regex pattern at line [" + std::to_string(line.id) + "]\n";
  return {};
}

std::string LexerConfigParser::getRuleName(const LexerConfigParser::Line& line){
  auto first = line.text.find_first_not_of("[");
  if(first != std::string::npos){
    auto last = line.text.find_last_not_of("]");
    if(last != std::string::npos){
      auto ruleName = line.text.substr(first, last - first + 1);
      if(isValidCName(ruleName)){
        return ruleName;
      }else{
        mLastError += "[Error] Current rule name is invalid at line [" + std::to_string(line.id) + "]\n";
        mLastError += "[Error] Current rule name is [" + ruleName + "]\n";
      }
    }
  }
  mLastError += "[Error] Cannot get rule name at line [" + std::to_string(line.id) + "]\n";
  return {};
}

std::string LexerConfigParser::getIdentifier(const LexerConfigParser::Line& line){
  auto pos = line.text.find_first_of('=');
  if(pos == std::string::npos){
    return {};
  }

  // Skip "=";
  pos += 1;

  // Skip spaces.
  auto size = line.text.size();
  while(pos < size && std::isspace(line.text[pos])) ++pos;

  std::string identifier;
  if(pos < size && is_az_AZ_(line.text[pos])){
    identifier.push_back(line.text[pos]);
    ++pos;
    while(pos < size && is_az_AZ_09_(line.text[pos])){
      identifier.push_back(line.text[pos]);
      ++pos;
    }
    return identifier;
  }

  return {};
}

std::string LexerConfigParser::getLexerCharType(const LexerConfigParser::Line& line){
  auto lexerCharType = getIdentifier(line);
  if(lexerCharType.empty()){
    // mLastError += "[Error] Cannot get lexer char type at line [" + std::to_string(line.id) + "]\n";
    lexerCharType = "char";
  }
  return lexerCharType;
}

std::string LexerConfigParser::getLexerClassName(const LexerConfigParser::Line& line){
  auto lexerClassName = getIdentifier(line);
  if(lexerClassName.empty()){
    // mLastError += "[Error] Cannot get lexer class name at line [" + std::to_string(line.id) + "]\n";
    lexerClassName = "MyLexer";
  }
  return lexerClassName;
}

bool LexerConfigParser::isValidCName(const std::string &name) {
  auto size = name.size();
  std::size_t i = 0;
  if(i < size && is_az_AZ_(name[i])){
    ++i;
    while(i < size && is_az_AZ_09_(name[i])){
      ++i;
    }
    return i > 0 && i == size;
  }
  return false;
}