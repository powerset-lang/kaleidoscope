#ifndef HH_UICMD
#define HH_UICMD


#include <ast.hh>

// Directives as returned by compiler to REPL.
struct UiCmd {
    // The types of UI command:
    enum UiCmdType {
        CSetPrompt,
        CExit,
        CParsed,
        CError,
        CEmpty,
    };
    struct SetPrompt {};
    struct Exit {};
    struct Parsed {};
    struct Error {};
    struct Empty {};
    
    // Data
    enum UiCmdType type {CEmpty};
    std::string valSetPrompt {};
    UAst valParsed {};
    std::string valError {};
    
    UiCmd(SetPrompt, std::string newPrompt) 
        : type(CSetPrompt), valSetPrompt(newPrompt) {}
    UiCmd(Exit) : type(CExit) {}
    UiCmd(Parsed, UAst uast) : type(CParsed), valParsed(move(uast)) {}
    UiCmd(Error, std::string errMsg) : type(CError), valError(errMsg) {}
    UiCmd(Empty) : type(CEmpty) {}
};


#endif // HH_UICMD
