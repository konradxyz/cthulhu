#include "parser/parser.h"
#include "gen/Parser.H"


namespace parser {

Program* Parser::Parse(FILE *inp) {
  return pProgram(inp);
}


}

