/*
 * contextbase.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: kp
 */

#include "asm/contextbase.h"
#include "asm/instruction.h"

namespace casm {
const Instruction* ContextBase::generateStartingInstruction() {
	auto res =
			allocator.alloc<MoveFromA>(1,
			allocator.alloc<par::Load>(1, 0,
			allocator.alloc<par::SetValueAndWake>(0, 1)
			));
	return res;
}
}



