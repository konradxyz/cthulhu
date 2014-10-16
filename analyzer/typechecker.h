#ifndef __TYPECHECKER_TYPECHECKER_
#define __TYPECHECKER_TYPECHECKER_

#include "gen/Absyn.H"
#include "ast/ast.h"
#include <string>

namespace typechecker {

class Typechecker {
  public:
    ast::Program* Typecheck(parser::Program* p, std::string* error);
};

}
#endif
