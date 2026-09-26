#include <iostream>
#include <filesystem>
#include <fstream>

#include "template/config.h"
#include "LexerConfigParser.h"
#include "CodeGenerator.h"

static int printUsage(){
  printf("re2c-lexer-maker -i <file.ini> -o <directory>\n");
  printf("\n");
  printf("Options:\n");
  printf("-i,--input  <file.ini>    Specify config file.\n");
  printf("-o,--output <directory>   Specify output dir.\n");
  printf("-h,--help                 Print this help.\n");
  printf("-H,--HELP                 Print help with example.\n");
  printf("-v,--version              Print version.\n");
  printf("\n");
  return 0;
}

static int printVersion(){
  printf("%s version %s\n", AppNameStr, AppVersionStr);
  return 0;
}

static int printExample(){
  static const char* example = R"(# Start example of file.ini =====================
  LexerClassName = MyLexer;
  LexerCharType  = char;
  [Key]
    RegexPattern    = [a-zA-Z_][a-zA-Z0-9_]{0,30};
    ReturnTokenType = KEY;
  [Equal]
    RegexPattern    = "=";
  [Value]
    RegexPattern    = [0-9]+;
    ReturnTokenType = VALUE;
  [Spaces]
    RegexPattern    = [ \t\v\f\r\n]+;)";
  printf("%s\n# End example of file.ini =======================\n\n", example);
  return 0;
}

static int writeCode(const std::string& stream, const std::filesystem::path& filePath){
  std::ofstream ofs(filePath);
  if(ofs.is_open()){
    ofs << stream;
    ofs.close();
    return 0;
  }
  return 1;
}

static int genCode(const std::string& iniFile, const std::string& outDir){
  std::filesystem::path inputFilePath(iniFile);
  if(inputFilePath.is_relative()){
    inputFilePath = std::filesystem::current_path();
    inputFilePath /= iniFile;
  }

  std::ifstream ifs(inputFilePath);
  std::string iniText;
  if(ifs.is_open()){
    iniText.insert(iniText.end(), (std::istreambuf_iterator<char>(ifs)),std::istreambuf_iterator<char>());
    ifs.close();
  }

  LexerConfigParser parser;
  auto cfg = parser.parse(iniText);
  if(parser.hasError()){
    std::cout << parser.getLastError() << "\n";
    return 2;
  }

  CodeGenerator codeGenerator;
  std::string tokenHeaderCode;
  std::string classHeaderCode; 
  std::string classLexCode;
  std::string cmakeCode;
  if(!codeGenerator.genCode(cfg, tokenHeaderCode, classHeaderCode, classLexCode, cmakeCode)){
    std::cout << codeGenerator.getLastError() << "\n";
    return 4;
  }

  std::filesystem::path path(outDir);
  auto tokenFilePath = path / "Token.h";
  auto classHeaderPath = path / std::string(cfg.lexerClassName + ".h");
  auto classLexPath = path / std::string(cfg.lexerClassName + ".l");
  auto cmakePath = path / "CMakeLists.txt";
  int count = 0;
  std::filesystem::create_directories(path);
  count += writeCode(tokenHeaderCode, tokenFilePath);
  count += writeCode(classHeaderCode, classHeaderPath);
  count += writeCode(classLexCode, classLexPath);
  count += writeCode(cmakeCode, cmakePath);
  if(count != 0){
    printf("[Error] output code failed.\n");
  }

  return count;
}

int main(int argc, char* argv[]){
  if(argc == 2){
    if(std::string("-v") == argv[1] || std::string("--version") == argv[1]){
      return printVersion();
    }else if(std::string("-h") == argv[1] || std::string("--help") == argv[1]){
      return printUsage();
    }else if(std::string("-H") == argv[1] || std::string("--HELP") == argv[1]){
      printUsage();
      printExample();
      return 0;
    }
  }else if(argc == 5){
    if(std::string("-i") == argv[1] || std::string("-input") == argv[1]){
      if(std::string("-o") == argv[3] || std::string("-output") == argv[3]){
        if(std::string(argv[2]) != argv[4]){
          return genCode(argv[2], argv[4]);
        }
      }
    }
  }

  printf("[Error] Invalid parameters.\n\n");
  
  return 1;
}