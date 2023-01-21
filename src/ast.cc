
#include <iostream>

#include <ast.hh>

using std::cout;
using std::endl;


// Definitions of virtual 'show' for pretty-printing AST nodes

void NumberExpAst::show(int indent) const {
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "Num val: " << num << endl;
}

void VarExpAst::show(int indent) const {
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "Var name: " << name << endl;
}

void BinOpExpAst::show(int indent) const {
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "BinOp op: " << op << endl;
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "  lchild: \\" << endl;
    left->show(indent+4);
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "  rchild: \\" << endl;
    right->show(indent+4);
}

void CallExpAst::show(int indent) const {
    for (int i = 0; i < indent; i++) { cout << " "; }
    cout << "Call fn: " << name << ", args: \\" << endl;
    for (int ai = 0; auto&& a: args) {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "  arg " << ai << ": \\" << endl;
        a->show(indent+4);
        ai++;
    }
}

void PrototypeAst::show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "Proto fn: " << name << ", params: \\" << endl;
        for (int pi = 0; auto&& p: params) {
            for (int i = 0; i < indent; i++) { cout << " "; }
            cout << "  param " << pi << ": " << p << endl;
            pi++;
        }
    }

void FunctionAst::show(int indent) const {
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "FunDef proto: \\" << endl;
        proto->show(indent+4);
        for (int i = 0; i < indent; i++) { cout << " "; }
        cout << "  body: \\" << endl;
        body->show(indent+4);
    }



