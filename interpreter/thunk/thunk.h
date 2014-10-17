/*
 * classure.h
 *
 *  Created on: Oct 17, 2014
 *      Author: kp
 */

#ifndef INTERPRETER_PARALLEL_THUNK_H_
#define INTERPRETER_PARALLEL_THUNK_H_

#include "utils/memory.h"

namespace thunk {

class ApplyThunk;
class IfThunk;
class IntThunk;
class Thunk;
class GlobalThunk;
class BuiltInThunk;
class ThunkThunkPtr;

typedef memory::ManagedPtr<ThunkWrapper> ThunkPtr;

class Thunk {
public:
	virtual ~Thunk() {
	}
	virtual std::string descr() = 0;
};

// Note that we could replace virtual methods with some kind of template.
// We can later check performance.
class ThunkVisitor {
public:
	virtual void visitApply(ApplyThunk* p) = 0;
	virtual void visitInt(IntThunk* p) = 0;
	virtual void visitIf(IfThunk* p) = 0;
	virtual void visitGlobal(GlobalThunk* p) = 0;
	virtual void visitOperator(OperatorThunk* p) = 0;
	virtual ~ThunkVisitor() {
	}
};

#define SINGLE 0
#define MULTIPLE 1

class ThunkWrapper {
private:
	char ref_counter = SINGLE;
	ThunkPtr next;
	std::unique_ptr<Thunk> expr;
public:
	ThunkWrapper(ThunkPtr next, std::unique_ptr<Thunk> expr) :
			next(next), expr(expr) {
	}
	ThunkWrapper(std::unique_ptr<Thunk> expr) :
			expr(expr) {
	}

	Thunk* getExpr() const {
		return expr.get();
	}

	ThunkPtr getNext() const {
		return next;
	}

	bool isSingleReference() const {
		return ref_counter == SINGLE;
	}

	void incRefCounter() {
		ref_counter = MULTIPLE;
	}

	std::string descr() {
		return "wrapper";
	}
};

class ApplyThunk: public Thunk {
private:
	ThunkPtr function;
	ThunkPtr param;
public:
	ApplyThunk(ThunkPtr function, ThunkPtr param) :
			function(function), param(param) {
	}
	void accept(ThunkVisitor& v) {
		v.visitApply(this);
	}
	std::string descr() {
		return "apply " + function->descr() + " " + param->descr();
	}

	ThunkPtr getFunction() const {
		return function;
	}

	ThunkPtr getParam() const {
		return param;
	}
};

class IfThunk: public Thunk {
private:
	ThunkPtr condition;
	ThunkPtr true_val;
	ThunkPtr false_val;
public:
	IfThunk(ThunkPtr condition, ThunkPtr true_val, ThunkPtr false_val) :
			condition(condition), true_val(true_val), false_val(false_val) {
	}
	void accept(ThunkVisitor& v) {
		v.visitIf(this);
	}
	std::string descr() {
		return "if";
	}

	ThunkPtr getCondition() const {
		return condition;
	}

	ThunkPtr getFalseVal() const {
		return false_val;
	}

	ThunkPtr getTrueVal() const {
		return true_val;
	}
};

class IntThunk: public Thunk {
private:
	int val;
	bool evaluated() {
		return true;
	}
public:
	IntThunk(int val) :
			val(val) {
	}
	void accept(ThunkVisitor& v) {
		v.visitInt(this);
	}
	std::string descr() {
		return ("int" + std::to_string(val));
	}
	int getVal() const {
		return val;
	}
};

class GlobalThunk: public Thunk {
private:
	const ast::Function* function;
public:
	GlobalThunk(ast::Function* function) :
			function(function) {
	}
	void accept(ThunkVisitor& v) {
		v.visitGlobal(this);
	}
	std::string descr() {
		return "global ";
	}

	const ast::Function* getFunction() const {
		return function;
	}
};

class OperatorThunk: public Thunk {
private:
	const ast::Operator* operator_ptr;
public:
	OperatorThunk(ast::Operator* operator_ptr) :
			operator_ptr(operator_ptr) {
	}
	void accept(ThunkVisitor& v) {
		v.visitOperator(this);
	}

	std::string descr() {
		return "builtin";
	}

	const ast::Operator* getOperatorPtr() const {
		return operator_ptr;
	}
};

}

#endif /* INTERPRETER_PARALLEL_THUNK_H_ */
