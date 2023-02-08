
#include <syntax/token.hh>


int32_t Token::valIfChar() const {
    if (auto* c = std::get_if<TokChr>(&this->data)) {
        return c->val;
    }
    return 0;
}

bool Token::matchIfChar(int matchWith) const {
    if (auto c = valIfChar()) {
        return c == matchWith;
    }
    return false;
}
