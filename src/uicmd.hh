#ifndef HH_UICMD
#define HH_UICMD


#include <variant>

#include <ast.hh>


struct CmdSetPrompt {std::string valSetPrompt;};
struct CmdExit {};
struct CmdParsed {UAst valParsed;};
struct CmdError {std::string valError;};
struct CmdEmpty {};

using UiCmd = std::variant< //DataUiCmd
    CmdSetPrompt,
    CmdExit,
    CmdParsed,
    CmdError,
    CmdEmpty
>;

// // Directives as returned by compiler to REPL.
// struct UiCmd {
    
// public:
//     UiCmd(DataUiCmd d) : data(move(d)) {}
    
// private:
//     DataUiCmd data;
// };


#endif // HH_UICMD
