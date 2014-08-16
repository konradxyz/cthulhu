#include "typechecker/typechecker.h"
#include "gen/Skeleton.H"
#include <map>
#include <string>
#include <memory>
#include <utility>
#include <vector>
using std::pair;
using std::make_pair;
using std::map;
using std::string;
using std::unique_ptr;
using parser::Type;
using std::vector;
using ast::Exp;

namespace typechecker {

namespace {
//We extend skeleton instead of visitor because we are lazy...
class TypesGatherer : public parser::Skeleton {
  private:
    map<string, parser::Type*>* globals = nullptr;
    string* error = nullptr;
    bool ok = true;
  public:
    void visitFnDef(parser::FnDef *fndef) {
      if ( fndef->ident_1 != fndef->ident_2 ) {
        *error = string("Inconsistent function names: '") + fndef->ident_1 + "' and '" +
          fndef->ident_2 + "'";
        ok = false;
        return;
      }
      if ( globals->find(fndef->ident_1) != globals->end() ) {
        *error = string("Multiple definitions of function '") + fndef->ident_1 + "'";
        ok = false;
        return;
      }

      (*globals)[fndef->ident_1] = fndef->type_;
    }
    static bool gatherTypes(parser::Program* prog, map<string, parser::Type*>* result, 
        string* error) {
      TypesGatherer gatherer;
      gatherer.globals = result;
      gatherer.error = error;
      prog->accept(&gatherer);
      return gatherer.ok;
    }


};


Type* ResultType(const Type* app) {
  auto type = dynamic_cast<const parser::FnType*>(app);
  if ( type != nullptr )
    return type->type_2;
  return nullptr;
}


Type* ParamType(const Type* app) {
  auto type = dynamic_cast<const parser::FnType*>(app);
  if ( type != nullptr )
    return type->type_1;
  return nullptr;
}

bool TypesEqual(const Type* f, const Type* s) {
  if ( dynamic_cast<const parser::FnType*>(f) ) {
    return TypesEqual(ResultType(f), ResultType(s))
      && TypesEqual(ParamType(f), ParamType(s));
  } else 
    return dynamic_cast<const parser::IntType*>(s) != nullptr;
}


class ExpGenerator : public parser::Skeleton {
  private:
    parser::IntType INT_TYPE;
    map<string, parser::Type*>* globals;
    map<string, ast::Function>* functions;
    vector<pair<string, const parser::Type*> > locals;
    pair<ast::Exp*, const parser::Type*> result;

    pair<ast::Exp*, const parser::Type*> GenerateExp(parser::Expr* e) {
      e->accept(this);
      return result; 
    }

    ast::Exp* GenerateExp(parser::Expr* e, const parser::Type* type) {
      auto result = this->GenerateExp(e);
      if ( TypesEqual(result.second, type) )
        return result.first;
      delete result.first;
      throw "Types not equal";
    }


  public:
    static ast::Exp* GenerateFunctionExp(parser::Expr* e, 
                                         map<string, parser::Type*>* globals,
                                         map<string, ast::Function>* functions,
                                         vector<pair<string, const parser::Type*> >* params,
                                         Type* expected_type,
                                         string* error) {
      ExpGenerator generator;
      generator.globals = globals;
      generator.functions = functions;
      generator.locals = *params;
      try {
        ast::Exp* result = generator.GenerateExp(e, expected_type);
        return result;
      } catch ( const string& err ) {
        *error = err;
      }
    }
    void visitEVar(parser::EVar *evar) {
      int i = locals.size() - 1;
      while ( i >=  0 && locals[i].first != evar->ident_ )
        --i;
      if ( i >= 0 ) {
        result = make_pair(new ast::Local(i), locals[i].second);
        return;
      }
      if ( globals->find(evar->ident_) != globals->end() ) {
        result = make_pair(new ast::Global(&(*functions)[evar->ident_]), (*globals)[evar->ident_]);
        return; 
      }
      throw string("Unknown identifier: '" + evar->ident_ + "'"); 
    }
    
    void visitELitInt(parser::ELitInt *elitint)
    {
      result = make_pair(new ast::Int(elitint->integer_), &INT_TYPE);
    }
    void visitEApply(parser::EApply *eapply) {
      auto fun = this->GenerateExp(eapply->expr_1);
      unique_ptr<Exp> f(fun.first);
      auto param = this->GenerateExp(eapply->expr_2);
      unique_ptr<Exp> p(param.first);
      auto expected_param_type = ParamType(fun.second);
      if ( expected_param_type == nullptr ) {
        throw string("Expression is not a function");
      }
      if ( !TypesEqual(expected_param_type, param.second) ) {
        throw string("Wrong parameter type!");
      }
      result = make_pair(new ast::Apply(f.release(), p.release()), ResultType(fun.second)); 
    }
    
    void visitELet(parser::ELet *elet) {
      auto assigned = this->GenerateExp(elet->expr_1);
      unique_ptr<Exp> assigned_e(assigned.first);
      locals.push_back(make_pair(elet->ident_, assigned.second)); 
      auto value = this->GenerateExp(elet->expr_2);
      result = make_pair(new ast::Let(locals.size(), assigned_e.release(), value.first), 
                         value.second);
      locals.pop_back();
    }

    void visitECase(parser::ECase *ecase) {
      auto condition = this->GenerateExp(ecase->expr_1, &INT_TYPE);
      auto true_val = this->GenerateExp(ecase->expr_2);
      unique_ptr<Exp> tve(true_val.first);
      auto false_val = this->GenerateExp(ecase->expr_3, true_val.second);
      result = make_pair(new ast::If(condition, tve.release(), false_val), 
                         true_val.second);
    } 



};


class FunctionGenerator : public parser::Skeleton {
  private:
    map<string, ast::Function>* functions;
    string* error;
    map<string, parser::Type*>* globals;
    bool ok = true;
  public:
    void visitFnDef(parser::FnDef *fndef) {
      parser::Type* expected_type = fndef->type_;
      vector<pair<string, const parser::Type*> > params;
      for ( int i = 0; i < fndef->listident_->size() && expected_type != nullptr; ++i ) {
        params.push_back(make_pair(fndef->listident_->at(i), ParamType(expected_type)));
        expected_type = ResultType(expected_type);
      }
      if ( expected_type == nullptr ) {
        ok = false;
        *error = "To many arguments in definition of '" + fndef->ident_1 + "' function";
        return;
      }

      ast::Exp* fun_exp = ExpGenerator::GenerateFunctionExp(fndef->expr_,
                                                            globals,
                                                            functions,
                                                            &params,
                                                            expected_type,
                                                            error);
      if ( fun_exp == nullptr ) {
        ok = false;
        *error = "In function: '" + fndef->ident_1 + "': " + *error;
        return;
      }
      (*functions)[fndef->ident_1].SetExpr(fun_exp);
    }

    static bool GenerateFunctions(parser::Program* p, 
                                  map<string, ast::Function>* function,
                                  map<string, parser::Type*>* globals,
                                  string* error) {
      FunctionGenerator generator;
      generator.functions = function;
      generator.error = error;
      generator.globals = globals;
      p->accept(&generator);
      return generator.ok; 
    }
 
};

}

ast::Program* Typechecker::Typecheck(parser::Program* p, std::string* error) {
  map<string, parser::Type*> globals;
  if ( !TypesGatherer::gatherTypes(p, &globals, error) )
    return nullptr;
  if ( globals.find("call") == globals.end() ) {
    *error = "'call' function not found";
    return nullptr;
  }
  parser::Type* call_type = new parser::FnType(new parser::IntType(),
                                               new parser::IntType());
  if ( !TypesEqual(globals["call"], call_type) ) {
    *error = "Wrong type of function 'call'";
    delete call_type;
    return nullptr;
  }
  delete call_type;
  map<string, ast::Function>* functions = new map<string, ast::Function>();
  
  if ( !FunctionGenerator::GenerateFunctions(p,
                                             functions,
                                             &globals,
                                             error) ) {
    delete functions;
    return nullptr;
  }
  return new ast::Program(functions, &(*functions)["call"]);

}

}
