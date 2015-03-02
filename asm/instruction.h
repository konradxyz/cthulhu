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

namespace seq {
class Load : public Instruction, public WithContinuation {
private:
	const unsigned source;
	const unsigned target;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const;
	Load(unsigned source, unsigned target, const Instruction* next) :
		WithContinuation(next), source(source), target(target) {}
};
}

namespace par {
class Load : public Instruction, public WithContinuation {
private:
	const unsigned source;
	const unsigned target;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const;
	Load(unsigned source, unsigned target, const Instruction* next) :
		WithContinuation(next), source(source), target(target) {}
};

class CallFunctionPar : public Instruction, public WithContinuation {
private:
	const Function* function;
	std::vector<unsigned> parameters;
public:
	CallFunctionPar(const Function* function, const std::vector<unsigned>& parameters, const Instruction* next) :
		WithContinuation(next), function(function), parameters(parameters) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};

class SetValueAndWake : public Instruction {
private:
	const unsigned futureWrapperId;
	const unsigned valueWrapperId;
public:
	SetValueAndWake(unsigned futureWrapperId, unsigned valueWrapperId)
		: futureWrapperId(futureWrapperId), valueWrapperId(valueWrapperId) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;

};

class StoreResult : public Instruction {
private:
	// Never thought that I will use this keyword.
	// Well, things sometimes change.
	mutable ResultKeeper* keeper;
	unsigned source;
public:
	StoreResult(ResultKeeper* keeper, unsigned source) : keeper(keeper), source(source) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};



}


template
<typename OpType, typename LGet, typename RGet, typename Updater>
class OperatorInstruction : public Instruction, public WithContinuation {
private:
	OpType op;
	LGet left;
	RGet right;
	Updater updater;
public:
	template<typename L, typename R, typename U>
	OperatorInstruction(L left, R right, U u, const Instruction* next) :
			WithContinuation(next), left(left), right(right), updater(u) {
	}
	template<typename L, typename R>
	OperatorInstruction(L left, R right, const Instruction* next) :
			WithContinuation(next), left(left), right(right) {
	}

	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override {
		auto ctx = context.get();
		int result = op(left(ctx), right(ctx));
		updater(ctx, result);
		ctx->nextInstruction = getContinuation();
		return std::move(context);
	}
};


class IfElseInstruction : public Instruction {
private:
	unsigned conditionIndex;
	const Instruction* trueContinuation;
	const Instruction* falseContinuation;
public:
	IfElseInstruction(unsigned conditionIndex, const Instruction* trueContinuation, const Instruction* falseContinuation) :
		conditionIndex(conditionIndex), trueContinuation(trueContinuation), falseContinuation(falseContinuation) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};


// MoveA never blocks.
// Note that it removes source from environment.
// TODO: remove it? Maybe it should be merged with return.
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


class LowerThanFunctor {
public:
	int operator() (int l, int r) const {
		return l < r;
	}
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
	unsigned source;
public:
	StoreResult(ResultKeeper* keeper, unsigned source) : keeper(keeper), source(source) {}
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const override;
};

/*
class LoadR : public Load {

};
*/

};



#endif /* ASM_INSTRUCTION_H_ */
