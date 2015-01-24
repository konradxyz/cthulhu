/*
 * executor.cpp
 *
 *  Created on: Jan 17, 2015
 *      Author: kp
 */

#include "executor.h"


void Executor::perform(std::unique_ptr<Operation>&& operation, TaskQueue* queue) {
	auto op = std::move(operation);
	while ( op != nullptr ) {
		op = op->perform(queue);
	}
}



