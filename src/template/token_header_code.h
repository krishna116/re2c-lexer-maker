#pragma once

constexpr const char* ReservedTokenTypeName = "LEXER_END_OF_FILE";

namespace PlaceHolder{
  constexpr const char* LexerCharType = "@LexerCharType@";
  constexpr const char* ClassName = "@ClassName@";
  constexpr const char* className = "@className@";
  constexpr const char* CLASSNAME = "@CLASSNAME@";
  constexpr const char* EnumClassMemberArray = "@EnumClassMemberArray@";
  constexpr const char* EnumClassMemberStrArray = "@EnumClassMemberStrArray@";
}

//===================================================================
// Token Header Code Template
//===================================================================
constexpr const char* TokenHeaderCodeTemplate = R"~(
#ifndef @CLASSNAME@_TOKEN_H
#define @CLASSNAME@_TOKEN_H

#include <cstddef>      // std::size_t
#include <string>       // std::string
#include <string_view>  // C++17 string_view

//===============================
// Token definition.
//===============================
class Token {
public:
  using char_t = @LexerCharType@;
  using size_t = std::size_t;
  
  // Token type.
  enum class Type : unsigned { 
@EnumClassMemberArray@
  };

  // Token text type.
  using Text = std::basic_string_view<char_t>;

  // Position in the source file.
  struct Position {
    int x;  // x col.
    int y;  // y row.

    Position():x{1}, y{1} {}
    Position(int nx, int ny):x{nx},y{ny}{}

    std::string str() const{ 
      return "[" + std::to_string(y) + ":" + std::to_string(x) + "]";
    }
  };

  // Token location in the source file.
  struct Location {
    Position start; // Token start position in the source file.
    Position end;   // Token end position in the source file.

    Location() : start{}, end{} {}

    std::string str() const{
      std::string s = start.str() + "->" + end.str();
      return s;
    }
  };

public:
  //===============================
  // Token public apis.
  //===============================
  Token() : type_(Type::LEXER_END_OF_FILE), text_{}, location_{} {}

  /**
   * Get toke type.
   * 
   * @return Token type.
   */
  const Type& getType()const { return type_; }

  /**
   * Get token text.
   * 
   * @return Token text.
   */
  const Text& getText()const { return text_; }

  /**
   * Get token location in the source file.
   * 
   * @return Token location.
   */
  const Location& getLocation()const { return location_; }

  /**
   * Get token location in the source file.
   * 
   * @return Token location.
   */
  const Position& getStartPos()const { return location_.start; }

  /**
   * Get token location in the source file.
   * 
   * @return Token location.
   */
  const Position& getEndPos()const { return location_.end; }

  /**
   * Get token typename.
   * 
   * @return Token type name.
   */
  const char* getTypeName()const{
    static const char* names[]={
@EnumClassMemberStrArray@
    };
    return names[static_cast<unsigned>(type_)];
  }

  /**
   * Set token start position.
   * 
   * @param pos   Token start position.
   */
  void setStartPos(const Position& pos){ location_.start = pos; }

  /**
   * Set token end position.
   * 
   * @param pos   Token end position.
   */
  void setEndPos(const Position& pos) { location_.end = pos; }

  /**
   * Set token type and text.
   * 
   * @param type  Token type.
   * @param text  Token text.
   */
  void setTypeAndText(Type type, Text text){ type_ = type; text_ = text; }

private:
  Type type_;          // Token type.
  Text text_;          // Token text.
  Location location_;  // Token location in the source file.
};

#endif // #define @CLASSNAME@_TOKEN_H
)~";
