/*
 * operations.h
 *
 *  Created on: Jan 14, 2015
 *      Author: kp
 */

#ifndef OPERATIONS_OPERATIONS_H_
#define OPERATIONS_OPERATIONS_H_

#include<memory.h>

#include "environment.h"

class TaskQueue {

};

class Operation {
public:
	virtual std::unique_ptr<Operation> perform(TaskQueue* queue) = 0;
	virtual ~Operation() {
	}
};

class AcceptEnvironmentOperation : public Operation, public AcceptEnvironment {
};

class AcceptRealValueOwnershipOperation : public Operation, public AcceptRealValueOwnership {
};

class AcceptRealValueOwnershipAndEnvironmentOperation : public AcceptRealValueOwnershipOperation, public AcceptEnvironment {
};

class AcceptRealValueAndEnvironmentOperation : public AcceptEnvironmentOperation {
public:
	RealValue* getValue() {
		return value;
	}

	void setValue(RealValue* value) {
		this->value = value;
	}

private:
	RealValue* value;
};

template
<class T>
class WithContinuation {
private:
	std::unique_ptr<T> continuation;
public:
	void setContinuation(std::unique_ptr<T>&& continuation) {
		this->continuation = std::move(continuation);
	}
	std::unique_ptr<T> releaseContinuation() {
		return std::move(continuation);
	}
};

// Trailing operations - those will destroy environment before they would block.
// The idea is that those operations will return value from function.
class TrailingOperaion : public WithContinuation<AcceptRealValueOwnershipOperation>{
};

class TrailingGetOperation : public TrailingOperaion, public AcceptEnvironmentOperation {
private:
	int paramId;
public:
	TrailingGetOperation(int paramId) : paramId(paramId) {}
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto env = this->releaseEnvironment();
		auto result = env->getValues()->operator [](paramId);
		auto continuation = releaseContinuation();
		// It might be good idea to enable_shared_from_this in here.
		continuation->setValue(std::move(std::static_pointer_cast<RealValue>(result)));
		return std::move(continuation);
	}
};


class GetRealValueFromEnvOperation : public AcceptEnvironmentOperation, public WithContinuation<AcceptRealValueAndEnvironmentOperation> {
private:
	int paramId;
public:
	GetRealValueFromEnvOperation(int paramId) : paramId(paramId) {}
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto env = this->releaseEnvironment();
		auto result = env->getValues()->operator [](paramId)->asRealValue();
		auto continuation = releaseContinuation();
		continuation->setEnvironment(std::move(env));
		// TODO: this one WILL be failing.
		continuation->setValue(result);
		return std::move(continuation);
	}
};

class SetRealValueForOperatorOperation: public AcceptRealValueAndEnvironmentOperation,
		public WithContinuation<AcceptEnvironmentOperation> {
private:
	int paramId;
public:
	SetRealValueForOperatorOperation(int paramId) :
			paramId(paramId) {
	}
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto env = this->releaseEnvironment();
		(*env->getOperatorValues())[paramId] = this->getValue();
		auto continuation = releaseContinuation();
		continuation->setEnvironment(std::move(env));
		return std::move(continuation);
	}
};
template
<class T>
class EvaluateBinaryOperatorOperation : public AcceptEnvironmentOperation, public WithContinuation<AcceptEnvironmentOperation> {
private:
	int leftId;
	int rightId;
	int targetId;
	T operatorFunctor;
public:
	EvaluateBinaryOperatorOperation(int leftId, int rightId, int targetId) :
		leftId(leftId),
		rightId(rightId),
		targetId(targetId) {
	}
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto env = this->releaseEnvironment();
		auto left = static_cast<IntValue*>(env->getOperatorValues()->operator[](leftId));
		auto right = static_cast<IntValue*>(env->getOperatorValues()->operator[](rightId));
		auto result = std::make_shared<IntValue>(operatorFunctor(left->getVal(), right->getVal()));
		env->getValues()->operator[](targetId) = std::move(result);
		auto continuation = releaseContinuation();
		continuation->setEnvironment(std::move(env));
		return std::move(continuation);
	}

};

template
<class L, class R>
std::unique_ptr<L> operator>>=(std::unique_ptr<L>&& left, std::unique_ptr<R>&& right) {
	left->setContinuation(std::move(right));
	return std::move(left);
}
#endif /* OPERATIONS_OPERATIONS_H_ */
