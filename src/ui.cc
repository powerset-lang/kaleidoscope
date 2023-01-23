
// Contains all user interaction code, both REPL and command line arguments.
// This module creates & manages Compiler objects.

#include <ui.hh>

// using std::endl;


Ui::Ui (std::queue<std::string_view> args) {
    co = Compiler(); // sets defaults
    auto programName = args.front(); // do nothing with it for now
    if (args.size() == 0) {
        useRepl = true;
        return; // no params, go to repl.
    }
    auto mode = args.front(); // First argument is the compile mode.
    // if (mode == "l" || mode == "lex") cs.compileMode = CompileMode::Lex;
    // else 
    if (mode == "p" || mode == "parse") { 
        co.cs.mode = CompileMode::Parse; 
    } else if (mode == "c" || mode == "compile") {
        co.cs.mode = CompileMode::Compile;
    }
}

int Ui::run() {
    if (useRepl == true) {
        return repl();
    } else {
        *co.cs.err << "Only the REPL works currently";
        return 1;
    }
}


// ** REPL **

int Ui::repl() {
    greet();
    while (true) {
        using enum UiCmd::UiCmdType;
        
        auto s = &prompt();
        auto cmd = co.parser.replParse();
        
        switch (cmd.type) {
        case CSetPrompt:
            promptStr = cmd.valSetPrompt;
            break;
        case CExit:
            return farewell();
        case CParsed:
            *s << "Parsed a: " << cmd.valParsed->getTypeDesc() <<"."<<std::endl;
            cmd.valParsed->show(2);
            break;
        default:
            cmd.type = CError;
            cmd.valError = "Unmatched UI Command type";
            [[fallthrough]];
        case CError:
            *s << cmd.valError << std::endl;
            return 1;
        case CEmpty:
            break;
        }
    }
}

// int Ui::replDef() {
//     if (auto f = parseFunDef()) {
//         cout << "Parsed a function definition." << endl;
//         f->show(2);
//     } else {
//         getNextToken(); // skip tok for error recovery
//     }
// }

// int Ui::replExtern() {
//     if (auto e = parseExternFun()) {
//         cout << "Parsed an extern function declaration." << endl;
//         e->show(2);
//     } else {
//         getNextToken(); // skip tok for error recovery
//     }
// }

// int Ui::replExp() {
//     if (auto e = parseExp()) {
//         cout << "Parsed a top-level expression." << endl;
//         e->show(2);
//     } else {
//         getNextToken(); // skip tok for error recovery
//     }
// }


