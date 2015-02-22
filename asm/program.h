/*
 * program.h
 *
 *  Created on: Feb 17, 2015
 *      Author: kp
 */

#ifndef ASM_PROGRAM_H_
#define ASM_PROGRAM_H_

#include "asm/instruction.h"
#include "asm/context.h"
#include "utils/allocator.h"

namespace casm {


class Function {
private:
	const Instruction* first;
	unsigned envSize;
	unsigned tmpSize;
public:
	Function(const Instruction* first, unsigned envSize, unsigned tmpSize) : first(first), envSize(envSize), tmpSize(tmpSize) {}
	Function() : first(nullptr), envSize(0), tmpSize(0) {}

	void prepare(const Instruction* first, unsigned envSize, unsigned tmpSize) {
		this->first = first;
		this->envSize = envSize;
		this->tmpSize = tmpSize;
	}

	const Instruction* getInstruction() const {
		return first;
	}

	unsigned getEnvSize() const {
		return envSize;
	}

	unsigned getTmpSize() const {
		return tmpSize;
	}
};

class Program {
private:
	std::unique_ptr<utils::Allocator<Instruction>> allocator;
	std::vector<std::unique_ptr<Function>> functions;
	const Function* main;
public:
	Program(std::unique_ptr<utils::Allocator<Instruction>>&& allocator, std::vector<std::unique_ptr<Function>>&& functions, const Function* main)
		: allocator(std::move(allocator)), functions(std::move(functions)), main(main) {}
	// This function generates context that first executes main function and then stores result in keeper.
	// This function will use allocator to generate additional instructions. Allocator must be kept alive during whole execution of program.
	std::unique_ptr<Context> generateStartingContext(int param, utils::Allocator<Instruction>* allocator, ResultKeeper* keeper) const;

};

}



#endif /* ASM_PROGRAM_H_ */
