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
	virtual std::unique_ptr<Operation> perform(TaskQueue* ) = 0;
	virtual ~Operation() {
	}
};

class AcceptEnvironment {
public:
	const Environment* getEnvironment() const {
		return environment.get();
	}

	void setEnvironment(const std::unique_ptr<Environment>& environment) {
		this->environment = environment;
	}

private:
	std::unique_ptr<Environment> environment;
};

#endif /* OPERATIONS_OPERATIONS_H_ */
