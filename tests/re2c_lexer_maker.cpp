#include <iostream>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <cstring>
#include <exception>

#include "template/config.h"
#include "LexerConfigParser.h"
#include "CodeGenerator.h"

static int printUsage(){
  printf("Usage:\n");
  printf("re2c-lexer-maker -i <config.ini>\n");
  printf("re2c-lexer-maker -d <directory> -t <type> -i <config.ini>\n");
  printf("\n");
  printf("Options:\n");
  printf("-i <config.ini>     Input config file.\n");
  printf("-x                  Input config file from stdin.\n");
  printf("-d <directory>      Specify output file directory.\n");
  printf("-t <type>           Specify output file type, and the file type can be\n");
  printf("                    one of [token.h, lexer.h, lexer.l, cmake.txt, all].\n");
  printf("-h,--help           Print this help.\n");
  printf("-?,--help-example   Print config.ini example.\n");
  printf("-v,--version        Print version.\n");
  printf("\n");
  return 0;
}

static int printVersion(){
  printf("%s version %s\n", AppNameStr, AppVersionStr);
  return 0;
}

static int printExample(){
  static const char* example = R"(// Example of MyLexer.ini
LexerClassName = MyLexer;                         // Lexer class name.
LexerCharType  = char;                            // Lexer char type.

[Key]                                             // Rule name.
  RegexPattern    = [a-zA-Z_][a-zA-Z0-9_]{0,30};  // Rule regex pattern.
  ReturnTokenType = KEY;                          // Return token type( is optional).

[Equal]                                           // Rule name.
  RegexPattern    = "=";                          // Rule regex pattern.

[Value]                                           // Rule name.
  RegexPattern    = [0-9]+;                       // Rule regex pattern.
  ReturnTokenType = VALUE;                        // Return token type

[Spaces]                                          // Rule name.
  RegexPattern    = [ \t\v\f\r\n]+;               // Rule regex pattern.
)";
  printf("%s", example);
  return 0;
}

static std::string toLower(const std::string& text){
  std::string str;
  for(auto& ch : text){
    str.push_back(std::tolower(ch));
  }
  return str;
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

static LexerConfig readInput(const std::string &filePathStr) {
  std::string iniText;
  if (!filePathStr.empty()) {
    std::filesystem::path inputFilePath(filePathStr);
    if (inputFilePath.is_relative()) {
      inputFilePath = std::filesystem::current_path();
      inputFilePath /= filePathStr;
    }
    std::cerr <<"[Info] Input File Path = " << inputFilePath <<"\n";
    if(!std::filesystem::exists(inputFilePath)){
      std::cerr <<"[Error] File doesn't exist:" << inputFilePath << "\n";
    }
    FILE* fp = fopen(inputFilePath.string().c_str(), "rb");
    if (!fp){
      std::cerr <<"[Error] Cannot read:" << inputFilePath << "\n";
      return {};
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    std::cerr <<"[Info] Input file size = " << size <<"\n";
    fseek(fp, 0, SEEK_SET);
    iniText.resize(size, 0);
    fread(iniText.data(), 1, size, fp);
    fclose(fp);
  } else {
    std::string line;
    while (std::getline(std::cin, line)) {
      line.push_back('\n');
      iniText += line;
    }
  }

  LexerConfigParser parser;
  auto cfg = parser.parse(iniText);
  if (parser.hasError()) {
    std::cerr << parser.getLastError() << "\n";
    return {};
  }
  return cfg;
}

static int genCode(const LexerConfig& cfg,  const std::string& outDir, const std::string& fileType){
  std::string tokenHeaderCode;
  std::string classHeaderCode; 
  std::string classLexCode;
  std::string cmakeCode;
  CodeGenerator codeGenerator;

  // Generate all code.
  if(fileType == "token" || fileType == "token.h"){
    tokenHeaderCode = codeGenerator.genTokenHeaderCode(cfg);
  }else if(fileType == "lexer.h"){
    classHeaderCode = codeGenerator.genClassHeaderCode(cfg);
  }else if(fileType == "lexer.l" || fileType == "lexer.lex"){
    classLexCode = codeGenerator.genClassLexCode(cfg);
  }else if(fileType == "cmake" || fileType == "cmake.txt" || fileType == "cmakelists.txt"){
    cmakeCode = codeGenerator.genCmakeCode(cfg);
  }else if(fileType == "all"){
    tokenHeaderCode = codeGenerator.genTokenHeaderCode(cfg);
    classHeaderCode = codeGenerator.genClassHeaderCode(cfg);
    classLexCode = codeGenerator.genClassLexCode(cfg);
    cmakeCode = codeGenerator.genCmakeCode(cfg);
  }else{
    // Default file type.
    classLexCode = codeGenerator.genClassLexCode(cfg);
  }

  // Output all code.
  if(!outDir.empty()){
    std::filesystem::path path(outDir);
    auto tokenFilePath = path / "Token.h";
    auto classHeaderPath = path / std::string(cfg.lexerClassName + ".h");
    auto classLexPath = path / std::string(cfg.lexerClassName + ".l");
    auto cmakePath = path / "CMakeLists.txt";
    try {
      std::filesystem::create_directories(path);
    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
      return 1;
    }
    int count = 0;
    if(!tokenHeaderCode.empty()) count += writeCode(tokenHeaderCode, tokenFilePath);
    if(!classHeaderCode.empty()) count += writeCode(classHeaderCode, classHeaderPath);
    if(!classLexCode.empty()) count += writeCode(classLexCode, classLexPath);
    if(!cmakeCode.empty()) count += writeCode(cmakeCode, cmakePath);
    if(count != 0){
      std::cerr << "[Error] Output code failed.\n";
    }
    return count;
  }else{
    if(!tokenHeaderCode.empty()) std::cout << tokenHeaderCode;
    if(!classHeaderCode.empty()) std::cout << classHeaderCode;
    if(!classLexCode.empty()) std::cout << classLexCode;
    if(!cmakeCode.empty()) std::cout << cmakeCode;
  }
  return 0;
}

int main(int argc, char* argv[]){
  std::string inputConfigFile;
  bool inputConfigFromStdin = false;
  std::string outputDir;
  std::string outputFileType;

  auto match = [](const char* str1, const char* str2){
    std::string a = toLower(str1);
    std::string b = toLower(str2);
    return a == b;
  };

  auto printError = [](const char* msg){
    std::cerr << msg << "\n";
    return 1;
  };

  if(argc == 1) return printUsage();
  for(int i = 1; i < argc; ++i){
    auto& arg = argv[i];
    if(match("-h", arg) || match("-help", arg) || match("--help", arg)){
      return printUsage();
    }else if(match("-?", arg) || match("--help-example", arg)){
      return printExample();
    }else if(match("-v", arg) || match("--version", arg)){
      return printVersion();
    }else if(match("-i", arg) || match("--input", arg)){
      if(i+1 < argc){
        inputConfigFile = argv[++i];
        continue;
      }else{
        return printError("Invalid parameters.");
      }
    }else if(match("-x", arg) || match("-", arg)){
      inputConfigFromStdin = true;
    }else if(match("-d", arg) || match("--dir", arg)){
      if(i+1 < argc){
        outputDir = argv[++i];
        continue;
      }else{
        return printError("Invalid parameters.");
      }
    }else if(match("-t", arg) || match("--type", arg)){
      if(i+1 < argc){
        outputFileType = argv[++i];
        continue;
      }else{
        return printError("Invalid parameters.");
      }
    }else{
      return printError("Invalid parameters.");
    }
  }

  if(inputConfigFile.empty() && inputConfigFromStdin == false){
    return printError("Please input a config file.");
  }

  auto cfg = readInput(inputConfigFile);
  if(cfg.lexerClassName.empty()) return -1;
  auto result = genCode(cfg, outputDir, outputFileType);
  std::cerr <<"[info] Generate code " << (result == 0? "success.":"failed.") <<"\n";

  return result;
}
