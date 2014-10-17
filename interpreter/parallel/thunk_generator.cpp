/*
 * thunk_generator.cpp
 *
 *  Created on: Oct 17, 2014
 *      Author: kp
 */

#include "thunk_generator.h"
#include "utils/memory.h"
namespace thunk_generator {

void ThunkGenerator::visitLet(const ast::Let* p) {
	locals->push_back( { Generate(p->assigned), false });
	SetResult(Generate(p->value));
}

void ThunkGenerator::visitApply(const ast::Apply* p) {
	auto function = Generate(p->function);
	auto param = Generate(p->param);
	auto result = memory_manager->Allocate(
			utils::make_unique<thunk::ApplyThunk>(function, param));
	SetResult(result);
}

void ThunkGenerator::visitInt(const ast::Int* p) {
	SetResult(
			memory_manager->Allocate(
					utils::make_unique<thunk::IntThunk>(p->value)));
}

void ThunkGenerator::visitIf(const ast::If* p) {
	auto condition = Generate(p->condition);
	auto true_val = Generate(p->true_val);
	auto false_val = Generate(p->false_val);
	SetResult(
			memory_manager->Allocate(
					utils::make_unique<thunk::IfThunk>(condition, true_val,
							false_val)));
}

void ThunkGenerator::visitGlobal(const ast::Global* p) {
	SetResult(
			memory_manager->Allocate(
					utils::make_unique<thunk::GlobalThunk>(p->global)));
}

void ThunkGenerator::visitLocal(const ast::Local* p) {
	if ((*locals)[p->offset].second) {
		(*locals)[p->offset].first->incRefCounter();
	} else {
		(*locals)[p->offset].second = true;
	}
	SetResult((*locals)[p->offset].first);
}

void ThunkGenerator::visitPrimitive(const ast::Primitive* p) {
	SetResult(
			memory_manager->Allocate(
					utils::make_unique<thunk::OperatorThunk>(p->definition)));
}

thunk::ThunkPtr ThunkGenerator::Generate(ast::Exp* e) {
	e->accept(this);
	return result;
}

}

