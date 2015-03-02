/*
 * value.h
 *
 *  Created on: Feb 17, 2015
 *      Author: kp
 */

#ifndef ASM_VALUE_H_
#define ASM_VALUE_H_

namespace casm {

class Function;
class Value;
class FutureContext;
class ContextBase;

}

#include <vector>
#include <memory>
#include <sstream>


namespace casm {

enum ValueType {
	FUTURE_VALUE = 0,
	REAL_VALUE = 1
};

// TODO: we might improve performance by moving int to this class.
class ValueWrapper {
private:
	const ValueType type;
	std::unique_ptr<Value> value;

	std::unique_ptr<FutureContext> future;
public:
	ValueWrapper(std::unique_ptr<FutureContext>&& future);
	ValueWrapper(std::unique_ptr<Value>&& value);

	const Value* getValue() {
		return value.get();
	}

	const ValueType getType() const {
		return type;
	}

	FutureContext* getFutureContext() {
		return future.get();
	}
	~ValueWrapper();
};

class Value {
public:
	virtual std::string print() const {
		return "unknown";
	}

	virtual ~Value() {}
};

class ApplyValue : public Value {
private:
	const Function* function;
	std::vector<std::shared_ptr<ValueWrapper>> arguments;
};

class StructureValue : public Value {
private:
	unsigned constructorId;
	std::vector<std::shared_ptr<ValueWrapper>> arguments;
};

class IntValue : public Value {
public:
	std::string print() const override {
		std::ostringstream ss;
		ss << value;
		std::string str = ss.str();

		return std::string("(int ") + str + ")";
	}
	int getValue() const {
		return value;
	}
	IntValue(int value) : value(value) {}
private:
	int value;
};

std::shared_ptr<ValueWrapper> generateFutureWrapper(ContextBase* base);
std::shared_ptr<ValueWrapper> generateValueWrapper(std::unique_ptr<Value>&& value);

}

#endif /* ASM_VALUE_H_ */
