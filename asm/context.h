/*
 * context.h
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#ifndef ASM_CONTEXT_H_
#define ASM_CONTEXT_H_

#include "asm/value.h"
#include "utils/ptr.h"
#include <vector>

namespace casm {

class Instruction;

struct Accumulator {
	const Value* value;
};

struct Frame {
	std::vector<std::shared_ptr<ValueWrapper>> environment;
	const Instruction* nextInstruction;
};

struct Context {
	std::vector<std::unique_ptr<Frame>> frames;
	Frame* currentFrame;
	std::shared_ptr<ValueWrapper> accumulator;
	Accumulator lAccumulator;
	Accumulator rAccumulator;
	const Instruction* nextInstruction;

public:
	void allocateFrame(unsigned size) {
		frames.push_back(utils::make_unique<Frame>());
		currentFrame = frames.back().get();
		currentFrame->environment.resize(size);
	}

	void removeLastFrame() {
		frames.pop_back();
		currentFrame = frames.back().get();
	}


	int intFromLAccumulator() {
		return static_cast<const IntValue*>(lAccumulator.value)->getValue();
	}
};

}


#endif /* ASM_CONTEXT_H_ */
