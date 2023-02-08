#ifndef HH_SYNTAX_TOKEN
#define HH_SYNTAX_TOKEN


#include <cstdint>
#include <memory>
#include <string>
#include <variant>

using std::int32_t;


// Types

using Sstring = std::shared_ptr<std::string>;

struct TokEof {};
struct TokDef {};
struct TokExtern {};
struct TokIdent {Sstring val;};
struct TokNumber {double val;};
struct TokChr {int32_t val;};
using DataToken = std::variant<
    TokEof,
    TokDef,
    TokExtern,
    TokIdent,
    TokNumber,
    TokChr
>;

// Helper for visitors
template<class... Fs> 
struct Overload : Fs... { 
    template <class... Ts>
    Overload(Ts&&... ts) : Fs{std::forward<Ts>(ts)}... {}
    
    using Fs::operator()...; 
};
// deduction guide
template<class... Ts> 
Overload(Ts&&...) -> Overload<std::remove_reference_t<Ts>...>;


class Token {
public:
    // Copy constructors
    Token (Token const& t) = default;
    Token& operator = (Token const& t) = default;
    
    DataToken value() const {return data;};
    
    int32_t valIfChar() const;
    bool matchIfChar(int matchWith) const;
private:
    friend class Lexer;
    // Constructor
    Token(DataToken dt) : data(dt) {};
    // Data
    DataToken data;
};


#endif // HH_SYNTAX_TOKEN
