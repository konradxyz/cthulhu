/*
 * futurecontext.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: kp
 */

#include "asm/futurecontext.h"
#include "asm/contextbase.h"
#include "asm/context.h"


namespace casm {
std::unique_ptr<Context> FutureContext::updateContext(
		std::unique_ptr<Context>&& context, unsigned source, unsigned target) {
	std::lock_guard<utils::SpinLock> guard(lock);
	if (valueWrapper.use_count() > 0 ) {
		auto frame = context->currentFrame;
		frame->environment[source] = valueWrapper;
		frame->temporaryValues[target] = valueWrapper->getValue();
		return std::move(context);
	} else {
		requests.emplace_back(std::move(context), source, target);
		return nullptr;
	}

}

void FutureContext::setValueAndWakeAll(std::shared_ptr<ValueWrapper>&& value) {
	std::lock_guard<utils::SpinLock> guard(lock);
	valueWrapper = std::move(value);
	std::vector<std::unique_ptr<Context>> contexts;
	for (auto& request : requests) {
		auto frame = request.context->currentFrame;
		frame->environment[request.source] = valueWrapper;
		frame->temporaryValues[request.target] = valueWrapper->getValue();
		contexts.emplace_back(std::move(request.context));
	}
	base->pushTasks(&contexts);
}

}


