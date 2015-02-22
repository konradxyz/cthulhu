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
#include "operations/operations.h"
#include "operations/executor.h"
#include "utils/ptr.h"
#include "utils/logging.h"

#include "asm/instruction.h"
#include "asm/executor.h"
#include "asm/program.h"
#include "utils/allocator.h"

#include "test/test.h"

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
	res->allocateFrame(0);
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

BOOST_AUTO_TEST_CASE(universeInOrder) {
	BOOST_CHECK(2 + 2 == 4);
}

BOOST_AUTO_TEST_CASE(id) {
	programTest(0, 13, 13);
}


BOOST_AUTO_TEST_CASE(twice) {
	programTest(1, 13, 26);
}



BOOST_AUTO_TEST_CASE(triple) {
	programTest(3, 13, 39);
}

BOOST_AUTO_TEST_CASE(fibo) {
	programTest(4, 9, 34);
}


BOOST_AUTO_TEST_CASE(simple) {
	utils::Allocator<casm::Instruction> a;
	auto op = a.alloc<casm::MoveA>(1, a.alloc<casm::EqualInstruction>(3));
	std::vector<std::shared_ptr<casm::ValueWrapper>> env =
		{ casm::generateValueWrapper(utils::make_unique<casm::IntValue>(2)),
		  casm::generateValueWrapper(utils::make_unique<casm::IntValue>(3))};
	auto ctx = generateContext(&env, op);
	casm::Executor::execute(std::move(ctx));

}

BOOST_AUTO_TEST_CASE(add) {
	utils::Allocator<casm::Instruction> a;
	auto op = a.alloc<casm::LoadL>(1,
			  a.alloc<casm::LoadR>(0,
			  a.alloc<casm::AddInstruction>(
			  a.alloc<casm::EqualInstruction>(5))));
	std::vector<std::shared_ptr<casm::ValueWrapper>> env =
		{ casm::generateValueWrapper(utils::make_unique<casm::IntValue>(2)),
		  casm::generateValueWrapper(utils::make_unique<casm::IntValue>(3))};
	auto ctx = generateContext(&env, op);
	casm::Executor::execute(std::move(ctx));

}

BOOST_AUTO_TEST_CASE(ifelse) {
	utils::Allocator<casm::Instruction> a;
	auto cont = a.alloc<casm::EqualInstruction>(3);
	auto op = a.alloc<casm::LoadL>(0,
			  a.alloc<casm::IfElseInstruction>(
					  a.alloc<casm::MoveA>(0, cont),
					  a.alloc<casm::MoveA>(1, cont)
			  ));
	std::vector<std::shared_ptr<casm::ValueWrapper>> env =
		{ casm::generateValueWrapper(utils::make_unique<casm::IntValue>(0)),
		  casm::generateValueWrapper(utils::make_unique<casm::IntValue>(3))};
	auto ctx = generateContext(&env, op);
	casm::Executor::execute(std::move(ctx));
}

BOOST_AUTO_TEST_CASE(ifelsef) {
	utils::Allocator<casm::Instruction> a;
	auto cont = a.alloc<casm::EqualInstruction>(1);
	auto op = a.alloc<casm::LoadL>(0,
			  a.alloc<casm::IfElseInstruction>(
					  a.alloc<casm::MoveA>(0, cont),
					  a.alloc<casm::MoveA>(1, cont)
			  ));
	std::vector<std::shared_ptr<casm::ValueWrapper>> env =
		{ casm::generateValueWrapper(utils::make_unique<casm::IntValue>(1)),
		  casm::generateValueWrapper(utils::make_unique<casm::IntValue>(3))};
	auto ctx = generateContext(&env, op);
	casm::Executor::execute(std::move(ctx));
}

/*
BOOST_AUTO_TEST_CASE(operatorTest) {
	auto env = utils::make_unique<Environment>(3, 2);
	*env->getValues() = {
			std::make_shared<IntValue>(3),
			std::make_shared<IntValue>(5),
			nullptr
	};
	auto op =
			utils::make_unique<GetRealValueFromEnvOperation>(0) >>=
			utils::make_unique<SetRealValueForOperatorOperation>(0) >>=
			utils::make_unique<GetRealValueFromEnvOperation>(1) >>=
			utils::make_unique<SetRealValueForOperatorOperation>(1) >>=
			utils::make_unique<EvaluateBinaryOperatorOperation<std::plus<int>>>(0, 1, 2) >>=
			utils::make_unique<TrailingGetOperation>(2) >>=
			utils::make_unique<EqualOperation>(utils::make_unique<IntValue>(8));
	op->setEnvironment(std::move(env));
	Executor::perform(std::move(op), nullptr);

}

std::unique_ptr<OperationWrapper> getWrapper(std::unique_ptr<TrailingGetOperation>&& op) {
	auto oper = op.get();
	return utils::make_unique<OperationWrapper>(std::move(op), oper);
}

BOOST_AUTO_TEST_CASE(ifelsetrueTest) {
	auto env = utils::make_unique<Environment>(3, 0);
	*env->getValues() = {
			std::make_shared<IntValue>(1),
			std::make_shared<IntValue>(1),
			std::make_shared<IntValue>(0)
	};
	auto op =
			utils::make_unique<GetRealValueFromEnvOperation>(0) >>=
			utils::make_unique<IfElseTrailingOperation>(getWrapper(utils::make_unique<TrailingGetOperation>(1)), getWrapper(utils::make_unique<TrailingGetOperation>(2))) >>=
			utils::make_unique<EqualOperation>(utils::make_unique<IntValue>(1));
	op->setEnvironment(std::move(env));
	Executor::perform(std::move(op), nullptr);

}


BOOST_AUTO_TEST_CASE(ifelsefalseTest) {
	auto env = utils::make_unique<Environment>(3, 0);
	*env->getValues() = {
			std::make_shared<IntValue>(0),
			std::make_shared<IntValue>(1),
			std::make_shared<IntValue>(0)
	};
	auto op =
			utils::make_unique<GetRealValueFromEnvOperation>(0) >>=
			utils::make_unique<IfElseTrailingOperation>(getWrapper(utils::make_unique<TrailingGetOperation>(1)), getWrapper(utils::make_unique<TrailingGetOperation>(2))) >>=
			utils::make_unique<EqualOperation>(utils::make_unique<IntValue>(0));
	op->setEnvironment(std::move(env));
	Executor::perform(std::move(op), nullptr);

}
*/
