/*
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
 */

#include "test/test.h"
#include<iostream>
#include <cstdlib>
#include "utils/logging.h"

using namespace std;
/*

int main(int argc, char ** argv) {
	int fun = atoi(argv[1]);
	int param = atoi(argv[2]);

	utils::Allocator<casm::Instruction> alloc;
	casm::ResultKeeper keeper;
	auto program = generateProgram(fun);
	auto ctx = program->generateStartingContext(param, &alloc, &keeper);
	casm::Executor::execute(std::move(ctx));
	cout << param << " " << keeper.result << endl;
}
*/



int main(int argc, char ** argv) {
	int threadc = atoi(argv[1]);
	auto program = generateProgram(9);
	casm::Executor e(20, threadc);
	int result = e.run(program.get(), 8);
	LOG(debug, result);
}
