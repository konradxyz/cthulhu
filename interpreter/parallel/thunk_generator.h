/*
 * thunk_generator.h
 *
 *  Created on: Oct 17, 2014
 *      Author: kp
 */

#ifndef INTERPRETER_PARALLEL_THUNK_GENERATOR_H_
#define INTERPRETER_PARALLEL_THUNK_GENERATOR_H_
#include "thunk.h"
#include "ast/ast.h"
namespace thunk_generator {

class ThunkGenerator : public ast::ExpVisitor {
public:
	  void visitLet(const ast::Let* p);
	  void visitApply(const ast::Apply* p);
	  void visitInt(const ast::Int* p);
	  void visitIf(const ast::If* p);
	  void visitGlobal(const ast::Global* p);
	  void visitLocal(const ast::Local* p);
	  void visitPrimitive(const ast::Primitive* p);
	  thunk::ThunkPtr Generate(ast::Exp* e);
	  ThunkGenerator
private:
	  // Second element tells whether this ThunkPtr has already been created.
	  std::vector<std::pair<thunk::ThunkPtr, bool>>* locals;
	  thunk::ThunkPtr result;
	  void SetResult(thunk::ThunkPtr ptr) { result = ptr; }
	  memory::MemoryManager<thunk::ThunkWrapper>* memory_manager;
};

}

#endif /* INTERPRETER_PARALLEL_THUNK_GENERATOR_H_ */
