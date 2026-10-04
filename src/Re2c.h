#pragma once

#include <string>

/**
 * Provides cross-platform process communication to call re2c.
 */
class Re2c{
public:
  /**
   * @param lexCode   Input lexer.h code
   * @param cppCode   Output lexer.cpp code.
   * 
   * @return 0        No error.
   * @return other    Error happened.
   */
  static int run(const std::string& lexCode, std::string& cppCode);
};