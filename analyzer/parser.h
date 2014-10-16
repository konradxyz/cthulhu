#ifndef __PARSER_PARSER_
#define __PARSER_PARSER_

#include "gen/Absyn.H"

namespace parser {
class Parser {
	public:
  	Program* Parse(FILE *inp);
};
}
#endif
