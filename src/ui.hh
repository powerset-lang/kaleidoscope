#ifndef HH_UI
#define HH_UI


#include <string>
#include <string_view>
#include <queue>

#include <compiler.hh>
#include <uicmd.hh>


// Store state for UI code. Contains any UI settings.
class [[nodiscard]] Ui {
    Compiler co; // todo should there even be a single default compiler obj?
    
    bool useRepl {true}; //?
    
    std::string promptStr {"repl> "};
    
    
    std::ostream& prompt() {
        return *co.cs.out << promptStr;
    }
    
    std::ostream& greet() {
        return *co.cs.out << "Welcome to the Kaleidoscope REPL!" << std::endl;
    }
    int farewell() {
        *co.cs.out << "Goodbye!" << std::endl;
        return 0;
    }
    
    // ** REPL **
    int repl();
    // int replDef();
    // int replExtern();
    // int replExp();
    
public:
    // Parse command-line arguments.
    Ui(std::queue<std::string_view> args);
    int run();
};


#endif // HH_UI
