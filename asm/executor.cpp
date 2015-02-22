/*
 * executor.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#include "asm/executor.h"
#include "asm/instruction.h"

namespace casm {

void  Executor::execute(std::unique_ptr<Context>&& context) {
	while ( context != nullptr ) {
		context = context->nextInstruction->perform(std::move(context));
	}
}

}

