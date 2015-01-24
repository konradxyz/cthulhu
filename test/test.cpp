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

using namespace std;


class LogOperation : public AcceptRealValueOwnershipOperation {
public:
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto val = this->releaseRealValue();
		auto intVal = std::static_pointer_cast<IntValue>(val);
		LOG(info, intVal->getVal());
		return nullptr;
	}
};

class EqualOperation : public AcceptRealValueOwnershipOperation {
public:
	std::unique_ptr<Operation> perform(TaskQueue* queue) {
		auto val = this->releaseRealValue();
		BOOST_CHECK(*val == *expected);
		return nullptr;
	}
	EqualOperation(std::unique_ptr<RealValue>&& expected) : expected(std::move(expected)) {}
private:
	std::unique_ptr<RealValue> expected;
};



BOOST_AUTO_TEST_CASE(universeInOrder) {
	BOOST_CHECK(2 + 2 == 4);
}

BOOST_AUTO_TEST_CASE(simple) {

	auto op =
			utils::make_unique<TrailingGetOperation>(0) >>=
			utils::make_unique<EqualOperation>(utils::make_unique<IntValue>(3));
	auto env = utils::make_unique<Environment>(1, 0);
	(*env->getValues())[0] = std::make_shared<IntValue>(3);
	op->setEnvironment(std::move(env));
	Executor::perform(std::move(op), nullptr);
}

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
