/*
 * value.cpp
 *
 *  Created on: Feb 17, 2015
 *      Author: kp
 */

#include "asm/value.h"
#include "asm/program.h"
#include "asm/context.h"
#include "utils/ptr.h"
#include "asm/contextbase.h"
#include "asm/futurecontext.h"

namespace casm {
std::shared_ptr<ValueWrapper> generateFutureWrapper(ContextBase* base) {
	return std::make_shared<ValueWrapper>(
			utils::make_unique<FutureContext>(base));
}
std::shared_ptr<ValueWrapper> generateValueWrapper(
		std::unique_ptr<Value>&& value) {
	return std::make_shared<ValueWrapper>(std::move(value));
}
ValueWrapper::ValueWrapper(std::unique_ptr<FutureContext>&& future) :
		type(FUTURE_VALUE), future(std::move(future)) {
}
ValueWrapper::ValueWrapper(std::unique_ptr<Value>&& value) :
		type(REAL_VALUE), value(std::move(value)) {
}

ValueWrapper::~ValueWrapper() {
}
}
