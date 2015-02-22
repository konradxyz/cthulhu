/*
 * program.h
 *
 *  Created on: Feb 13, 2015
 *      Author: kp
 */

#ifndef OPERATIONS_PROGRAM_H_
#define OPERATIONS_PROGRAM_H_
class Program;
class Function;

#include "operations/operations.h"

class OperationWrapper {
public:
private:
	std::unique_ptr<AcceptEnvironmentOperation> operation;
	TrailingOperaion* trailingOperation;
};

class Function {
public:
	std::unique_ptr<AcceptEnvironmentOperation> generateOperation(std::unique_ptr<AcceptRealValueOwnershipOperation>&& continuation);
	std::unique_ptr<Environment> generateEnvironment();
private:
	unsigned parameterCount;
	unsigned environmentSize;
};


class Program {
private:
	std::vector<Function> functions;
	const Function* main;
};


#endif /* OPERATIONS_PROGRAM_H_ */
