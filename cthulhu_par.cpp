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

int main(int argc, char ** argv) {
	int threads = atoi(argv[1]);
	int param = atoi(argv[2]);
	cout << "Multiple threads " << threads << " with param " << param << endl;
	utils::Allocator<casm::Instruction> alloc;
	auto program = generateProgram(9);
	casm::Executor e(2 * threads, threads);
	auto result = e.run(program.get(), param);
	cout << "Multiple threads " << threads << " with param " << param << " result " << result << endl;
}


