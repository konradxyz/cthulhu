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
	std::vector<const Value*> temporaryValues;
	const Instruction* nextInstruction;

	Frame(unsigned envSize, unsigned tmpValuesCount) : environment(envSize), temporaryValues(tmpValuesCount), nextInstruction(nullptr) {}
};

struct Context {
	std::vector<std::unique_ptr<Frame>> frames;
	Frame* currentFrame;
	std::shared_ptr<ValueWrapper> accumulator;
	Accumulator lAccumulator;
	Accumulator rAccumulator;
	const Instruction* nextInstruction;

public:
	void allocateFrame(unsigned envSize, unsigned tmpSize) {
		frames.push_back(utils::make_unique<Frame>(envSize, tmpSize));
		currentFrame = frames.back().get();
	}

	void removeLastFrame() {
		frames.pop_back();
		currentFrame = frames.back().get();
	}


	int intFromLAccumulator() {
		return static_cast<const IntValue*>(lAccumulator.value)->getValue();
	}

	int intFromTemporary(unsigned id) const {
		return static_cast<const IntValue*>(currentFrame->temporaryValues[id])->getValue();
	}
};

}


#endif /* ASM_CONTEXT_H_ */
