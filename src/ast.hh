#ifndef HH_AST
#define HH_AST


#include <memory>
#include <optional>
#include <string>
#include <vector>


// ##[ AST Nodes ]##

// Shared behavior for all kinds of AST nodes
class Ast {
public:
    virtual void show(int indent) const = 0; //=0 ?
    virtual std::string getTypeDesc() const = 0;
};

using UAst = std::unique_ptr<Ast>;
using OUAst = std::optional<UAst>;


// ** Expressions **

// Base class for all expression AST nodes
class ExpAst : public Ast {
public:
    virtual ~ExpAst() {};
    // virtual Value* codegen() = 0;
    // Todo: should codegen be in the Ast common base class?
    std::string getTypeDesc() const {return "Expression";}
};

using UExpAst = std::unique_ptr<ExpAst>;
using OUExpAst = std::optional<UExpAst>;


// Numeric literals
class NumberExpAst : public ExpAst {
    double num;
public:
    NumberExpAst(double numVal) : num(numVal) {}
    
    void show(int indent) const; //todo needed?
};

class VarExpAst : public ExpAst {
    std::string name;
public:
    VarExpAst(const std::string& nameVal) : name(nameVal) {}
    
    void show(int indent) const; //?
};

class BinOpExpAst : public ExpAst {
    char op;
    UExpAst left;
    UExpAst right;
public:
    BinOpExpAst(char opVal, UExpAst ltree, UExpAst rtree) 
        : op(opVal), left(move(ltree)), right(move(rtree)) {}
    
    void show(int indent) const; //?
};

class CallExpAst : public ExpAst {
    std::string name;
    std::vector<UExpAst> args;
public:
    CallExpAst(const std::string& name, std::vector<UExpAst> args)
        : name(name), args(move(args)) {}
    
    void show(int indent) const; //?
};


// ** Function declarations & definitions **

// Function Prototypes / Signatures, with fn name and arg names
class PrototypeAst : public Ast {
    std::string name;
    std::vector<std::string> params;
public:
    PrototypeAst(const std::string& name, std::vector<std::string> params)
        : name(name), params(move(params)) {}
    
    const std::string& getName() const { return name; }
    
    virtual void show(int indent) const; //?
    std::string getTypeDesc() const {return "Function Prototype";}
};

using UProtoAst = std::unique_ptr<PrototypeAst>;
using OUProtoAst = std::optional<UProtoAst>;


class FunctionAst : public Ast {
    UProtoAst proto;
    UExpAst body;
public:
    FunctionAst(UProtoAst proto, UExpAst body) 
        : proto(move(proto)), body(move(body)) {}
    
    virtual void show(int indent) const; //?
    std::string getTypeDesc() const {return "Function Definition";}
};

using UFunAst = std::unique_ptr<FunctionAst>;
using OUFunAst = std::optional<UFunAst>;


#endif // HH_AST
