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

namespace dev {

class Load : public Instruction, public WithContinuation {
private:
	const unsigned source;
	const unsigned target;
public:
	std::unique_ptr<Context> perform(std::unique_ptr<Context>&& context) const;
	Load(unsigned source, unsigned target, const Instruction* next) :
		WithContinuation(next), source(source), target(target) {}
};

class ConstGet {
private:
	int value;
public:
	ConstGet(int value) : value(value) {}
	int operator()(casm::Context* ctx) const {
		return value;
	}
};

class TmpValueGet {
private:
	unsigned index;
public:
	TmpValueGet(unsigned index) : index(index) {}
	int operator() (casm::Context* ctx) const {
		return static_cast<const casm::IntValue*>(ctx->currentFrame->temporaryValues[index])->getValue();
	}
};

class AccUpdater {
public:
	inline void operator() (casm::Context* ctx, int val) const {
		ctx->accumulator = casm::generateValueWrapper(utils::make_unique<casm::IntValue>(val));
	}
};

class EnvUpdater {
private:
	unsigned target;
public:
	EnvUpdater(unsigned target) : target(target) {}
	inline void operator()(casm::Context* ctx, int val) const {
		ctx->currentFrame->environment[target] = casm::generateValueWrapper(
				utils::make_unique<casm::IntValue>(val));
	}
};

class EnvTmpUpdater {
private:
	unsigned targetEnv;
	unsigned targetTmp;
public:
	EnvTmpUpdater(const std::pair<unsigned, unsigned>&& target) : targetEnv(target.first), targetTmp(target.second) {}
	inline void operator()(casm::Context* ctx, int val) const {
		auto ptr = utils::make_unique<casm::IntValue>(val);
		ctx->currentFrame->temporaryValues[targetTmp] = ptr.get();
		ctx->currentFrame->environment[targetEnv] = casm::generateValueWrapper(std::move(ptr));

	}
};


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


// Uses value from LAcc as condition.
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



}


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
