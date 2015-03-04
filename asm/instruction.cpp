/*
 * instruction.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#include "utils/logging.h"

#include "asm/instruction.h"
#include "asm/program.h"
#include "asm/context.h"
#include "asm/contextbase.h"
#include "asm/futurecontext.h"

namespace casm {

namespace {

void fillFrame(Frame* newFrame, Frame* srcFrame,
		const std::vector<unsigned>& parameters) {
	for (unsigned i = 0; i < parameters.size(); ++i) {
		newFrame->environment[i] = std::move(srcFrame->environment[parameters[i]]);
	}
}

std::unique_ptr<Frame> prepareFrameParams(Context* context, const Function* function,
		const std::vector<unsigned>& parameters) {
	auto result = utils::make_unique<Frame>(function->getEnvSize(), function->getTmpSize());
	fillFrame(result.get(), context->currentFrame, parameters);
	return std::move(result);
}

std::unique_ptr<Context> currentThreadCall(std::unique_ptr<Context>&& context, std::unique_ptr<Frame>&& frame,
		const Function* function, const Instruction* continuation) {
	context->currentFrame->nextInstruction = continuation;
	context->pushFrame(std::move(frame));
	context->nextInstruction = function->getInstruction();
	return std::move(context);
}

std::unique_ptr<Context> otherThreadCall(std::unique_ptr<Context>&& context, std::unique_ptr<Frame>&& frame,
		const Function* function, const Instruction* continuation) {

	if ( context->base->isFull() ) {

		return currentThreadCall(std::move(context), std::move(frame), function, continuation);
	}

	// Generate wrapper to store result.
	auto futureWrapper = casm::generateFutureWrapper(context->base);
	// Generate context that will be queued
	auto parContext = utils::make_unique<Context>(context->base);
	// Prepare first frame - it will wake all waiting contexts.
	auto retFrame = utils::make_unique<Frame>(2, 1);
	retFrame->environment[0] = futureWrapper;

	retFrame->nextInstruction = context->base->getRetInstruction(); // TODO: change to sth meaningful
	parContext->pushFrame(std::move(retFrame));
	// Prepare second frame - this one contains parameters:
	parContext->pushFrame(std::move(frame));
	parContext->nextInstruction = function->getInstruction();
	// Push context into the task stack:
	context->base->pushTask(std::move(parContext));
	// Simulate return:
	context->accumulator = std::move(futureWrapper);
	context->nextInstruction = continuation;
	return std::move(context);
}




std::unique_ptr<Context> performCall(std::unique_ptr<Context>&& context,
		const Instruction* continuation, const Function* function,
		const std::vector<unsigned>& parameters) {
	context->currentFrame->nextInstruction = continuation;
	auto previousFrame = context->currentFrame;
	context->allocateFrame(function->getEnvSize(), function->getTmpSize());
	fillFrame(context->currentFrame, previousFrame, parameters);
	context->nextInstruction = function->getInstruction();
	return std::move(context);
}

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

std::unique_ptr<Context> CallFunction::perform(std::unique_ptr<Context>&& context) const {
	return currentThreadCall(std::move(context),
				prepareFrameParams(context.get(), function, parameters), function,
				getContinuation());
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

namespace seq {
std::unique_ptr<Context> Load::perform(std::unique_ptr<Context>&& context) const {
	auto frame = context->currentFrame;
	frame->temporaryValues[target] = frame->environment[source]->getValue();
	context->nextInstruction = getContinuation();
	return std::move(context);
}
}

namespace par {

std::unique_ptr<Context> Load::perform(std::unique_ptr<Context>&& context) const {

	auto frame = context->currentFrame;
	auto src = frame->environment[source].get();
	context->nextInstruction = getContinuation();
	switch (src->getType()) {
	case ValueType::REAL_VALUE:
		frame->temporaryValues[target] = src->getValue();

		return std::move(context);
	case ValueType::FUTURE_VALUE: {
		auto res = src->getFutureContext()->updateContext(std::move(context), source, target);
		return std::move(res);
	}
	default:
		return nullptr; // Should not happen.
	}
}

std::unique_ptr<Context> CallFunctionPar::perform(std::unique_ptr<Context>&& context) const {

return otherThreadCall(std::move(context),
			prepareFrameParams(context.get(), function, parameters), function,
			getContinuation());
}

std::unique_ptr<Context> SetValueAndWake::perform(std::unique_ptr<Context>&& context) const {
	auto future = context->currentFrame->environment[futureWrapperId]->getFutureContext();
	future->setValueAndWakeAll(std::move(context->currentFrame->environment[valueWrapperId]));
	return nullptr;
}

std::unique_ptr<Context> StoreResult::perform(std::unique_ptr<Context>&& context) const {
	LOG(debug, "storing");
	keeper->result = context->intFromTemporary(source);
	context->base->stop();

	return nullptr;
}


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

