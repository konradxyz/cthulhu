/*
 * executor.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#include "asm/executor.h"
#include "asm/instruction.h"
#include "utils/logging.h"

namespace casm {

namespace {
void executeThread(ContextBase* base) {
	LOG(debug, "running thread");
	auto ctx = base->popTask();
	while (ctx != nullptr ) {
		LOG(debug, "next task running thread");
		Executor::execute(std::move(ctx));
		ctx = base->popTask();
	}
}
}

void  Executor::execute(std::unique_ptr<Context>&& context) {
	while ( context != nullptr ) {
		context = context->nextInstruction->perform(std::move(context));
	}
}




void Executor::start(Program* program, int param) {
	auto ctx = program->generateStartingContextPar(param, &base, base.getAllocator(), &keeper);
	base.pushTask(std::move(ctx));
	for ( unsigned i = 0; i < threadCount; ++i ) {
		threads.emplace_back(executeThread, &base);
	}
}


int Executor::join() {
	for ( auto& thread : threads ) {
		thread.join();
	}
	return keeper.result;
}

}

