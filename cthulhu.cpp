#include "analyzer/parser.h"
#include "gen/Absyn.H"
#include "gen/Printer.H"
#include "analyzer/typechecker.h"
#include "interpreter/interpreter.h"
#include <memory>
#include "utils/ptr.h"
using parser::Program;

Program* parse(const char* path) {
	FILE* in;
	in = fopen(path, "r");
	if (!in) {
		printf("Error opening file %s.\n", path);
		exit(1);
	}
	parser::Parser p;
	Program *parse_tree = p.Parse(in);
	if (!parse_tree) {
		printf("Syntax error in %s.\n", path);
		exit(1);
	}
	return parse_tree;
}

std::unique_ptr<interpreter::Interpreter> GetInterpreter() {
	return utils::make_unique<interpreter::Interpreter>();
}

int main(int argc, char ** argv) {
	Program* main;
	if (argc <= 2) {
		printf("Usage: %s file.ct param\n", argv[0]);
		exit(1);
	}

	main = parse(argv[1]);
	if (main) {
		parser::PrintAbsyn printer;
		printf("%s\n", printer.print(main));
		typechecker::Typechecker t;
		std::string error;
		ast::Program* program = t.Typecheck(main, &error);
		auto interpreter = GetInterpreter();
		if (program) {
			int result = interpreter->Run(program, atoi(argv[2]));
			printf("%d\n", result);
		} else
			printf("Error: %s\n", error.c_str());
		delete main;
		delete program;    
  } else {
    printf("Syntax error\n");
    return 1;
  }
}
