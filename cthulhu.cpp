#include "parser/parser.h"
#include "gen/Absyn.H"
#include "gen/Printer.H"
#include "typechecker/typechecker.h"

using parser::Program;

Program* parse(const char* path) {
  FILE* in;
  in = fopen(path, "r");
  if (!in)
  {
      printf("Error opening file %s.\n", path);
      exit(1);
  }
  parser::Parser p;  
  Program *parse_tree = p.Parse(in);
  if ( !parse_tree ) {
    printf("Syntax error in %s.\n", path);
    exit(1);
  }
  return parse_tree;
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
		if ( program )
			printf("types ok\n");
		else
			printf("bad types %s\n", error.c_str());
		delete main;
		delete program;    
  } else
    printf("syntax error\n");
  return 1;
}
