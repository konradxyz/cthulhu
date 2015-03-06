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
	int fun = atoi(argv[1]);
	int threads = atoi(argv[2]);
	int param = atoi(argv[3]);
	std::atomic_int tmp(0);
	if ( !tmp.is_lock_free() ) {
		cout << "atomic_int not lock free" << endl;
	}
	std::atomic_bool b ( false);
	if ( !b.is_lock_free() ) {
		cout << "atomic_bool not lock free" << endl;
	}
	cout << "Function " << fun << " multiple threads " << threads
			<< " with param " << param << endl;
	utils::Allocator<casm::Instruction> alloc;
	auto program = generateProgram(fun);
	casm::Executor e(2 * threads, threads);
	auto result = e.run(program.get(), param);
	cout << "Function " << fun << " multiple threads " << threads
			<< " with param " << param << " result " << result << endl;
}


