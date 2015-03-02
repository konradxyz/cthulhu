/*
 * context.h
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#ifndef ASM_CONTEXT_H_
#define ASM_CONTEXT_H_

namespace casm {

class Instruction;
class ContextBase;

}

#include "asm/value.h"
#include "utils/ptr.h"
#include <vector>
#include "utils/spinlock.h"
#include "utils/allocator.h"
#include <mutex>

namespace casm {


struct Frame {
	std::vector<std::shared_ptr<ValueWrapper>> environment;
	std::vector<const Value*> temporaryValues;
	const Instruction* nextInstruction;

	Frame(unsigned envSize, unsigned tmpValuesCount);
	~Frame();
};

struct Context {
	std::vector<std::unique_ptr<Frame>> frames;
	Frame* currentFrame;
	std::shared_ptr<ValueWrapper> accumulator;
	const Instruction* nextInstruction;
	ContextBase* const base;
public:
	// TODO: remove this constructor.
	Context();
	Context(ContextBase* base);

	// This one should be removed as well probably.
	void allocateFrame(unsigned envSize, unsigned tmpSize) {
		pushFrame(utils::make_unique<Frame>(envSize, tmpSize));
	}

	void pushFrame(std::unique_ptr<Frame>&& frame) {
		frames.push_back(std::move(frame));
		currentFrame = frames.back().get();
	}

	void removeLastFrame() {
		frames.pop_back();
		currentFrame = frames.back().get();
	}

	int intFromTemporary(unsigned id) const;
};

}


#endif /* ASM_CONTEXT_H_ */
