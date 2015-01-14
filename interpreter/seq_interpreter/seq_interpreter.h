/*
 * par_interpreter.h
 *
 *  Created on: Oct 14, 2014
 *      Author: kp
 */

#ifndef INTERPRETER_PARALLEL_SEQ_INTERPRETER_H_
#define INTERPRETER_PARALLEL_SEQ_INTERPRETER_H_

#include "interpreter/interpreter.h"
#include "utils/memory.h"
#include "interpreter/thunk/thunk.h"
#include "interpreter/thunk/thunk_marker.h"

namespace seq_interpreter {

class ExpressionStack {
private:
	thunk::ThunkPtr top;
	std::vector<std::pair<thunk::ThunkPtr, thunk::ThunkPtr>> stack;
public:
	ExpressionStack(thunk::ThunkPtr expr) :
			top(expr) {
	}

	std::vector<std::pair<thunk::ThunkPtr, thunk::ThunkPtr>>* getMutableStack() {
		return &stack;
	}

	thunk::ThunkPtr getTop() const {
		return top;
	}

	void setTop(thunk::ThunkPtr top) {
		this->top = top;
	}
};

class SeqInterpreterFunctor: public memory::MemoryFunctor<thunk::ThunkWrapper>,
		public thunk::ThunkVisitor {
private:
	std::vector<ExpressionStack> stacks;
	thunk_marker::ThunkMarker marker;
	thunk::ThunkPtr main;
public:
	void operator()();
	void PerformMarking();
	SeqInterpreterFunctor(thunk::ThunkPtr main,
			memory::MemoryManager<thunk::ThunkWrapper>* manager) :
			memory::MemoryFunctor<thunk::ThunkWrapper>(manager), marker(this), main(
					main) {
		stacks.emplace_back(main);
	}
	void visitApply(thunk::ApplyThunk* p);
	void visitInt(thunk::IntThunk* p);
	void visitIf(thunk::IfThunk* p);
	void visitGlobal(thunk::GlobalThunk* p);
	void visitOperator(thunk::OperatorThunk* p);
	// Assumes that stacks is not empty
	void NextConfiguration();

};

class SeqInterpreter: public interpreter::Interpreter {

public:
	virtual int Run(const ast::Program* program, int param);
};

}

#endif /* INTERPRETER_PARALLEL_SEQ_INTERPRETER_H_ */
