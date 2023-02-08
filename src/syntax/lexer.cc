
#include <cctype>

#include <syntax/token.hh>
#include <syntax/lexer.hh>


Token Lexer::get() {
    
    // int32_t prevChr = ist.get();

    // Skip whitespace
    while (isspace(prevChr)) prevChr = ist->get();
    // Recognize kws and idents
    if (isalpha(prevChr)) {
        Sstring identVal = std::make_shared<std::string>("");
        do {
            *identVal += prevChr;
        } while (isalnum ((prevChr = ist->get ())));
        
        if (*identVal == "def") return Token (TokDef {});
        if (*identVal == "extern") return Token (TokExtern {});
        return Token (TokIdent {identVal});
    }
    // Recognize numbers
    std::string numStr {};
    while (isdigit(prevChr) || prevChr == '.') {
        numStr += prevChr;
        prevChr = ist->get();
    }
    if (numStr.length() > 0) {
        auto numberVal = strtod(numStr.c_str(), 0);
        return Token (TokNumber {numberVal});
    }
    // Recognize comments
    if (prevChr == '#') {
        while (prevChr != EOF && prevChr != '\n') prevChr = ist->get();
        if (prevChr != EOF) {
            return get(); // recursive call, start again on the next line
        }
    }
    // Recognize EOF
    if (prevChr == EOF) return Token (TokEof {}); //Don't step, leave EOF
    // Default: ASCII value of character
    auto toTok = Token (TokChr {prevChr});
    prevChr = ist->get();
    return toTok;

}

