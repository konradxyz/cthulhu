#ifndef __TYPECHECK_OPERATORS__
#define __TYPECHECK_OPERATORS__

#include "ast/ast.h"
#include "gen/Absyn.H"
#include <map>
#include <string>
namespace typechecker {
class Predefined {
  private:
    parser::Type* type = nullptr;
  public:
    //parser::Type pointers in types will be valid as long as Predefined object is alive
    void GetTypes(std::map<std::string, parser::Type*>* types); 
    //On the other hand... - caller is responsible for deleteing operator pointers
    void GetOperators(std::map<std::string, ast::Operator*>* operators);
    ~Predefined() {
      delete type;
    }
};

}


#endif
