/*
 * thunk_marker.h
 *
 *  Created on: 17 paź 2014
 *      Author: kp306410
 */

#ifndef THUNK_MARKER_H_
#define THUNK_MARKER_H_

#include "utils/memory.h"

namespace thunk_marker {

class ThunkMarker: public thunk::ThunkVisitor {
private:
	memory::MemoryFunctor<thunk::ThunkWrapper>* functor;
public:
	void visitApply(thunk::ApplyThunk* p);
	void visitInt(thunk::IntThunk* p);
	void visitIf(thunk::IfThunk* p);
	void visitGlobal(thunk::GlobalThunk* p);
	void visitOperator(thunk::OperatorThunk* p);
	void Mark(thunk::ThunkPtr ptr);
	ThunkMarker(memory::MemoryFunctor<thunk::ThunkWrapper>* functor) :
			functor(functor) {
	}
};

}

#endif /* THUNK_MARKER_H_ */
