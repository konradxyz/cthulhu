#include "analyzer/parser.h"
#include "gen/Parser.H"


namespace parser {

std::unique_ptr<Program> Parser::Parse(FILE *inp) {
  return std::unique_ptr<Program>(pProgram(inp));
}


}

