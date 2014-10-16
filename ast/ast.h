#ifndef __AST_AST__
#define __AST_AST__

#include <map>
#include <string>

namespace ast {
class ExpVisitor;

class BuiltIn;
class Let;
class Apply;
class Int;
class If;
class Global;
class Local;
class Primitive;


//Exp is struct

struct Exp {
  public:
    virtual ~Exp() {}
};

class Function {
  private:
    Exp* expr = nullptr;
    int params;  //params count
  public:
    void Initialize(Exp* expr, int params) {
      this->expr = expr;
      this->params = params;
    }
    ~Function () {
      delete expr;
    }
};

//Right now there are only binary int operators
class Operator {
  public:
    virtual int evaluate(int a, int b) = 0;
    virtual ~Operator() {}
};


class ExpVisitor {
  public:
    virtual void visitLet(const Let* p) = 0;
    virtual void visitApply(const Apply* p) = 0;
    virtual void visitInt(const Int* p) = 0;
    virtual void visitIf(const If* p) = 0;
    virtual void visitGlobal(const Global* p) = 0;
    virtual void visitLocal(const Local* p) = 0;
    virtual void visitPrimitive(const Primitive* p) = 0;
<<<<<<< HEAD
=======
    virtual ~ExpVisitor(){}
>>>>>>> memory
};

struct Let : public Exp {
    int id_offset;
    Exp* assigned;
    Exp* value;
    Let(int off, Exp* a, Exp* val) :
      id_offset(off), assigned(a), value(val) {}
    ~Let() { delete assigned; delete value; }  
    void accept(ExpVisitor* p) const { p->visitLet(this); } 

};


struct Apply : public Exp {
    Exp* function;
    Exp* param;
    Apply(Exp* f, Exp* p) : function(f), param(p) {}
    ~Apply() { delete function; delete param; } 
    void accept(ExpVisitor* p) const { p->visitApply(this); } 
};

struct If : public Exp {
    Exp* condition;
    Exp* true_val;
    Exp* false_val;
    If(Exp* c, Exp* t, Exp* f) : condition(c), true_val(t), false_val(f) {}
    ~If() { delete condition; delete true_val; delete false_val; }
    void accept(ExpVisitor* p) const { p->visitIf(this); } 
};

struct Int : public Exp {
    int value;
    Int(int n) : value(n) {}
    void accept(ExpVisitor* p) const { p->visitInt(this); } 
};

struct Local : public Exp {
    int offset;
    Local(int off) : offset(off) {}
    void accept(ExpVisitor* p) const { p->visitLocal(this); } 
};

struct Global : public Exp {
    const Function* global;
    Global(const Function* f) : global(f) {}
    void accept(ExpVisitor* p) const { p->visitGlobal(this); } 
    ~Global() {}
};


struct Primitive : public Exp {
    const Operator* definition;
    void accept(ExpVisitor* p) const { p->visitPrimitive(this); } 
    Primitive(const Operator* definition) : definition(definition) {}
};





class Program {
  private:
    const std::map<std::string, Function>* functions;
    const std::map<std::string, Operator*>* operators;
    const Function* main;
  public:
    Program(const std::map<std::string, Function>* functions,
            const std::map<std::string, Operator*>* operators,
            const Function* main) : functions(functions), operators(operators), main(main) {}
    ~Program() {
      for ( auto it = operators->begin(); it != operators->end(); ++it )
        delete it->second;
      delete operators;
      delete functions;
    }

};


}




#endif
