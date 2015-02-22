/*
 * instruction.h
 *
 *  Created on: Feb 19, 2015
 *      Author: kp
 */

#ifndef ASM_INSTRUCTION_H_
#define ASM_INSTRUCTION_H_

#include "asm/context.h"

namespace casm {


struct ResultKeeper {
	int result;
};


class Instruction {
public:
	virtual std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const = 0;
	virtual ~Instruction(){}
};


class WithContinuation {
private:
	const Instruction* nextInstruction;
public:
	WithContinuation(const Instruction* nextInstruction) : nextInstruction(nextInstruction) {}
	const Instruction* getContinuation() const {
		return nextInstruction;
	}
};

// This instruction loads value to register l/r.
// Possibly blocks.
class Load : public Instruction, public WithContinuation {
private:
	const unsigned source;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context, Accumulator* target) const;
	Load(unsigned source, const Instruction* next) :
		WithContinuation(next), source(source) {}
};

class LoadL : public Load {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	LoadL(unsigned source, const Instruction* next) :
		Load(source, next) {}
};

class LoadR : public Load {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	LoadR(unsigned source, const Instruction* next) :
		Load(source, next) {}
};

class LoadConst : public Instruction, public WithContinuation {
private:
	std::unique_ptr<Value> value;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context, Accumulator* target) const;
	LoadConst(std::unique_ptr<Value>&& value, const Instruction* next) :
		WithContinuation(next), value(std::move(value)) {}
};

class LoadConstL : public LoadConst {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	LoadConstL(std::unique_ptr<Value>&& value, const Instruction* next) :
		LoadConst(std::move(value), next) {}
};

class LoadConstR : public LoadConst {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	LoadConstR(std::unique_ptr<Value>&& value, const Instruction* next) :
		LoadConst(std::move(value), next) {}
};

// MoveA never blocks.
// Note that it removes source from environment.
class MoveA : public Instruction, public WithContinuation {
private:
	const unsigned source;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	MoveA(unsigned source, const Instruction* next) : WithContinuation(next), source(source) {}
};

class MoveFromA : public Instruction, public WithContinuation {
private:
	const unsigned target;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
	MoveFromA(unsigned target, const Instruction* next) : WithContinuation(next), target(target) {}
};

class Return : public Instruction {
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};


template
<typename T>
class OperatorInstruction : public Instruction, public WithContinuation {
private:
	T operation;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override {
		int l = static_cast<const IntValue*>(context->lAccumulator.value)->getValue();
		int r = static_cast<const IntValue*>(context->rAccumulator.value)->getValue();
		int res = operation(l, r);
		context->accumulator = casm::generateValueWrapper(utils::make_unique<IntValue>(res));
		context->nextInstruction = getContinuation();
		return std::move(context);
	}

	OperatorInstruction(const Instruction* next) : WithContinuation(next) {}
};


class LowerThanFunctor {
public:
	int operator() (int l, int r) const {
		return l < r;
	}
};

typedef OperatorInstruction<std::plus<int>> AddInstruction;
typedef OperatorInstruction<std::minus<int>> SubtractInstruction;
typedef OperatorInstruction<LowerThanFunctor> LowerThanInstruction;

// Uses value from LAcc as condition.
class IfElseInstruction : public Instruction {
private:
	const Instruction* trueContinuation;
	const Instruction* falseContinuation;
public:
	IfElseInstruction(const Instruction* trueContinuation, const Instruction* falseContinuation) :
		trueContinuation(trueContinuation), falseContinuation(falseContinuation) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};

class CallFunction : public Instruction, public WithContinuation {
private:
	const Function* function;
	std::vector<unsigned> parameters;
public:
	CallFunction(const Function* function, const std::vector<unsigned>& parameters, const Instruction* next) :
		WithContinuation(next), function(function), parameters(parameters) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};


// This instruction reads value from l, treats it as int and inserts into keeper.
class StoreResult : public Instruction {
private:
	// Never thought that I will use this keyword.
	// Well, things sometimes change.
	mutable ResultKeeper* keeper;
public:
	StoreResult(ResultKeeper* keeper) : keeper(keeper) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};

/*
class LoadR : public Load {

};
*/

};



#endif /* ASM_INSTRUCTION_H_ */
