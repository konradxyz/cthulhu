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

class SeqInterpreterThunkVisitor : public thunk::ThunkVisitor {
public:
	void visitApply(thunk::ApplyThunk* p);
	void visitInt(thunk::IntThunk* p);
	void visitIf(thunk::IfThunk* p);
	void visitGlobal(thunk::GlobalThunk* p);
	void visitOperator(thunk::OperatorThunk* p);
};

class ExpressionStack {
private:
	thunk::ThunkPtr top;
	std::vector<std::pair<thunk::ThunkPtr, thunk::ApplyThunk*>> stack;
public:
	ExpressionStack(thunk::ThunkPtr expr) : top(expr){}

	std::vector<thunk::ThunkPtr, thunk::ApplyThunk*>* getMutableStack() {
		return &stack;
	}

	thunk::ThunkPtr getTop() const {
		return top;
	}

	void setTop(thunk::ThunkPtr top) {
		this->top = top;
	}
};

class SeqInterpreterFunctor : public memory::MemoryFunctor<thunk::Thunk> {
private:
	std::vector<ExpressionStack> stacks;
	thunk_marker::ThunkMarker marker;
	thunk::ThunkPtr main;
public:
	void operator()();
	void PerformMarking();
	SeqInterpreterFunctor(thunk::ThunkPtr main) : marker(this), main(main) {
		stacks.emplace_back(main);
	}

};

class SeqInterpreter : public interpreter::Interpreter {

};

}


#endif /* INTERPRETER_PARALLEL_SEQ_INTERPRETER_H_ */
