// Launch the UI with cmd line arguments.

#include <string_view>
#include <queue>

#include <ui.hh>


// Entry Point
auto main(int argc, char* argv[]) -> int {
    // std::cout << "Welcome to the Kaleidoscope compiler!" << std::endl;
    
    std::queue<std::string_view> args {};
    for (int i = 0; i < argc; i++) {
        args.push(argv[i]);
    }
    
    auto ui = Ui(args);
    return ui.run();
}
