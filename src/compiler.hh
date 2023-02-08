#ifndef HH_COMPILER
#define HH_COMPILER


#include <iostream>

#include <ast.hh>
#include <compilesettings.hh>
#include <syntax/parser.hh>


class Compiler {
public:
    CompileSettings cs {};
    // UAst ast {}; //?
    Parser parser = Parser(&cs); //?
    
    // UAst parse();
    Compiler() = default;
    Compiler(CompileSettings cs) : cs(cs) {parser = Parser(&cs);}
};


#endif // HH_COMPILER
