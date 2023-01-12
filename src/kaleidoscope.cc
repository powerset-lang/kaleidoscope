
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

using namespace std;



// ##[ Lexer ]##

// Class to hold the state, typically one instance of it per parser.
class Lex {
public:
    enum Tok : int {
        TokEof = -1,
        // Commands
        TokDef = -2,
        TokExtern = -3,
        // primary (?)
        TokIdent = -4,
        TokNumber = -5,
        // All other toks are their (positive) ASCII value.
    };
    
    // Token value data storage
public:
    string identVal {}; // buffer for an ident tok
    double numberVal {0}; 
    
    // State for gettok()
private:
    int prevChar {' '};
    
    // Member Functions
    // ****************
public:
    int gettok() {
        // Skip whitespace
        while (isspace(prevChar)) prevChar = getchar();
        // Recognize kws and idents
        if (isalpha(prevChar)) {
            identVal = prevChar;
            while (isalnum((prevChar = getchar()))) {
                identVal += prevChar;
            }
            if (identVal == "def") return TokDef;
            if (identVal == "extern") return TokExtern;
            return TokIdent;
        }
        // Recognize numbers
        string numStr;
        while (isdigit(prevChar) || prevChar == '.') {
            numStr += prevChar;
            prevChar = getchar();
        }
        if (numStr.length() > 0) {
            numberVal = strtod(numStr.c_str(), 0);
            return TokNumber;
        }
        // Recognize comments
        if (prevChar == '#') {
            static auto done = \
                [&]{return (prevChar == EOF || prevChar == '\n');};
            
            while (not done()) prevChar = getchar();
            if (prevChar != EOF) {
                return gettok(); // recursive call, try again on next line
            }
        }
        // Recognize EOF
        if (prevChar == EOF) return TokEof; //Don't step to next, leave EOF be
        // Default: ASCII value of character
        auto toTok = static_cast<Tok>(prevChar);
        prevChar = getchar();
        return toTok;
    }
    
    
};




// [=================]
// # Parsing and AST #
// [=================]


// ##[ AST Nodes ]##

// Shared behavior for all kinds of AST nodes
class Ast {
public:
    virtual void show(int indent) const = 0;
};

// ** Expressions **

// Base class for all expression AST nodes
class ExpAst : public Ast {
public:
    virtual ~ExpAst() {}
    // virtual void show(int indent) const {}
    // Todo next: add code gen virtual method
};

using UExpAst = unique_ptr<ExpAst>;


// Numeric literals
class NumberExpAst : public ExpAst {
    double num;
public:
    NumberExpAst(double numVal) : num(numVal) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Num val: " << num << endl;
    }
};

class VarExpAst : public ExpAst {
    string name;
public:
    VarExpAst(const string& nameVal) : name(nameVal) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Var name: " << name << endl;
    }
};

class BinOpExpAst : public ExpAst {
    char op;
    UExpAst left, right;
public:
    BinOpExpAst(char opVal, UExpAst ltree, UExpAst rtree) 
        : op(opVal), left(move(ltree)), right(move(rtree)) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "BinOp op: " << op << endl;
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "  lchild: \\" << endl;
        left->show(indent+4);
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "  rchild: \\" << endl;
        right->show(indent+4);
    }
};

class CallExpAst : public ExpAst {
    string name;
    vector<UExpAst> args;
public:
    CallExpAst(const string& name, vector<UExpAst> args)
        : name(name), args(move(args)) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Call fn: " << name << ", args: \\" << endl;
        for (int ai = 0; auto&& a: args) {
            for (int i = 0; i < indent; i++) { cout << " "; }
            cout << "  arg " << ai << ": \\" << endl;
            a->show(indent+4);
            ai++;
        }
    }
};


// ** Function declarations & definitions **

// Function Prototypes / Signatures, with fn name and arg names
class PrototypeAst : public Ast {
    string name;
    vector<string> params;
public:
    PrototypeAst(const string& name, vector<string> params)
        : name(name), params(move(params)) {}
    
    const string& getName() const { return name; }
    
    virtual void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Proto fn: " << name << ", params: \\" << endl;
        for (int pi = 0; auto&& p: params) {
            for (int i = 0; i < indent; i++) { cout << " "; }
            cout << "  param " << pi << ": " << p << endl;
            pi++;
        }
    }
};

using UProtoAst = unique_ptr<PrototypeAst>;


class FunctionAst : public Ast {
    UProtoAst proto;
    UExpAst body;
public:
    FunctionAst(UProtoAst proto, UExpAst body) 
        : proto(move(proto)), body(move(body)) {}
    
    virtual void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "FunDef proto: \\" << endl;
        proto->show(indent+4);
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "  body: \\" << endl;
        body->show(indent+4);
    }
};

using UFunAst = unique_ptr<FunctionAst>;


// ##[ Parser ]##

class Parse {
    
    Lex lex {};
    
    int curTok; // Should be an enum Tok?
    
    int getNextToken() {
        return curTok = lex.gettok();
    }
    
    // Helpers for error logging
    static 
    UExpAst logError(const char* str) {
        fprintf(stderr, "LogError: %s\n", str);
        return nullptr; 
    }
    static
    UProtoAst logErrorP(const char* str) {
        logError(str);
        return nullptr;
    }
    
    // ** Mutually Recursive Parser Functions **
    
    UExpAst parseNumberExp() {
        auto result = make_unique<NumberExpAst>(lex.numberVal);
        getNextToken(); // consumes the number token
        return result; //Note std::move is redundant
    }
    
    UExpAst parseParenExp() {
        getNextToken(); // consume left paren
        auto e = parseExp();
        if (!e) return nullptr;
        if (curTok != ')') return logError("expected ')'");
        getNextToken(); // consume right paren
        return e;
    }
    
    // for both variable references and fn calls.
    UExpAst parseIdentExp() {
        string name = lex.identVal;
        getNextToken(); // consume ident
        if (curTok != '(') return make_unique<VarExpAst>(name);
        // since cur tok is left paren, this is now an fn call.
        getNextToken(); // consume left paren
        vector<UExpAst> args;
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
    UExpAst parsePrimary() {
        switch (curTok) {
        case Lex::TokIdent:
            return parseIdentExp();
        case Lex::TokNumber:
            return parseNumberExp();
        case '(':
            return parseParenExp();
        default:
            return logError("Unknown token when expecting an expression");
        }
    }
    
    // * Binary Operators via Operator Precedence Parser *
    
    // precedences
private:
    unordered_map<char, int> binOpPrec { // Todo: make static?
        {'<', 10}, 
        {'+', 20},
        {'-', 20},
        {'*', 40}
    };
    
    // return precedence of curTok, if applicable, else -1
    int getTokPrec() {
        if (!isascii(curTok)) return -1;
        int tokPrec = binOpPrec[curTok]; //Todo?
        if (tokPrec <= 0) return -1;
        return tokPrec;
    }
    
    UExpAst parseExp() {
        auto lhs = parsePrimary();
        if (!lhs) return nullptr;
        return parseBinOpRhs(0, move(lhs));
    }
    
    UExpAst parseBinOpRhs(int expPrec, UExpAst lhs) {
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
    
    UProtoAst parseProto() {
        if (curTok != Lex::TokIdent) {
            return logErrorP("Expected function name in prototype");
        }
        string name = lex.identVal;
        getNextToken(); // consume identifier
        if (curTok != '(') return logErrorP("Expected '(' in prototype");
        getNextToken(); // consume left paren
        vector<string> argNames;
        while(getNextToken() == Lex::TokIdent) {
            argNames.push_back(lex.identVal);
        }
        if (curTok != ')') return logErrorP("Expected ')' in prototype");
        getNextToken(); // consume right paren
        return make_unique<PrototypeAst>(name, move(argNames));
    }
    
    // Function definitions are 'def', a proto, then an exp as the body.
    UFunAst parseFunDef() {
        getNextToken(); // consume 'def'
        auto proto = parseProto();
        if (not proto) return nullptr;
        if (auto exp = parseExp()) {
            return make_unique<FunctionAst>(move(proto), move(exp));
        }
        return nullptr;
    }
    
    // Extern function declarations are 'extern' plus a proto.
    UProtoAst parseExternFun() {
        getNextToken(); // consume 'extern'
        return parseProto();
    }
    
    
    // ** REPL for interactive parsing experimentation **
// Todo: make this a separate class!
    
public:
    void repl() {
        while (true) {
            cout << "ready> ";
            getNextToken();
            switch (curTok) {
            case Lex::TokEof:
                cout << "Goodbye!" << endl;
                return;
            case ';':
                getNextToken(); // skip semicolons
                break;
            case Lex::TokDef:
                replDef();
                break;
            case Lex::TokExtern:
                replExtern();
                break;
            default:
                replExp();
                break;
            }
        }
    }
    
private:
    void replDef() {
        if (auto f = parseFunDef()) {
            cout << "Parsed a function definition." << endl;
            f->show(2);
        } else {
            getNextToken(); // skip tok for error recovery
        }
    }
    
    void replExtern() {
        if (auto e = parseExternFun()) {
            cout << "Parsed an extern function declaration." << endl;
            e->show(2);
        } else {
            getNextToken(); // skip tok for error recovery
        }
    }
    
    void replExp() {
        if (auto e = parseExp()) {
            cout << "Parsed a top-level expression." << endl;
            e->show(2);
        } else {
            getNextToken(); // skip tok for error recovery
        }
    }
    
};






// Entry Point
// auto main(int argc, char* argv[]) -> int {
auto main() -> int {
    // This project initially based on:
    // https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
    cout << "Welcome to my Kaleidoscope tutorial compiler!" << endl;
    
    auto parser = Parse{};
    
    parser.repl();
    
    return 0;
}