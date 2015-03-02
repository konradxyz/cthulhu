/*
 * executor.h
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#ifndef ASM_EXECUTOR_H_
#define ASM_EXECUTOR_H_

#include "asm/contextbase.h"
#include "asm/context.h"
#include "asm/program.h"
#include <memory>
#include <vector>
#include <thread>
namespace casm {

// Single executor should be used to execute single program.
class Executor {
private:
	ContextBase base;
	const unsigned threadCount;
	ResultKeeper keeper;
	std::vector<std::thread> threads;
public:
	static void execute(std::unique_ptr<Context>&& context);

	Executor(unsigned taskLimit, unsigned threadCount) : base(taskLimit, threadCount), threadCount(threadCount) {}
	void start(Program* program, int param);
	int join();
	int run(Program* program, int param) {
		start(program, param);
		return join();
	}
};

}



#endif /* ASM_EXECUTOR_H_ */
