
#include <cctype>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

using namespace std;

// Lexer
// Class to hold the state, typically only one instance of it.
class Lex {
    
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


// [=========]
// | Parsing |
// [=========]

namespace parse {

// Base class for all expression AST nodes
class ExpAst {
public:
    virtual ~ExpAst() {}
    virtual void show(int indent) const {}
};

using UAst = unique_ptr<ExpAst>;

// Numeric literals
class NumberExpAst : public ExpAst {
    double num;
public:
    NumberExpAst(double numVal) : num(numVal) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Num " << num << endl;
    }
};

class VarExpAst : public ExpAst {
    string name;
public:
    VarExpAst(const string& nameVal) : name(nameVal) {}
    
    void show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Var " << name << endl;
    }
};

class BinOpExpAst : public ExpAst {
    char op;
    UAst left, right;
public:
    BinOpExpAst(char opVal, UAst ltree, UAst rtree) 
        : op(opVal), 
        left(move(ltree)), 
        right(move(rtree)) 
    {}
    
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

// TODO next: Fn Call Expr AST

} // end namespace "parse"


// Entry Point
// auto main(int argc, char* argv[]) -> int {
auto main() -> int {
    // This project initially based on:
    // https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html
    cout << "Welcome to my Kaleidoscope tutorial compiler!" << endl;
    
    auto lex = Lex{};
    
    return 0;
}