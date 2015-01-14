
/*
 * seq_interpreter.cpp
 *
 *  Created on: 17 paź 2014
 *      Author: kp306410
 */

#include "seq_interpreter.h"

#include "config.h"
#include "utils/ptr.h"
#include <cassert>


namespace seq_interpreter {

void seq_interpreter::SeqInterpreterFunctor::operator ()() {
	while ( stacks.size() > 0 ) {
		this->NextConfiguration();
		this->Cleanup();
	}
	this->Finish();
}

void seq_interpreter::SeqInterpreterFunctor::PerformMarking() {
	for (auto& stack : stacks) {
		for ( auto& thunk : *stack.getMutableStack() ) {
			marker.Mark(thunk.first);
		}
		marker.Mark(stack.getTop());
	}
	marker.Mark(main);

}

int SeqInterpreter::Run(const ast::Program* program, int param) {
	memory::MemoryManagerFactory factory(MEMORY);
	auto memory_manager = factory.Get<thunk::ThunkWrapper>();
	auto param_thunk = memory_manager->Allocate(utils::make_unique<thunk::IntThunk>(param));
	auto call_thunk = memory_manager->Allocate(utils::make_unique<thunk::GlobalThunk>(program->getMain()));
	auto starting = memory_manager->Allocate(utils::make_unique<thunk::ApplyThunk>(call_thunk, param_thunk));
	return 0;
}

void SeqInterpreterFunctor::visitApply(thunk::ApplyThunk* p) {
	auto top = stacks.back().getTop();
	stacks.back().getMutableStack()->push_back({top, p->getParam()});
	stacks.back().setTop(p->getFunction());
}

// For now we simply remove current stack
void SeqInterpreterFunctor::visitInt(thunk::IntThunk* p) {
	assert(stacks.back().getMutableStack()->empty());
	stacks.pop_back();
}

void SeqInterpreterFunctor::visitIf(thunk::IfThunk* p) {
	auto condition = p->getCondition();
	if ( condition->getExpr()->evaluated() ) {
		int condition_value = condition->getExpr()->toInt();
		auto result = p->getFalseVal();
		if ( condition_value ) {
			result = p->getTrueVal();
		}
		if ( result->isSingleReference() ) {
			stacks.back().getTop()->setExpr(result->releaseExpr());
		} else {
			auto expr = result->releaseExpr();
		}
	} else {
		condition->incRefCounter();
		stacks.emplace_back(condition);
	}
}

void SeqInterpreterFunctor::visitGlobal(thunk::GlobalThunk* p) {
}

void SeqInterpreterFunctor::visitOperator(thunk::OperatorThunk* p) {

}

void SeqInterpreterFunctor::NextConfiguration() {
	stacks.back().getTop()->getExpr()->accept(*this);
}

}
