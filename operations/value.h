/*
 * values.h
 *
 *  Created on: Jan 14, 2015
 *      Author: kp
 */

#ifndef OPERATIONS_VALUE_H_
#define OPERATIONS_VALUE_H_

class RealValue;

class Function {

};

class Value {
public:
	virtual RealValue* asRealValue() = 0;
	virtual ~Value() {}
	virtual bool operator==(const Value& val) = 0;
};

class RealValue : public Value {
public:
	RealValue* asRealValue() {
		return this;
	}
};
class IntValue : public RealValue {
public:
	IntValue(int i) : val(i) {}

	int getVal() const {
		return val;
	}

	bool operator==(const Value& val) {
		auto ival = dynamic_cast<const IntValue&>(val);
		return getVal() == ival.getVal();
	}

private:
	int val;
};

class AcceptRealValueOwnership {
public:
	std::shared_ptr<RealValue> releaseRealValue() {
		return std::move(realValue);
	}

	void setValue(std::shared_ptr<RealValue>&& realValue) {
		this->realValue.swap(realValue);
	}

private:
	std::shared_ptr<RealValue> realValue;
};


#endif /* OPERATIONS_VALUE_H_ */
