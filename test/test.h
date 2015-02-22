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

std::unique_ptr<casm::Program> generateProgram(int mainFunction) {
	auto alloc = utils::make_unique<utils::Allocator<casm::Instruction>>();
	std::vector<std::unique_ptr<casm::Function>> functions(100);
	for ( int i = 0; i < 100; i++ )
		functions[i] = utils::make_unique<casm::Function>();

	auto id = alloc->alloc<casm::MoveA>(0, alloc->alloc<casm::Return>());
	functions[0]->prepare(id, 1);

	auto twice =
			alloc->alloc<casm::LoadL>(0,
			alloc->alloc<casm::LoadR>(0,
			alloc->alloc<casm::AddInstruction>(
			alloc->alloc<casm::Return>())));
	functions[1]->prepare(twice, 1);


	auto add =
			alloc->alloc<casm::LoadL>(0,
			alloc->alloc<casm::LoadR>(1,
			alloc->alloc<casm::AddInstruction>(
			alloc->alloc<casm::Return>())));
	functions[2]->prepare(add, 2);

	auto triple =
			alloc->alloc<casm::CallFunction>(functions[1].get(), std::vector<unsigned>({0}),
			alloc->alloc<casm::MoveFromA>(1,
			alloc->alloc<casm::CallFunction>(functions[2].get(), std::vector<unsigned>({0, 1}),
			alloc->alloc<casm::Return>())));
	functions[3]->prepare(triple, 2);



	auto ret = alloc->alloc<casm::LoadL>(1,
               alloc->alloc<casm::LoadR>(2,
               alloc->alloc<casm::AddInstruction>(
               alloc->alloc<casm::Return>())));
	auto fiboRec2 =
			alloc->alloc<casm::LoadL>(0,
			alloc->alloc<casm::LoadConstR>(utils::make_unique<casm::IntValue>(2),
			alloc->alloc<casm::SubtractInstruction>(
			alloc->alloc<casm::MoveFromA>(2,
			alloc->alloc<casm::CallFunction>(functions[4].get(), std::vector<unsigned>({2}),
			alloc->alloc<casm::MoveFromA>(2, ret))))));

	auto fiboRec1 =
			alloc->alloc<casm::LoadL>(0,
			alloc->alloc<casm::LoadConstR>(utils::make_unique<casm::IntValue>(1),
			alloc->alloc<casm::SubtractInstruction>(
			alloc->alloc<casm::MoveFromA>(1,
			alloc->alloc<casm::CallFunction>(functions[4].get(), std::vector<unsigned>({1}),
			alloc->alloc<casm::MoveFromA>(1, fiboRec2))))));

	auto fibo =
			alloc->alloc<casm::LoadL>(0,
			alloc->alloc<casm::LoadConstR>(utils::make_unique<casm::IntValue>(2),
			alloc->alloc<casm::LowerThanInstruction>(
			alloc->alloc<casm::MoveFromA>(1,
			alloc->alloc<casm::LoadL>(1,
			alloc->alloc<casm::IfElseInstruction>(
					alloc->alloc<casm::MoveA>(0, alloc->alloc<casm::Return>()),
					fiboRec1
			))))));
	functions[4]->prepare(fibo, 3);

	const casm::Function* main = functions[mainFunction].get();
	return utils::make_unique<casm::Program>(std::move(alloc), std::move(functions), main);
}



#endif /* TEST_TEST_H_ */
