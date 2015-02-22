/*
 * instruction.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */


#include "asm/instruction.h"
#include "asm/program.h"


namespace casm {

std::unique_ptr<Context> Load::perform(std::unique_ptr<Context>&& context,  Accumulator* target) const {
	target->value = context->currentFrame->environment[source]->getValue();
	context->nextInstruction = this->getContinuation();
	return std::move(context);
}

std::unique_ptr<Context> LoadL::perform(std::unique_ptr<Context>&& context) const {
	auto target = &context->lAccumulator;
	return Load::perform(std::move(context), target);
}


std::unique_ptr<Context> LoadR::perform(std::unique_ptr<Context>&& context) const {
	auto target = &context->rAccumulator;
	return Load::perform(std::move(context), target);
}


std::unique_ptr<Context> LoadConst::perform(std::unique_ptr<Context>&& context,  Accumulator* target) const {
	target->value = value.get();
	context->nextInstruction = this->getContinuation();
	return std::move(context);
}

std::unique_ptr<Context> LoadConstL::perform(std::unique_ptr<Context>&& context) const {
	auto target = &context->lAccumulator;
	return LoadConst::perform(std::move(context), target);
}


std::unique_ptr<Context> LoadConstR::perform(std::unique_ptr<Context>&& context) const {
	auto target = &context->rAccumulator;
	return LoadConst::perform(std::move(context), target);
}



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

std::unique_ptr<Context> IfElseInstruction::perform(
		std::unique_ptr<Context>&& context) const {
	int condition = context->intFromLAccumulator();
	if (condition)
		context->nextInstruction = trueContinuation;
	else
		context->nextInstruction = falseContinuation;
	return std::move(context);
}

std::unique_ptr<Context> CallFunction::perform(std::unique_ptr<Context>&& context) const {
	context->currentFrame->nextInstruction = this->getContinuation();
	auto previousFrame = context->currentFrame;
	context->allocateFrame(this->function->getEnvSize());
	for ( unsigned i = 0; i < parameters.size(); ++i ) {
		context->currentFrame->environment[i] = previousFrame->environment[parameters[i]];
	}
	context->nextInstruction = this->function->getInstruction();
	return std::move(context);
}

std::unique_ptr<Context> StoreResult::perform(std::unique_ptr<Context>&& context) const {
	keeper->result = context->intFromLAccumulator();
	return nullptr;
}

std::unique_ptr<Context> Return::perform(std::unique_ptr<Context>&& context) const {
	context->removeLastFrame();
	context->nextInstruction = context->currentFrame->nextInstruction;
	return std::move(context);
}

}

