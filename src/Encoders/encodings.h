
#ifndef NAIVEENCODINGASSIGNMENT_H
#define NAIVEENCODINGASSIGNMENT_H
#include "classlist.h"
#include "structs.h"
#include "csolver.h"
#include "mymemory.h"


#define GETVECTORFUNCTIONENCODING(o) (&((Encodings*)o)->funcEncoding)
#define GETVECTORELEMENTENCODING(o) (&((Encodings*)o)->elemEncoding)

struct Encodings{
	HashTableVoidToFuncEncod* voidToFuncEncode;
	HashTableElemToEncod* elemToEncode;
	VectorFunctionEncoding funcEncoding;
	VectorElementEncoding elemEncoding;
};

Encodings* allocEncodings();

//For now, This function just simply goes through elements/functions and 
//assigns a predefined Encoding to each of them
void assignEncoding(CSolver* csolver, Encodings* This);
void encodeFunctionsElements(Encodings* This);
void deleteEncodings(Encodings* This);

#endif /* NAIVEENCODINGASSIGNMENT_H */

