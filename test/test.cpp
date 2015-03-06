/*
 * Test.cpp
 *
 *  Created on: Oct 11, 2014
 *      Author: kp
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_LOG_DYN_LINK
#define BOOST_TEST_MODULE cthulhu_test
#include <boost/test/unit_test.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include "utils/ptr.h"
#include "utils/logging.h"

#include "asm/instruction.h"
#include "asm/executor.h"
#include "asm/program.h"
#include "utils/allocator.h"

#include "test/test.h"
#include "asm/executor.h"

using namespace std;


namespace casm {

class LogInstruction : public Instruction {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override {
		auto ptr = context->accumulator.get();
		auto val = static_cast<const IntValue*>(ptr->getValue());
		LOG(info, val->getValue());
		return nullptr;
	}
};


class EqualInstruction : public Instruction {
private:
	int expected;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override {
		auto ptr = context->accumulator.get();
		auto val = static_cast<const IntValue*>(ptr->getValue());
		LOG(info, val->getValue());
		BOOST_ASSERT(expected == val->getValue());
		return nullptr;
	}

	EqualInstruction(int expected) : expected(expected) {}
};


}

std::unique_ptr<casm::Context> generateContext(std::vector<std::shared_ptr<casm::ValueWrapper>>* values, casm::Instruction* instruction) {
	auto res = utils::make_unique<casm::Context>();
	res->allocateFrame(0, 0);
	auto fr = res->currentFrame;
	fr->environment.swap(*values);
	res->nextInstruction = instruction;
	return std::move(res);
}



void programTest(int main, int param, int expected) {
	utils::Allocator<casm::Instruction> alloc;
		casm::ResultKeeper keeper;
		auto program = generateProgram(main);
		auto ctx = program->generateStartingContext(param, &alloc, &keeper);
		casm::Executor::execute(std::move(ctx));
		LOG(debug, keeper.result);
		BOOST_ASSERT(keeper.result == expected);
}

void programTestPar(int main, int param, int expected) {
	auto program = generateProgram(main);
	casm::Executor e(20, 2);
	LOG(debug, "running");
	int result = e.run(program.get(), param);
	LOG(debug, result);
	BOOST_ASSERT(result == expected);
}

utils::SpinLock l;
void lockRun(int* target) {
	for ( unsigned i = 0; i < 100000; ++i ) {
		std::unique_lock<utils::SpinLock> m(l);
		(*target)++;
	}
}


BOOST_AUTO_TEST_CASE(universeInOrder) {
	BOOST_CHECK(2 + 2 == 4);
}




BOOST_AUTO_TEST_CASE(twice6) {
	programTest(6, 13, 26);
}

BOOST_AUTO_TEST_CASE(fibo5) {
	programTest(5, 9, 34);
}


BOOST_AUTO_TEST_CASE(fibo7) {
	programTest(7, 9, 34);
}

BOOST_AUTO_TEST_CASE(fibo8) {
	programTest(8, 9, 34);
}

BOOST_AUTO_TEST_CASE(par_fibo9) {
	programTestPar(9, 9, 34);
}


BOOST_AUTO_TEST_CASE(fibo10) {
	programTestPar(10, 9, 34);
}


/*
BOOST_AUTO_TEST_CASE(threadTest) {
	int target = 0;
	std::vector<std::thread> threads;
	unsigned MAX = 8;
	for ( unsigned i = 0; i < MAX; ++i )
		threads.emplace_back(lockRun, &target);
	for ( unsigned i = 0; i < MAX; ++i )
			threads[i].join();
	LOG(debug, target);
	BOOST_ASSERT(target == MAX * 100000);
	//programTest(8, 9, 34);
}
*/



BOOST_AUTO_TEST_CASE(simple) {
	utils::Allocator<casm::Instruction> a;
	auto op = a.alloc<casm::MoveA>(1, a.alloc<casm::EqualInstruction>(3));
	std::vector<std::shared_ptr<casm::ValueWrapper>> env =
		{ casm::generateValueWrapper(utils::make_unique<casm::IntValue>(2)),
		  casm::generateValueWrapper(utils::make_unique<casm::IntValue>(3))};
	auto ctx = generateContext(&env, op);
	casm::Executor::execute(std::move(ctx));

}

