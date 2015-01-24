/*
 * executor.h
 *
 *  Created on: Jan 17, 2015
 *      Author: kp
 */

#ifndef OPERATIONS_EXECUTOR_H_
#define OPERATIONS_EXECUTOR_H_

#include<memory.h>

#include "operations.h"

class Executor {
public:
	static void perform(std::unique_ptr<Operation>&& operation, TaskQueue* queue);

};




#endif /* OPERATIONS_EXECUTOR_H_ */
