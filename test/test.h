/*
 * test.h
 *
 *  Created on: Feb 20, 2015
 *      Author: kp
 */

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include "operations/operations.h"
#include "operations/executor.h"
#include "utils/ptr.h"
#include "utils/logging.h"

#include "asm/instruction.h"
#include "asm/executor.h"
#include "asm/program.h"
#include "utils/allocator.h"

#include "asm/updaters.h"

std::unique_ptr<casm::Program> generateProgram(int mainFunction) {
	auto alloc = utils::make_unique<utils::Allocator<casm::Instruction>>();
	std::vector<std::unique_ptr<casm::Function>> functions(100);
	for (int i = 0; i < 100; i++)
		functions[i] = utils::make_unique<casm::Function>();

	auto id = alloc->alloc<casm::MoveA>(0, alloc->alloc<casm::Return>());
	functions[0]->prepare(id, 1, 2);

	{

		auto ret = alloc->alloc<casm::seq::Load>(1, 1,
				alloc->alloc<casm::seq::Load>(2, 2,
						alloc->alloc<
								casm::OperatorInstruction<std::plus<int>,
										casm::TmpValueGet,
										casm::TmpValueGet,
										casm::AccUpdater>>(1, 2,
								alloc->alloc<casm::Return>())));

		auto fiboRec2 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::AccUpdater>>(0, 2,
				alloc->alloc<casm::MoveFromA>(2,
						alloc->alloc<casm::CallFunction>(functions[5].get(),
								std::vector<unsigned>( { 2 }),
								alloc->alloc<casm::MoveFromA>(2, ret))));

		auto fiboRec1 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::AccUpdater>>(0, 1,
				alloc->alloc<casm::MoveFromA>(1,
						alloc->alloc<casm::CallFunction>(functions[5].get(),
								std::vector<unsigned>( { 1 }),
								alloc->alloc<casm::MoveFromA>(1, fiboRec2))));

		auto fibo =
				alloc->alloc<casm::seq::Load>(0, 0,
						alloc->alloc<
								casm::OperatorInstruction<
										casm::LowerThanFunctor,
										casm::TmpValueGet,
										casm::ConstGet,
										casm::AccUpdater>>(0, 2,
								alloc->alloc<casm::MoveFromA>(1,
										alloc->alloc<casm::seq::Load>(1, 1,
												alloc->alloc<
														casm::IfElseInstruction>(
														1,
														alloc->alloc<casm::MoveA>(
																0,
																alloc->alloc<
																		casm::Return>()),
														fiboRec1)))));
		functions[5]->prepare(fibo, 3, 3);

		auto twice = alloc->alloc<casm::seq::Load>(0, 0,
				alloc->alloc<
						casm::OperatorInstruction<std::plus<int>,
								casm::TmpValueGet, casm::TmpValueGet,
								casm::AccUpdater>>(0, 0,
						alloc->alloc<casm::Return>()));
		functions[6]->prepare(twice, 1, 1);
	}

	{
		auto ret = alloc->alloc<casm::seq::Load>(1, 1,
				alloc->alloc<casm::seq::Load>(2, 2,
						alloc->alloc<
								casm::OperatorInstruction<std::plus<int>,
										casm::TmpValueGet,
										casm::TmpValueGet,
										casm::AccUpdater>>(1, 2,
								alloc->alloc<casm::Return>())));

		auto fiboRec2 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::EnvUpdater>>(0, 2, 2,
				alloc->alloc<casm::CallFunction>(functions[7].get(),
						std::vector<unsigned>( { 2 }),
						alloc->alloc<casm::MoveFromA>(2, ret)));

		auto fiboRec1 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::EnvUpdater>>(0, 1, 1,
				alloc->alloc<casm::CallFunction>(functions[7].get(),
						std::vector<unsigned>( { 1 }),
						alloc->alloc<casm::MoveFromA>(1, fiboRec2)));

		auto fibo = alloc->alloc<casm::seq::Load>(0, 0,
				alloc->alloc<
						casm::OperatorInstruction<casm::LowerThanFunctor,
								casm::TmpValueGet, casm::ConstGet,
								casm::EnvUpdater>>(0, 2, 1,
						alloc->alloc<casm::seq::Load>(1, 1,
								alloc->alloc<casm::IfElseInstruction>(1,
										alloc->alloc<casm::MoveA>(0,
												alloc->alloc<casm::Return>()),
										fiboRec1))));
		functions[7]->prepare(fibo, 3, 3);

	}

	{
		auto ret = alloc->alloc<casm::seq::Load>(1, 1,
				alloc->alloc<casm::seq::Load>(2, 2,
						alloc->alloc<
								casm::OperatorInstruction<std::plus<int>,
										casm::TmpValueGet,
										casm::TmpValueGet,
										casm::AccUpdater>>(1, 2,
								alloc->alloc<casm::Return>())));

		auto fiboRec2 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::EnvUpdater>>(0, 2, 2,
				alloc->alloc<casm::CallFunction>(functions[8].get(),
						std::vector<unsigned>( { 2 }),
						alloc->alloc<casm::MoveFromA>(2, ret)));

		auto fiboRec1 = alloc->alloc<
				casm::OperatorInstruction<std::minus<int>,
						casm::TmpValueGet, casm::ConstGet,
						casm::EnvUpdater>>(0, 1, 1,
				alloc->alloc<casm::CallFunction>(functions[8].get(),
						std::vector<unsigned>( { 1 }),
						alloc->alloc<casm::MoveFromA>(1, fiboRec2)));

		auto fibo = alloc->alloc<casm::seq::Load>(0, 0,
				alloc->alloc<
						casm::OperatorInstruction<casm::LowerThanFunctor,
								casm::TmpValueGet, casm::ConstGet,
								casm::EnvTmpUpdater>>(0, 2,
						std::make_pair(1, 1),
						alloc->alloc<casm::IfElseInstruction>(1,
								alloc->alloc<casm::MoveA>(0,
										alloc->alloc<casm::Return>()),
								fiboRec1)));
		functions[8]->prepare(fibo, 3, 3);

	}


	{
			auto ret = alloc->alloc<casm::par::Load>(1, 1,
					alloc->alloc<casm::par::Load>(2, 2,
							alloc->alloc<
									casm::OperatorInstruction<std::plus<int>,
											casm::TmpValueGet,
											casm::TmpValueGet,
											casm::AccUpdater>>(1, 2,
									alloc->alloc<casm::Return>())));

			auto fiboRec2 = alloc->alloc<
					casm::OperatorInstruction<std::minus<int>,
							casm::TmpValueGet, casm::ConstGet,
							casm::EnvUpdater>>(0, 2, 2,
					alloc->alloc<casm::CallFunction>(functions[9].get(),
							std::vector<unsigned>( { 2 }),
							alloc->alloc<casm::MoveFromA>(2, ret)));

			auto fiboRec1 = alloc->alloc<
					casm::OperatorInstruction<std::minus<int>,
							casm::TmpValueGet, casm::ConstGet,
							casm::EnvUpdater>>(0, 1, 1,
					alloc->alloc<casm::par::CallFunctionPar>(functions[9].get(),
							std::vector<unsigned>( { 1 }),
							alloc->alloc<casm::MoveFromA>(1, fiboRec2)));

			auto fibo = alloc->alloc<casm::par::Load>(0, 0,
					alloc->alloc<
							casm::OperatorInstruction<casm::LowerThanFunctor,
									casm::TmpValueGet, casm::ConstGet,
									casm::EnvTmpUpdater>>(0, 2,
							std::make_pair(1, 1),
							alloc->alloc<casm::IfElseInstruction>(1,
									alloc->alloc<casm::MoveA>(0,
											alloc->alloc<casm::Return>()),
									fiboRec1)));
			functions[9]->prepare(fibo, 3, 3);

		}


	const casm::Function* main = functions[mainFunction].get();
	return utils::make_unique<casm::Program>(std::move(alloc),
			std::move(functions), main);
}

#endif /* TEST_TEST_H_ */
