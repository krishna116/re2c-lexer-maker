#include <cctype>
#include <cstdio>

#include "CodeGenerator.h"
#include "LexerConfig.h"

#include "template/token_header_code.h"
#include "template/class_header_code.h"
#include "template/class_lex_code.h"
#include "template/cmake_code.h"

/**
 * Search and replace tokens in the text.
 */
static void replace(std::string& text, 
                             const std::string& toFind, 
                             const std::string& toReplace) {
  size_t pos = 0;
  while ((pos = text.find(toFind, pos)) != std::string::npos) {
    text.replace(pos, toFind.length(), toReplace);
    pos += toReplace.length();
  }
}

static std::string toUpper(const std::string& text){
  std::string str;
  for(auto& ch : text){
    str.push_back(std::toupper(ch));
  }
  return str;
}

using TokenTypeNameArray = std::vector<std::string>;

/**
 * Remove duplicate tokens while preserving the original token order.
 */
static TokenTypeNameArray getUniqueTokenTypeNameArray(const LexerConfig& cfg){
  TokenTypeNameArray  uniqueNameArray{ReservedTokenTypeName};
  for(auto& rule: cfg.ruleArray){
    if(rule.tokenTypeName.empty()) continue;
    bool found = false;
    for(auto& name: uniqueNameArray){
      if(name == rule.tokenTypeName){
        found = true;
        break;
      }
    }
    if(!found){
      uniqueNameArray.push_back(rule.tokenTypeName);
    }
  }
  return uniqueNameArray;
}

/**
 * Generate Token.h
 */
static std::string genTokenHeaderCode(const LexerConfig& cfg){
  const TokenTypeNameArray tokenTypeNameArray = getUniqueTokenTypeNameArray(cfg);
  const std::size_t size = tokenTypeNameArray.size();
  const std::string CLASSNAME = toUpper(cfg.lexerClassName);

  auto getEnumClassMemberArray = [&](){
    std::string str;
    std::string indent(4,' ');
    for(std::size_t i = 0; i<size; ++i){
      str += indent + tokenTypeNameArray[i];
      if(i + 1 < size) str += ",\n";
    }
    return str;
  };

  auto getEnumClassMemberStrArray = [&](){
    std::string str;
    std::string indent(6, ' ');
    for(std::size_t i = 0; i<size; ++i){
      str += indent + "\"" + tokenTypeNameArray[i] + "\"";
      if(i + 1 < size) str += ",\n";
    }
    return str;
  };

  std::string code = TokenHeaderCodeTemplate;
  replace(code, PlaceHolder::CLASSNAME, CLASSNAME);
  replace(code, PlaceHolder::LexerCharType, cfg.lexerCharType);
  replace(code, PlaceHolder::EnumClassMemberArray, getEnumClassMemberArray());
  replace(code, PlaceHolder::EnumClassMemberStrArray, getEnumClassMemberStrArray());
  return code;
}

/**
 * Generate MyLexer.h
 */
static std::string genClassHeaderCode(const LexerConfig& cfg){
  const std::string CLASSNAME = toUpper(cfg.lexerClassName);
  std::string code = ClassHeaderCodeTemplate;
  replace(code, PlaceHolder::CLASSNAME, CLASSNAME);
  replace(code, PlaceHolder::ClassName, cfg.lexerClassName);
  return code;
}

/**
 * Generate MyLexer.l
 */
static std::string genClassLexCode(const LexerConfig& cfg){
  auto getRuleDefine = [&](const std::string& ruleName, 
                           std::size_t nspace, 
                           const std::string& regexPattern){
    std::string code = PlaceHolder::RuleDefine;
    std::string spaces(nspace, ' ');
    replace(code, PlaceHolder::RuleName, ruleName);
    replace(code, PlaceHolder::SpaceAfterRuleName, spaces);
    replace(code, PlaceHolder::RegexPattern, regexPattern);
    return code;
  };

  auto getMaxRuleNameSize = [&](){
    std::size_t i = 0;
    for(auto& rule : cfg.ruleArray){
      auto size = rule.ruleName.size();
      if(i < size){
        i = size;
      }
    }
    return i + 1;
  };

  auto getArrayOfRuleDefine = [&](){
    auto maxRuleNameSize = getMaxRuleNameSize();
    std::string code;
    std::size_t size = cfg.ruleArray.size();
    for(std::size_t i = 0; i<size; ++i){
      auto& ruleName = cfg.ruleArray[i].ruleName;
      auto nspace = maxRuleNameSize - cfg.ruleArray[i].ruleName.size();
      auto& regexPattern = cfg.ruleArray[i].regexPattern;
      code += getRuleDefine(ruleName, nspace, regexPattern);
      if(i + 1 < size) code += "\n";
    }
    return code;
  };

  auto getAction = [&](const std::string& ruleName, const std::string& tokenTypeName){
    std::string code;
    if(tokenTypeName.empty()){
      code = PlaceHolder::ActionSkipToken;
      replace(code, PlaceHolder::ClassName, cfg.lexerClassName);
      replace(code, PlaceHolder::RuleName, ruleName);
    }else{
      code = PlaceHolder::ActionReturnToken;
      replace(code, PlaceHolder::RuleName, ruleName);
      replace(code, PlaceHolder::TokenName, tokenTypeName);
    }
    return code;
  };

  auto getArrayOfAction = [&](){
    std::string code;
    std::size_t size = cfg.ruleArray.size();
    for(std::size_t i = 0; i<size; ++i){
      code += getAction(cfg.ruleArray[i].ruleName, cfg.ruleArray[i].tokenTypeName);
      if(i + 1 < size) code += "\n";
    }
    return code;
  };

  std::string code = ClassLexCodeTemplate;
  replace(code, PlaceHolder::ClassName, cfg.lexerClassName);
  replace(code, PlaceHolder::ArrayOfRuleDefine, getArrayOfRuleDefine());
  replace(code, PlaceHolder::ArrayOfAction, getArrayOfAction());
  return code;
}

/**
 * Generate CMakeLists.txt
 */
static std::string genCmakeCode(const LexerConfig& cfg){
  std::string code = CmakeCodeTemplate;
  replace(code, PlaceHolder::ClassName, cfg.lexerClassName);
  return code;
}

/**
 * Generate token.h, MyLexer.h, MyLexer.l, CMakeLists.txt
 */
bool CodeGenerator::genCode(const LexerConfig &cfg,
                            std::string &tokenHeaderCode,
                            std::string &classHeaderCode,
                            std::string &classLexCode, std::string &cmakeCode) {
  tokenHeaderCode = genTokenHeaderCode(cfg);
  classHeaderCode = genClassHeaderCode(cfg);
  classLexCode = genClassLexCode(cfg);
  cmakeCode = genCmakeCode(cfg);
  return true;
}
