/*
 * updaters.h
 *
 *  Created on: Feb 22, 2015
 *      Author: kp
 */

#ifndef ASM_UPDATERS_H_
#define ASM_UPDATERS_H_

#include "asm/context.h"
#include "asm/value.h"


// Following classes are used in OperatorInstruction.

namespace casm {

class IdOperator {
public:
	template<typename T>
	inline T operator()(T t) const {
		return t;
	}
};

class AccUpdater {
public:
	inline void operator()(casm::Context* ctx, int val) const {
		ctx->accumulator = casm::generateValueWrapper(
				utils::make_unique<casm::IntValue>(val));
	}
};

class EnvUpdater {
private:
	unsigned target;
public:
	EnvUpdater(unsigned target) :
			target(target) {
	}
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
	EnvTmpUpdater(const std::pair<unsigned, unsigned>&& target) :
			targetEnv(target.first), targetTmp(target.second) {
	}
	inline void operator()(casm::Context* ctx, int val) const {
		auto ptr = utils::make_unique<casm::IntValue>(val);
		ctx->currentFrame->temporaryValues[targetTmp] = ptr.get();
		ctx->currentFrame->environment[targetEnv] = casm::generateValueWrapper(
				std::move(ptr));

	}
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


}


#endif /* ASM_UPDATERS_H_ */
