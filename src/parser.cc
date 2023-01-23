
// #include <iostream>

#include <parser.hh>

// using std::cout;
using std::endl;


// ##[ Lexer ]##

int Parser::Lexer::gettok() {
    // Skip whitespace
    while (isspace(prevChar)) prevChar = std::cin.get();
    // Recognize kws and idents
    if (isalpha(prevChar)) {
        identVal = prevChar;
        while (isalnum((prevChar = std::cin.get()))) {
            identVal += prevChar;
        }
        if (identVal == "def") return TokDef;
        if (identVal == "extern") return TokExtern;
        return TokIdent;
    }
    // Recognize numbers
    std::string numStr;
    while (isdigit(prevChar) || prevChar == '.') {
        numStr += prevChar;
        prevChar = std::cin.get();
    }
    if (numStr.length() > 0) {
        numberVal = strtod(numStr.c_str(), 0);
        return TokNumber;
    }
    // Recognize comments
    if (prevChar == '#') {
        static auto done = 
            [&]{return (prevChar == EOF || prevChar == '\n');};
        
        while (not done()) prevChar = std::cin.get();
        if (prevChar != EOF) {
            return gettok(); // recursive call, try again on next line
        }
    }
    // Recognize EOF
    if (prevChar == EOF) return TokEof; //Don't step to next, leave EOF be
    // Default: ASCII value of character
    auto toTok = static_cast<Tok>(prevChar);
    prevChar = std::cin.get();
    return toTok;
}


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
    if (!isascii(curTok)) return -1;
    int tokPrec = binOpPrec[curTok]; //Todo?
    if (tokPrec <= 0) return -1;
    return tokPrec;
}

// ** Mutually Recursive Parser Functions **

UExpAst Parser::parseNumberExp() {
    auto result = std::make_unique<NumberExpAst>(lex.numberVal);
    getNextToken(); // consumes the number token
    return result; //Note std::move is redundant
}

UExpAst Parser::parseParenExp() {
    getNextToken(); // consume left paren
    auto e = parseExp();
    if (!e) return nullptr;
    if (curTok != ')') return logError("expected ')'");
    getNextToken(); // consume right paren
    return e;
}

// for both variable references and fn calls.
UExpAst Parser::parseIdentExp() {
    std::string name = lex.identVal;
    getNextToken(); // consume ident
    if (curTok != '(') return make_unique<VarExpAst>(name);
    // since cur tok is left paren, this is now an fn call.
    getNextToken(); // consume left paren
    std::vector<UExpAst> args;
    if (curTok != ')') {
        while (true) {
            if (auto arg = parseExp()) {
                args.push_back(move(arg));
            } else {
                return nullptr;
            }
            if (curTok == ')') break;
            if (curTok != ',') {
                return logError("Expected ')' or ',' in argument list.");
            }
            getNextToken();
        }
    }
    getNextToken(); // consume right paren
    return make_unique<CallExpAst>(name, move(args));
}

// determine which kind of "primary expression" this is.
UExpAst Parser::parsePrimary() {
    switch (curTok) {
    case Lexer::TokIdent:
        return parseIdentExp();
    case Lexer::TokNumber:
        return parseNumberExp();
    case '(':
        return parseParenExp();
    default:
        return logError("Unknown token when expecting an expression");
    }
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
        int binOp = curTok;
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
    if (curTok != Lexer::TokIdent) {
        return logErrorProto("Expected function name in prototype");
    }
    std::string name = lex.identVal;
    getNextToken(); // consume identifier
    if (curTok != '(') return logErrorProto("Expected '(' in prototype");
    getNextToken(); // consume left paren
    std::vector<std::string> argNames;
    while(getNextToken() == Lexer::TokIdent) {
        argNames.push_back(lex.identVal);
    }
    if (curTok != ')') return logErrorProto("Expected ')' in prototype");
    getNextToken(); // consume right paren
    return make_unique<PrototypeAst>(name, move(argNames));
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
    switch (curTok) {
    case Lexer::TokEof:
        return UiCmd(UiCmd::Exit{});
    case ';':
        getNextToken(); // skip semicolons
        return UiCmd(UiCmd::Empty{});
    case Lexer::TokDef:
        if (auto ua = replParseDef()) {
            return UiCmd(UiCmd::Parsed{}, move(ua.value()));
        }
        return UiCmd(UiCmd::Error{}, "Failed to parse a function definition.");
    case Lexer::TokExtern:
        if (auto ua = replParseExtern()) {
            return UiCmd(UiCmd::Parsed{}, move(ua.value()));
        }
        return UiCmd(UiCmd::Error{}, 
                    "Failed to parse an extern function declaration.");
    default:
        if (auto ua = replParseExp()) {
            return UiCmd(UiCmd::Parsed{}, move(ua.value()));
        }
        return UiCmd(UiCmd::Error{}, "Failed to parse an expression.");
    }
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



