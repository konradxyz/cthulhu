#ifndef __INTERPRETER_INTERPRETER_
#define __INTERPRETER_INTERPRETER_

#include "ast/ast.h"

namespace interpreter {


//base Interpreter interface with dummy implementation
class Interpreter {
  public:
    virtual int Run(const ast::Program* program,
                    int param);
    virtual ~Interpreter() {}

};

}
#endif
