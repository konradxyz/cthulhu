
/*
 * seq_interpreter.cpp
 *
 *  Created on: 17 paź 2014
 *      Author: kp306410
 */

#include "seq_interpreter.h"
namespace seq_interpreter {
void seq_interpreter::SeqInterpreterFunctor::operator ()() {
	while ( stacks.size() > 0 ) {
		auto& stack = stacks.back();




		this->Cleanup();
	}
	this->Finish();

}

void seq_interpreter::SeqInterpreterFunctor::PerformMarking() {
	for (auto& stack : stacks) {
		for ( auto& thunk : stack.getMutableStack() ) {
			marker.Mark(thunk.first);
		}
		marker.Mark(stack.getTop());
	}
	marker.Mark(main);

}
}


