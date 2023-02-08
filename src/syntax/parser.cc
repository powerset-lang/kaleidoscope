
// #include <iostream>

#include "syntax/token.hh"
#include "uicmd.hh"
#include <syntax/parser.hh>

// using std::cout;
using std::endl;


// ##[ Lexer ]##

// int Parser::Lexer::gettok() {
//     // Skip whitespace
//     while (isspace(prevChar)) prevChar = std::cin.get();
//     // Recognize kws and idents
//     if (isalpha(prevChar)) {
//         identVal = prevChar;
//         while (isalnum((prevChar = std::cin.get()))) {
//             identVal += prevChar;
//         }
//         if (identVal == "def") return TokDef;
//         if (identVal == "extern") return TokExtern;
//         return TokIdent;
//     }
//     // Recognize numbers
//     std::string numStr;
//     while (isdigit(prevChar) || prevChar == '.') {
//         numStr += prevChar;
//         prevChar = std::cin.get();
//     }
//     if (numStr.length() > 0) {
//         numberVal = strtod(numStr.c_str(), 0);
//         return TokNumber;
//     }
//     // Recognize comments
//     if (prevChar == '#') {
//         static auto done = 
//             [&]{return (prevChar == EOF || prevChar == '\n');};
        
//         while (not done()) prevChar = std::cin.get();
//         if (prevChar != EOF) {
//             return gettok(); // recursive call, try again on next line
//         }
//     }
//     // Recognize EOF
//     if (prevChar == EOF) return TokEof; //Don't step to next, leave EOF be
//     // Default: ASCII value of character
//     auto toTok = static_cast<Tok>(prevChar);
//     prevChar = std::cin.get();
//     return toTok;
// }


// ##[ Parser ]##

// Helpers for error logging
UExpAst Parser::logError(const char* str) {
    *cs->err << str << endl;
    return nullptr; 
}
UProtoAst Parser::logErrorProto(const char* str) {
    logError(str);
    return nullptr;
}

// int Parse::getNextToken();

// return precedence of curTok, if applicable, else -1
int Parser::getTokPrec() {
    auto dt = curTok.value();
    if(auto* c = std::get_if<TokChr>(&dt)) {
        if (!isascii(c->val)) return -1;
        for (auto b : binOpPrec) {
            if (std::get<char>(b) == c->val) return std::get<int>(b);
        }
    }
    return -1;
    // int tokPrec = binOpPrec[curTok]; //Todo?
    // if (tokPrec <= 0) return -1;
    // return tokPrec;
}

// ** Mutually Recursive Parser Functions **

UExpAst Parser::parseNumberExp() {
    auto num = std::get<TokNumber>(curTok.value()); // Throws if not number
    auto result = std::make_unique<NumberExpAst>(num.val);
    getNextToken(); // consumes the number token
    return result; //Note std::move is redundant
}

UExpAst Parser::parseParenExp() {
    getNextToken(); // consume left paren
    auto e = parseExp();
    if (!e) return nullptr;
    auto dt = curTok.value();
    if (auto* rpar = std::get_if<TokChr>(&dt)) {
        if (rpar->val != ')') return logError("expected ')'");
        getNextToken(); // consume right paren
        return e;
    }
    return logError("expected ')'");
}

// for both variable references and fn calls.
UExpAst Parser::parseIdentExp() {
    // Throw if not ident
    auto name = std::get<TokIdent>(curTok.value()).val;
    getNextToken(); // consume ident
    if (!curTok.matchIfChar('(')) return make_unique<VarExpAst>(*name);
    // since cur tok is left paren, this is now an fn call.
    getNextToken(); // consume left paren
    std::vector<UExpAst> args;
    if (!curTok.matchIfChar(')')) {
        while (true) {
            if (auto arg = parseExp()) {
                args.push_back(move(arg));
            } else {
                return nullptr;
            }
            if (curTok.matchIfChar(')')) break;
            if (!curTok.matchIfChar(',')) {
                return logError("Expected ')' or ',' in argument list.");
            }
            getNextToken();
        }
    }
    getNextToken(); // consume right paren
    return make_unique<CallExpAst>(*name, move(args));
}

// determine which kind of "primary expression" this is.
UExpAst Parser::parsePrimary() {
    return std::visit<UExpAst>(Overload{
        
        [this](TokIdent& t) {return parseIdentExp();},
        [this](TokNumber& t) {return parseNumberExp();},
        [this](TokChr& t) {
            if (t.val == '(') {
                return parseParenExp();
            } else {
                return logError("Unknown token when expecting an expression");
            }
        },
        // [this](TokEof& t) {
        //     return logError("Unknown token when expecting an expression");
        // },
        // [this](TokDef& t) {
        //     return logError("Unknown token when expecting an expression");
        // },
        // [this](TokExtern& t) {
        //     return logError("Unknown token when expecting an expression");
        // },
        [this](auto&& other) {
            return logError("Unknown token when expecting an expression");
        }
        
    }, curTok.value());
    
    // switch (curTok) {
    // case Lexer::TokIdent:
    //     return parseIdentExp();
    // case Lexer::TokNumber:
    //     return parseNumberExp();
    // case '(':
    //     return parseParenExp();
    // default:
    //     return logError("Unknown token when expecting an expression");
    // }
}

UExpAst Parser::parseExp() {
    auto lhs = parsePrimary();
    if (!lhs) return nullptr;
    return parseBinOpRhs(0, move(lhs));
}

// * Binary Operators via Operator Precedence Parser *

UExpAst Parser::parseBinOpRhs(int expPrec, UExpAst lhs) {
    while (true) {
        int tokPrec = getTokPrec();
        if (tokPrec < expPrec) return lhs;
        int binOp = curTok.valIfChar();
        getNextToken(); // consume operator
        auto rhs = parsePrimary();
        if (!rhs) return nullptr;
        int nextPrec = getTokPrec();
        if (tokPrec < nextPrec) {
            rhs = parseBinOpRhs(tokPrec+1, move(rhs));
            if (!rhs) return nullptr;
        }
        // Merge the lhs and rhs.
        lhs = make_unique<BinOpExpAst>(binOp, move(lhs), move(rhs));
    }
}

// * Function declarations and definitions *

UProtoAst Parser::parseProto() {
    auto dt = curTok.value();
    if (auto* t = std::get_if<TokIdent>(&dt)) {
        std::string name = *t->val;
        getNextToken(); // consume identifier
        if (curTok.matchIfChar('(')) {
            getNextToken(); // consume left paren
            std::vector<std::string> argNames;
            while(true) {
                auto tok = getNextToken();
                auto dtok = tok.value();
                if (auto* id = std::get_if<TokIdent>(&dtok)) {
                    argNames.push_back(*id->val);
                    continue;
                } else {
                    break;
                }
            }
            if (curTok.matchIfChar(')')) {
                getNextToken(); // consume right paren
                return make_unique<PrototypeAst>(name, move(argNames));
            } else {
                return logErrorProto("Expected ')' in prototype"); 
            }
        } else {
            return logErrorProto("Expected '(' in prototype");
        }
    } else {
        return logErrorProto("Expected function name in prototype");
    }
    
    // std::string name = dt.val;
    // getNextToken(); // consume identifier
    // if (curTok != '(') return logErrorProto("Expected '(' in prototype");
    // getNextToken(); // consume left paren
    // std::vector<std::string> argNames;
    // while(getNextToken() == Lexer::TokIdent) {
    //     argNames.push_back(lex.identVal);
    // }
    // if (curTok != ')') return logErrorProto("Expected ')' in prototype");
    // getNextToken(); // consume right paren
    // return make_unique<PrototypeAst>(name, move(argNames));
}

// Extern function declarations are 'extern' plus a proto.
UProtoAst Parser::parseExternFun() {
    getNextToken(); // consume 'extern'
    return parseProto();
}

// Function definitions are 'def', a proto, then an exp as the body.
UFunAst Parser::parseFunDef() {
    getNextToken(); // consume 'def'
    auto proto = parseProto();
    if (not proto) return nullptr;
    if (auto exp = parseExp()) {
        return make_unique<FunctionAst>(move(proto), move(exp));
    }
    return nullptr;
}


// ** Support REPL parsing **

UiCmd Parser::replParse [[nodiscard]] () {
    getNextToken(); //?
    
    auto datatok = curTok.value();
    
    return std::visit<UiCmd>(Overload{
        
        [this](TokEof& t) {return UiCmd{CmdExit{}};},
        [this](TokDef& t) {
            if (auto ua = replParseDef()) {
                return UiCmd{CmdParsed{move(ua.value())}};
            }
            return UiCmd{CmdError{"Failed to parse a function definition."}};
        },
        [this](TokExtern& t) {
            if (auto ua = replParseExtern()) {
                return UiCmd{CmdParsed{move(ua.value())}};
            }
            return UiCmd(
                CmdError{"Failed to parse an extern function declaration."}
            );
        },
        [this](DataToken& dt) {
            
            if (auto* chr = std::get_if<TokChr>(&dt)) {
                if (chr->val == ';') {
                    return UiCmd{CmdEmpty{}};
                }
            }
            
            if (auto ua = replParseExp()) {
                return UiCmd{CmdParsed{move(ua.value())}};
            }
            return UiCmd{CmdError{"Failed to parse an expression."}};
        },
        [this](auto&& other) {
            return UiCmd{CmdError{"Unknown error while parsing in REPL."}};
        }
        
    }, datatok);
    
    // switch (curTok) {
    // case Lexer::TokEof:
    //     return UiCmd(UiCmd::Exit{});
    // case ';':
    //     getNextToken(); // skip semicolons
    //     return UiCmd(UiCmd::Empty{});
    // case Lexer::TokDef:
    //     if (auto ua = replParseDef()) {
    //         return UiCmd(UiCmd::Parsed{}, move(ua.value()));
    //     }
    //     return UiCmd(UiCmd::Error{}, "Failed to parse a function definition.");
    // case Lexer::TokExtern:
    //     if (auto ua = replParseExtern()) {
    //         return UiCmd(UiCmd::Parsed{}, move(ua.value()));
    //     }
    //     return UiCmd(UiCmd::Error{}, 
    //                 "Failed to parse an extern function declaration.");
    // default:
    //     if (auto ua = replParseExp()) {
    //         return UiCmd(UiCmd::Parsed{}, move(ua.value()));
    //     }
    //     return UiCmd(UiCmd::Error{}, "Failed to parse an expression.");
    // }
}

OUFunAst Parser::replParseDef [[nodiscard]] () {
    if (UFunAst f = parseFunDef()) {
        // cout << "Parsed a function definition." << endl;
        // f->show(2);
        return f;
    } else {
        getNextToken(); // skip tok for error recovery
        return std::nullopt;
    }
}

OUProtoAst Parser::replParseExtern [[nodiscard]] () {
    if (UProtoAst e = parseExternFun()) {
        // cout << "Parsed an extern function declaration." << endl;
        // e->show(2);
        return e;
    } else {
        getNextToken(); // skip tok for error recovery
        return std::nullopt;
    }
}

OUExpAst Parser::replParseExp [[nodiscard]] () {
    if (UExpAst e = parseExp()) {
        // cout << "Parsed a top-level expression." << endl;
        // e->show(2);
        return e;
    } else {
        getNextToken(); // skip tok for error recovery
        return std::nullopt;
    }
}



