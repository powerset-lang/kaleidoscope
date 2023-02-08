#ifndef HH_SYNTAX_LEXER
#define HH_SYNTAX_LEXER


#include <iostream>
#include <optional>

#include <syntax/token.hh>


class Lexer {
public:
    explicit Lexer (std::istream* ist) : ist(ist), curr(TokChr{' '}) {}
    Token get ();
    Token current () const {return curr;};
private:
    // Data
    std::istream* ist;
    Token curr;
    int32_t prevChr {' '}; //todo needed?
};


#endif // HH_SYNTAX_LEXER
