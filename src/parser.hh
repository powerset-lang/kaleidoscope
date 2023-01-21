#ifndef HH_PARSER
#define HH_PARSER


#include <list>
#include <unordered_map>

#include <ast.hh>
#include <compilesettings.hh>
#include <uicmd.hh>


// ##[ Parser ]##

class Parse {
    
    // ##[ Lexer ]##
    // One instance of it per parser.
    class Lex {
    public:
        enum Tok : int {
            TokEof = -1,
            TokDef = -2,
            TokExtern = -3,
            TokIdent = -4,
            TokNumber = -5,
            // All other toks are their (positive) ASCII value.
        };
        
        // Token value data storage
    public:
        std::string identVal {}; // buffer for an ident tok
        double numberVal {0}; 
        
    private:
        int prevChar {' '}; // State for gettok()
    public:
        int gettok();
    };
    
    // Data
    Lex lex {};
    int curTok {}; 
    std::unordered_map<char, int> binOpPrec {};
    const CompileSettings* cs;
    
public:
    Parse(const CompileSettings* cs) : cs(cs) {
        // cs = csp;
        // If there are parser specific settings, perhaps there should be a 
        //   parser settings class? or should parser hold a ptr to compiler? 
        // Set operator precedences.
        binOpPrec['<'] = 10;
        binOpPrec['+'] = 20;
        binOpPrec['-'] = 20;
        binOpPrec['*'] = 40;
    }
private:
    
    // Helpers for error logging
    // static? 
    UExpAst logError(const char* str);
    // static?
    UProtoAst logErrorProto(const char* str);
    
    // Updates curTok.
    int getNextToken() { return curTok = lex.gettok(); }
    // return precedence of curTok, if applicable, else -1
    int getTokPrec();
    
    // ** Mutually Recursive Parser Functions **
    UExpAst parseNumberExp();
    UExpAst parseParenExp();
    // for both variable references and fn calls.
    UExpAst parseIdentExp();
    // determine which kind of "primary expression" this is.
    UExpAst parsePrimary();
    UExpAst parseExp();
    // * Binary Operators via Operator Precedence Parser *
    UExpAst parseBinOpRhs(int expPrec, UExpAst lhs);
    // * Function declarations and definitions *
    UProtoAst parseProto();
    UProtoAst parseExternFun();
    UFunAst parseFunDef();
    
    // ** Support REPL parsing **
public:
    UiCmd replParse [[nodiscard]] ();
private:
    OUFunAst replParseDef [[nodiscard]] ();
    OUProtoAst replParseExtern [[nodiscard]] ();
    OUExpAst replParseExp [[nodiscard]] ();
};


#endif // HH_PARSER
