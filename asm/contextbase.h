/*
 * contextbase.h
 *
 *  Created on: Mar 1, 2015
 *      Author: kp
 */

#ifndef ASM_CONTEXTBASE_H_
#define ASM_CONTEXTBASE_H_

#include <memory>
#include <vector>
#include "utils/spinlock.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "utils/allocator.h"
#include "asm/instruction.h"

#include "utils/logging.h"

namespace casm {

class Context;
class Instruction;


class ContextBase {
private:
	std::atomic_int taskCount;
	int unsafeTaskCount = 0;
	const int maxTaskCount; // This is only a hint, we will not enforce it.
	const int threadCount;
	int waitingThreads = 0;
	// Always taskCount == tasks.size()
	std::vector<std::unique_ptr<Context>> tasks;
	std::mutex lock;
	std::condition_variable waitingForTask;
	utils::Allocator<Instruction> allocator;
	const Instruction* retInstruction;
	const Instruction* generateStartingInstruction();
public:
	ContextBase(int maxTaskCount, int threadCount) : taskCount(0), maxTaskCount(maxTaskCount), threadCount(threadCount) {
		retInstruction = generateStartingInstruction();
	}
	bool isFull() {
		return unsafeTaskCount >= maxTaskCount;
	}

	void stop() {
		std::unique_lock<std::mutex> m(lock);
		for ( int i = 0; i < threadCount; ++i ) {
			tasks.push_back(nullptr);
		}
		waitingForTask.notify_all();
	}

	void pushTask(std::unique_ptr<Context>&& ctx) {
		std::unique_lock<std::mutex> m(lock);
		tasks.push_back(std::move(ctx));
		++unsafeTaskCount;
		waitingForTask.notify_one();
	}

	void pushTasks(std::vector<std::unique_ptr<Context>>* contexts) {
		std::unique_lock<std::mutex> m(lock);
		for (auto& ctx : *contexts) {
			tasks.push_back(std::move(ctx));
			++unsafeTaskCount;
			waitingForTask.notify_one();
		}
	}

	std::unique_ptr<Context> popTask() {
		std::unique_lock<std::mutex> m(lock);
		while ( tasks.size() <= 0 ) {
			++waitingThreads;
			waitingForTask.wait(m);
			--waitingThreads;
		}
		auto res = std::move(tasks.back());
		tasks.pop_back();
		--unsafeTaskCount;
		return std::move(res);
	}

	const Instruction* getRetInstruction() const {
		return retInstruction;
	}

	utils::Allocator<Instruction>* getAllocator() {
		return &allocator;
	}
};

}


#endif /* ASM_CONTEXTBASE_H_ */
