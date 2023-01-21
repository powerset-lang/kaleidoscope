#ifndef HH_COMPILER
#define HH_COMPILER


#include <iostream>

#include <ast.hh>
#include <compilesettings.hh>
#include <parser.hh>


class Compiler {
public:
    CompileSettings cs {};
    // UAst ast {}; //?
    Parse parser = Parse(&cs); //?
    
    // UAst parse();
    Compiler() = default;
    Compiler(CompileSettings cs) : cs(cs) {parser = Parse(&cs);}
};


#endif // HH_COMPILER
