/*
 * value.h
 *
 *  Created on: Feb 17, 2015
 *      Author: kp
 */

#ifndef ASM_VALUE_H_
#define ASM_VALUE_H_

#include <vector>
#include <memory>
#include <sstream>


namespace casm {
class Function;
class Value;

enum ValueType {
	FUTURE_VALUE = 0,
	REAL_VALUE = 1
};

class ValueWrapper {
private:
	const ValueType type;
	std::unique_ptr<Value> value;

	//std::shared_ptr<ValueWrapper> futureValue;
public:
	ValueWrapper() : type(FUTURE_VALUE) {}
	ValueWrapper(std::unique_ptr<Value>&& value) : type(REAL_VALUE), value(std::move(value)) {}

	const Value* getValue() {
		return value.get();
	}
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

std::shared_ptr<ValueWrapper> generateFutureWrapper();
std::shared_ptr<ValueWrapper> generateValueWrapper(std::unique_ptr<Value>&& value);

}

#endif /* ASM_VALUE_H_ */
