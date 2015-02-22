/*
 * value.cpp
 *
 *  Created on: Feb 17, 2015
 *      Author: kp
 */

#include "asm/value.h"
#include "asm/program.h"


namespace casm {
std::shared_ptr<ValueWrapper> generateFutureWrapper() {
	return std::make_shared<ValueWrapper>();
}
std::shared_ptr<ValueWrapper> generateValueWrapper(std::unique_ptr<Value>&& value) {
	return std::make_shared<ValueWrapper>(std::move(value));
}

}
