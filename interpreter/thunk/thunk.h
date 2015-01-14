/*
 * classure.h
 *
 *  Created on: Oct 17, 2014
 *      Author: kp
 */

#ifndef INTERPRETER_PARALLEL_THUNK_H_
#define INTERPRETER_PARALLEL_THUNK_H_

#include "utils/memory.h"
#include "ast/ast.h"

namespace thunk {

class ApplyThunk;
class IfThunk;
class IntThunk;
class Thunk;
class GlobalThunk;
class OperatorThunk;
class ThunkVisitor;
class ThunkWrapper;

typedef memory::ManagedPtr<ThunkWrapper> ThunkPtr;

class Thunk {
public:
	virtual ~Thunk() {
	}
	virtual std::string descr() = 0;
	virtual void accept(ThunkVisitor& v) = 0;
	virtual bool evaluated() {
		return false;
	}
	virtual int toInt() {
		return 0;
	}
	virtual std::unique_ptr<Thunk> clone() = 0;

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
	//ThunkPtr next;
	std::unique_ptr<Thunk> expr;
public:
	/*ThunkWrapper(ThunkPtr next, std::unique_ptr<Thunk> expr) :
	 next(next), expr(expr) {
	 }*/
	ThunkWrapper(std::unique_ptr<Thunk> expr) :
			expr(expr.release()) {
	}

	Thunk* getExpr() const {
		return expr.get();
	}
	std::unique_ptr<Thunk> releaseExpr() {
		return std::unique_ptr<Thunk>(expr.release());
	}
	void setExpr(std::unique_ptr<Thunk> expr) {
		this->expr.swap(expr);
	}

	/*ThunkPtr getNext() const {
	 return next;
	 }*/

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
	std::unique_ptr<Thunk> clone() {
		function->incRefCounter();
		param->incRefCounter();
		return utils::make_unique<ApplyThunk>(function, param);
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
	std::unique_ptr<Thunk> clone() {
		condition->incRefCounter();
		true_val->incRefCounter();
		false_val->incRefCounter();
		return utils::make_unique<IfThunk>(condition, true_val, false_val);
	}
};

class IntThunk: public Thunk {
private:
	int val;
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

	bool evaluated() override {
		return true;
	}
	int toInt() {
		return val;
	}
	std::unique_ptr<Thunk> clone() {
		return utils::make_unique<IntThunk>(val);
	}

};

class GlobalThunk: public Thunk {
private:
	const ast::Function* function;
public:
	GlobalThunk(const ast::Function* function_param) :
			function(function_param) {
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
	std::unique_ptr<Thunk> clone() {
		return utils::make_unique<GlobalThunk>(function);
	}
};

class OperatorThunk: public Thunk {
private:
	const ast::Operator* operator_ptr;
public:
	OperatorThunk(const ast::Operator* operator_ptr) :
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
	std::unique_ptr<Thunk> clone() {
		return utils::make_unique<OperatorThunk>(operator_ptr);
	}
};

}

#endif /* INTERPRETER_PARALLEL_THUNK_H_ */
