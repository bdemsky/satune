#ifndef FUNCTIONENCODER_H
#define FUNCTIONENCODER_H
#include "classlist.h"

enum FunctionEncoderType {
	ENUMERATEIMPLICATIONS
};

typedef enum FunctionEncoderType FunctionEncoderType;

struct FunctionEncoder {
	FunctionEncoderType type;
	Function * function;
};

FunctionEncoder * allocFunctionEncoder(FunctionEncoderType type, Function *function);
void deleteFunctionEncoder(FunctionEncoder *this);
#endif
