/*
 * types.h
 *
 *  Created on: Mar 18, 2015
 *      Author: kp
 */

#ifndef AST_TYPES_H_
#define AST_TYPES_H_

#include <string>
#include <vector>

namespace ast {

class Type {
public:
	virtual ~Type(){}
};

class IntegerType : public Type {
};

class ParamType : public Type {
private:
	std::string name;
};

class VariantType : public Type {
private:
	VariantTypeTemplate* typeTemplate;
	std::vector<std::unique_ptr<Type>> params;
};

class FunctionType : public Type {
private:
	std::unique_ptr<Type> param;
	std::unique_ptr<Type> result;
};

class VariantConstructor {
private:
	std::string name;
};

class VariantTypeTemplate {
private:
	std::string name;
	unsigned paramCount;
	std::vector<>
};



}



#endif /* AST_TYPES_H_ */
