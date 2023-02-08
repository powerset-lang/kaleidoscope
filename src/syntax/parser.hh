#ifndef HH_SYNTAX_PARSER
#define HH_SYNTAX_PARSER


#include <array>
#include <list>
#include <tuple>
#include <unordered_map>

#include <ast.hh>
#include <syntax/lexer.hh>
#include <syntax/token.hh>
#include <compilesettings.hh>
#include <uicmd.hh>


// ##[ Parser ]##

class Parser {
    
    // // ##[ Lexer ]##
    // // One instance of it per parser.
    // class Lexer {
    // public:
    //     enum Tok : int {
    //         TokEof = -1,
    //         TokDef = -2,
    //         TokExtern = -3,
    //         TokIdent = -4,
    //         TokNumber = -5,
    //         // All other toks are their (positive) ASCII value.
    //     };
        
    //     // Token value data storage
    // public:
    //     std::string identVal {}; // buffer for an ident tok
    //     double numberVal {0}; 
        
    // private:
    //     int prevChar {' '}; // State for gettok()
    // public:
    //     int gettok();
    // };
    
    // // Data
    const CompileSettings* cs;
    Lexer lex;// = Lexer(*cs->in);
    Token curTok;
    // std::unordered_map<char, int> binOpPrec {};
    static constexpr
    std::array binOpPrec {
        std::tuple {'<',10}, 
        std::tuple {'+',20},
        std::tuple {'-',20},
        std::tuple {'*',40}
    };
    
public:
    Parser(const CompileSettings* cs) 
    : cs(cs), lex(Lexer(cs->in)), curTok(lex.current()) {
        // cs = csp;
        // If there are parser specific settings, perhaps there should be a 
        //   parser settings class? or should parser hold a ptr to compiler? 
        // Set operator precedences.
        // binOpPrec['<'] = 10;
        // binOpPrec['+'] = 20;
        // binOpPrec['-'] = 20;
        // binOpPrec['*'] = 40;
        // cs = cs;
        // lex = Lexer(cs->in);
        // curTok = lex.current();
    }
private:
    
    // Helpers for error logging
    // static? 
    UExpAst logError(const char* str);
    // static?
    UProtoAst logErrorProto(const char* str);
    
    // Updates curTok.
    Token getNextToken() { return curTok = lex.get(); }
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


#endif // HH_SYNTAX_PARSER
