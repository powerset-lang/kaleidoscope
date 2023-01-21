#ifndef HH_COMPILESETTINGS
#define HH_COMPILESETTINGS


#include <iostream>


enum class CompileMode {
    // Lex,
    Parse,
    Compile, //todo codegen, obj, exe?
};

struct CompileSettings {
    CompileMode mode {CompileMode::Compile};
    std::istream* in {&std::cin};
    std::ostream* out {&std::cout};
    std::ostream* err {&std::cerr};
};


#endif // HH_COMPILESETTINGS
