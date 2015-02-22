/*
 * executor.h
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#ifndef ASM_EXECUTOR_H_
#define ASM_EXECUTOR_H_

#include "asm/context.h"
#include <memory>
namespace casm {

class Executor {
public:
	static void execute(std::unique_ptr<Context>&& context);
};
}



#endif /* ASM_EXECUTOR_H_ */
