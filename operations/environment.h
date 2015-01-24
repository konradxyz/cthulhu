/*
 * environment.h
 *
 *  Created on: Jan 14, 2015
 *      Author: kp
 */

#ifndef OPERATIONS_ENVIRONMENT_H_
#define OPERATIONS_ENVIRONMENT_H_

#include <vector>
#include <memory>
#include "value.h"

class Environment {
private:
	std::vector<std::shared_ptr<Value>> values;
	std::vector<RealValue*> operatorValues;
public:
	std::vector<std::shared_ptr<Value>>* getValues() {
		return &values;
	}
	std::vector<RealValue*>* getOperatorValues() {
		return &operatorValues;
	}
	// Note that this constructor should only be used during tests.
	// It copies shared ptrs. Moving pointers from one env to another
	// should be preferred instead.
	//	Environment(const std::vector<std::shared_ptr<Value>>& values) : values(values) {}
	Environment(int size, int operatorSize) :
			values(size), operatorValues(operatorSize) {
	}
};

class AcceptEnvironment {
public:
	std::unique_ptr<Environment> releaseEnvironment() {
		return std::move(environment);
	}

	void setEnvironment(std::unique_ptr<Environment>&& environment) {
		this->environment = std::move(environment);
	}

private:
	std::unique_ptr<Environment> environment;
};



#endif /* OPERATIONS_ENVIRONMENT_H_ */
