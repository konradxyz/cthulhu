/*
 * futurecontext.h
 *
 *  Created on: Mar 2, 2015
 *      Author: kp
 */

#ifndef ASM_FUTURECONTEXT_H_
#define ASM_FUTURECONTEXT_H_

// TODO: really, it should be TaskQueue
#include "asm/value.h"

#include "utils/spinlock.h"

namespace casm {
class ContextBase;
class Context;

struct ContextUpdateRequest {
	std::unique_ptr<Context> context;
	unsigned source;
	unsigned target;

	ContextUpdateRequest(std::unique_ptr<Context>&& context, unsigned source,
			unsigned target) :
			context(std::move(context)), source(source), target(target) {
	}
};

// Note that objects of this class are essentially owned by ValueWrappers.
// ValueWrappers are managed by shared pointers.
// As such, we should not store FutureContext* anywhere, we should retrieve it from wrappers.
class FutureContext {
private:
	// Result - only real value wrappers are allowed here.
	// There is no static check - we need to be careful here.
	std::shared_ptr<ValueWrapper> valueWrapper;
	std::vector<ContextUpdateRequest> requests;
	utils::SpinLock lock;
	ContextBase* const base;
public:
	FutureContext(ContextBase* base) :
			base(base) {
	}
	std::unique_ptr<Context> updateContext(std::unique_ptr<Context>&& context,
			unsigned source, unsigned target);

	void setValueAndWakeAll(std::shared_ptr<ValueWrapper>&& value);
};

}

#endif /* ASM_FUTURECONTEXT_H_ */
