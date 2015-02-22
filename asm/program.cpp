/*
 * program.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: kp
 */


#include "asm/program.h"

namespace casm {

std::unique_ptr<Context> Program::generateStartingContext(int param, utils::Allocator<Instruction>* allocator, ResultKeeper* keeper) const {
	auto result = utils::make_unique<Context>();
	result->allocateFrame(1);
	result->currentFrame->environment[0] = generateValueWrapper(utils::make_unique<IntValue>(param));
	auto instruction =
			allocator->alloc<CallFunction>(main, std::vector<unsigned>({0}),
			allocator->alloc<MoveFromA>(0,
			allocator->alloc<LoadL>(0,
			allocator->alloc<StoreResult>(keeper))));
	result->nextInstruction = instruction;
	return std::move(result);
}

}
