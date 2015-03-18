#ifndef __PARSER_PARSER_
#define __PARSER_PARSER_

#include "gen/Absyn.H"
#include <memory>

namespace parser {
class Parser {
	public:
  	std::unique_ptr<Program> Parse(FILE *inp);
};
}
#endif
