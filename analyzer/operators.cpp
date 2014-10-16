#include "analyzer/operators.h"

using parser::FnType;
using parser::IntType;
using std::string;

namespace typechecker {
namespace {
const int PREDEFINED_COUNT { 4 };
const char* PREDEFINED[] = {
    "lt",
    "add",
    "sub",
    "mul"
};

template
<typename T>

class OpOperator : public ast::Operator {
  private:
    T op;
  public:
    int evaluate(int a, int b) {
      return op(a, b);
    }
};

class LtOperator : public ast::Operator {
  public:
    int evaluate(int a, int b) {
      return a < b ? 1 : 0;
    }
};

}

void Predefined::GetTypes(std::map<std::string, parser::Type*>* types) {
  if ( this->type == nullptr )
    this->type = new FnType(new IntType(), new FnType(new IntType(), new IntType()));
  for ( int i = 0; i < PREDEFINED_COUNT; ++i )
    (*types)[string(PREDEFINED[i])] = this->type;
}


void Predefined::GetOperators(std::map<std::string, ast::Operator*>* operators) {
  (*operators)["lt"] = new LtOperator;
  (*operators)["add"] = new OpOperator<std::plus<int> >;
  (*operators)["sub"] = new OpOperator<std::minus<int> >;
  (*operators)["mul"] = new OpOperator<std::multiplies<int> >;


}

}


