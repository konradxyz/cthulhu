/*
 * context.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: kp
 */
#include "asm/context.h"

namespace casm {

Frame::Frame(unsigned envSize, unsigned tmpValuesCount) :
		environment(envSize), temporaryValues(tmpValuesCount), nextInstruction(
				nullptr) {
}
Frame::~Frame() {
}

int Context::intFromTemporary(unsigned id) const {
	return static_cast<const IntValue*>(currentFrame->temporaryValues[id])->getValue();
}

Context::Context() :
		currentFrame(nullptr), nextInstruction(nullptr), base(nullptr) {
}
Context::Context(ContextBase* base) :
		currentFrame(nullptr), nextInstruction(nullptr), base(base) {
}

}

