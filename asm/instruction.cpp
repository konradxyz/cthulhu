/*
 * instruction.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#include "utils/logging.h"

#include "asm/instruction.h"
#include "asm/program.h"


namespace casm {


std::unique_ptr<Context> MoveA::perform(std::unique_ptr<Context>&& context) const {
	context->accumulator = std::move(context->currentFrame->environment[source]);
	context->nextInstruction = this->getContinuation();
	return std::move(context);
}

std::unique_ptr<Context> MoveFromA::perform(std::unique_ptr<Context>&& context) const {
	context->currentFrame->environment[target] = std::move(context->accumulator);
	context->nextInstruction = this->getContinuation();
	return std::move(context);
}

std::unique_ptr<Context> CallFunction::perform(std::unique_ptr<Context>&& context) const {
	context->currentFrame->nextInstruction = this->getContinuation();
	auto previousFrame = context->currentFrame;
	context->allocateFrame(this->function->getEnvSize(), this->function->getTmpSize());
	for ( unsigned i = 0; i < parameters.size(); ++i ) {
		context->currentFrame->environment[i] = previousFrame->environment[parameters[i]];
	}
	context->nextInstruction = this->function->getInstruction();
	return std::move(context);
}

std::unique_ptr<Context> StoreResult::perform(std::unique_ptr<Context>&& context) const {
	keeper->result = context->intFromTemporary(source);
	return nullptr;
}

std::unique_ptr<Context> Return::perform(std::unique_ptr<Context>&& context) const {
	context->removeLastFrame();
	context->nextInstruction = context->currentFrame->nextInstruction;
	return std::move(context);
}

std::unique_ptr<Context> Load::perform(std::unique_ptr<Context>&& context) const {
	auto frame = context->currentFrame;
	frame->temporaryValues[target] = frame->environment[source]->getValue();
	context->nextInstruction = getContinuation();
	return std::move(context);
}


std::unique_ptr<Context> IfElseInstruction::perform(
		std::unique_ptr<Context>&& context) const {
	int condition = context->intFromTemporary(this->conditionIndex);
	if (condition)
		context->nextInstruction = trueContinuation;
	else
		context->nextInstruction = falseContinuation;
	return std::move(context);
}


}

