/*
 * operations.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: kp
 */

#include "operations/operations.h"

std::unique_ptr<Operation> TrailingGetOperation::perform(TaskQueue* queue) {
	auto env = this->releaseEnvironment();
	auto result = env->getValues()->operator [](paramId);
	auto continuation = releaseContinuation();
	// It might be good idea to enable_shared_from_this in here.
	continuation->setValue(
			std::move(std::static_pointer_cast<RealValue>(result)));
	return std::move(continuation);
}

std::unique_ptr<Operation> GetRealValueFromEnvOperation::perform(
		TaskQueue* queue) {
	auto env = this->releaseEnvironment();
	auto result = env->getValues()->operator [](paramId)->asRealValue();
	auto continuation = releaseContinuation();
	continuation->setEnvironment(std::move(env));
	// TODO: this one WILL be failing.
	continuation->setValue(result);
	return std::move(continuation);
}

std::unique_ptr<Operation> SetRealValueForOperatorOperation::perform(
		TaskQueue* queue) {
	auto env = this->releaseEnvironment();
	(*env->getOperatorValues())[paramId] = this->getValue();
	auto continuation = releaseContinuation();
	continuation->setEnvironment(std::move(env));
	return std::move(continuation);
}

static std::unique_ptr<Operation> generateOperationFromWrapper(std::unique_ptr<OperationWrapper>&& wrapper,
														std::unique_ptr<Environment>&& env,
														std::unique_ptr<AcceptRealValueOwnershipOperation>&& continuation) {
	auto op = wrapper->releaseOperation();
	op->setEnvironment(std::move(env));
	wrapper->getTrailingOperation()->setContinuation(
			std::move(continuation));
	return std::move(op);
}

std::unique_ptr<Operation> IfElseTrailingOperation::perform(TaskQueue* queue) {
	IntValue* condition = static_cast<IntValue*> (this->getValue());
	if ( condition->getVal() ) {
		return generateOperationFromWrapper(std::move(trueWrapper), releaseEnvironment(), releaseContinuation());
	} else {
		return generateOperationFromWrapper(std::move(falseWrapper), releaseEnvironment(), releaseContinuation());
	}
}
