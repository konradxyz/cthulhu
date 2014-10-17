/*
 * thunk_marker.cpp
 *
 *  Created on: 17 paź 2014
 *      Author: kp306410
 */

#include "thunk_marker.h"
namespace thunk_marker {
void ThunkMarker::visitApply(thunk::ApplyThunk* p) {
	Mark(p->getFunction());
	Mark(p->getParam());
}

void ThunkMarker::visitInt(thunk::IntThunk* p) {
}

void ThunkMarker::visitIf(thunk::IfThunk* p) {
	Mark(p->getCondition());
	Mark(p->getTrueVal());
	Mark(p->getFalseVal());
}

void ThunkMarker::visitGlobal(thunk::GlobalThunk* p) {
}

void ThunkMarker::visitOperator(thunk::OperatorThunk* p) {
}

void ThunkMarker::Mark(thunk::ThunkPtr ptr) {
	bool need_marking = !functor->Mark(ptr);
	/*while (need_marking && ptr->getNext() != nullptr ) {
		ptr = ptr->getNext();
		need_marking = !functor->Mark(ptr);
	}*/
	if ( need_marking )
		ptr->getExpr()->accept(*this);
}

}
