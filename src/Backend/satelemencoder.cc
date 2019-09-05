#include "satencoder.h"
#include "structs.h"
#include "common.h"
#include "ops.h"
#include "element.h"
#include "set.h"
#include "predicate.h"
#include "csolver.h"
#include "tunable.h"
#include <cmath>

void SATEncoder::shouldMemoize(Element *elem, uint64_t val, bool &memo) {
	uint numParents = elem->parents.getSize();
	uint posPolarity = 0;
	uint negPolarity = 0;
	memo = false;
	if (elem->type == ELEMFUNCRETURN) {
		memo = true;
	}
	for (uint i = 0; i < numParents; i++) {
		ASTNode *node = elem->parents.get(i);
		if (node->type == PREDICATEOP) {
			BooleanPredicate *pred = (BooleanPredicate *) node;
			Polarity polarity = pred->polarity;
			FunctionEncodingType encType = pred->encoding.type;
			bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
			if (pred->predicate->type == TABLEPRED) {
				//Could be smarter, but just do default thing for now

				UndefinedBehavior undefStatus = ((PredicateTable *)pred->predicate)->undefinedbehavior;

				Polarity tpolarity = polarity;
				if (generateNegation)
					tpolarity = negatePolarity(tpolarity);
				if (undefStatus == SATC_FLAGFORCEUNDEFINED)
					tpolarity = P_BOTHTRUEFALSE;
				if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_TRUE)
					memo = true;
				if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_FALSE)
					memo = true;
			} else if (pred->predicate->type == OPERATORPRED) {
				if (pred->encoding.type == ENUMERATEIMPLICATIONS || pred->encoding.type == ENUMERATEIMPLICATIONSNEGATE) {
					Polarity tpolarity = polarity;
					if (generateNegation)
						tpolarity = negatePolarity(tpolarity);
					PredicateOperator *predicate = (PredicateOperator *)pred->predicate;
					uint numDomains = pred->inputs.getSize();
					bool isConstant = true;
					for (uint i = 0; i < numDomains; i++) {
						Element *e = pred->inputs.get(i);
						if (elem != e && e->type != ELEMCONST) {
							isConstant = false;
						}
					}
					if (predicate->getOp() == SATC_EQUALS) {
						if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_TRUE)
							posPolarity++;
						if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_FALSE)
							negPolarity++;
					} else {
						if (isConstant) {
							if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_TRUE)
								posPolarity++;
							if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_FALSE)
								negPolarity++;
						} else {
							if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_TRUE)
								memo = true;
							if (tpolarity == P_BOTHTRUEFALSE || tpolarity == P_FALSE)
								memo = true;
						}
					}
				}
			} else {
				ASSERT(0);
			}
		} else if (node->type == ELEMFUNCRETURN) {
			//we are input to function, so memoize negative case
			memo = true;
		} else {
			ASSERT(0);
		}
	}
	if (posPolarity > 1)
		memo = true;
	if (negPolarity > 1)
		memo = true;
}


Edge SATEncoder::getElementValueConstraint(Element *elem, Polarity p, uint64_t value) {
	switch (elem->getElementEncoding()->type) {
	case ONEHOT:
		return getElementValueOneHotConstraint(elem, p, value);
	case UNARY:
		return getElementValueUnaryConstraint(elem, p, value);
	case BINARYINDEX:
		return getElementValueBinaryIndexConstraint(elem, p, value);
	case BINARYVAL:
		return getElementValueBinaryValueConstraint(elem, p, value);
		break;
	default:
		ASSERT(0);
		break;
	}
	return E_BOGUS;
}

bool impliesPolarity(Polarity curr, Polarity goal) {
	return (((int) curr) & ((int)goal)) == ((int) goal);
}

Edge SATEncoder::getElementValueBinaryIndexConstraint(Element *elem, Polarity p, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = elem->getElementEncoding();

	//Check if we need to generate proxy variables
	if (elemEnc->encoding == EENC_UNKNOWN && elemEnc->numVars > 1) {
		bool memo = false;
		shouldMemoize(elem, value, memo);
		if (memo) {
			elemEnc->encoding = EENC_BOTH;
			elemEnc->polarityArray = (Polarity *) ourcalloc(1, sizeof(Polarity) * elemEnc->encArraySize);
			elemEnc->edgeArray = (Edge *) ourcalloc(1, sizeof(Edge) * elemEnc->encArraySize);
		} else {
			elemEnc->encoding = EENC_NONE;
		}
	}

	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			if (elemEnc->numVars == 0)
				return E_True;

			if (elemEnc->encoding != EENC_NONE && elemEnc->numVars > 1) {
				if (impliesPolarity(elemEnc->polarityArray[i], p)) {
					return elemEnc->edgeArray[i];
				} else {
					if (edgeIsNull(elemEnc->edgeArray[i])) {
						elemEnc->edgeArray[i] = constraintNewVar(cnf);
					}
					if (elemEnc->polarityArray[i] == P_UNDEFINED && p == P_BOTHTRUEFALSE) {
						generateProxy(cnf, generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, i), elemEnc->edgeArray[i], P_BOTHTRUEFALSE);
						elemEnc->polarityArray[i] = p;
					} else if (!impliesPolarity(elemEnc->polarityArray[i], P_TRUE)  && impliesPolarity(p, P_TRUE)) {
						generateProxy(cnf, generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, i), elemEnc->edgeArray[i], P_TRUE);
						elemEnc->polarityArray[i] = (Polarity) (((int) elemEnc->polarityArray[i]) | ((int)P_TRUE));
					} else if (!impliesPolarity(elemEnc->polarityArray[i], P_FALSE)  && impliesPolarity(p, P_FALSE)) {
						generateProxy(cnf, generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, i), elemEnc->edgeArray[i], P_FALSE);
						elemEnc->polarityArray[i] = (Polarity) (((int) elemEnc->polarityArray[i]) | ((int)P_FALSE));
					}
					return elemEnc->edgeArray[i];
				}
			}
			return generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, i);
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueOneHotConstraint(Element *elem, Polarity p, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = elem->getElementEncoding();
	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			return (elemEnc->numVars == 0) ? E_True : elemEnc->variables[i];
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueUnaryConstraint(Element *elem, Polarity p, uint64_t value) {
	ASTNodeType type = elem->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN || type == ELEMCONST);
	ElementEncoding *elemEnc = elem->getElementEncoding();
	for (uint i = 0; i < elemEnc->encArraySize; i++) {
		if (elemEnc->isinUseElement(i) && elemEnc->encodingArray[i] == value) {
			if (elemEnc->numVars == 0)
				return E_True;
			if (i == 0)
				return constraintNegate(elemEnc->variables[0]);
			else if ((i + 1) == elemEnc->encArraySize)
				return elemEnc->variables[i - 1];
			else
				return constraintAND2(cnf, elemEnc->variables[i - 1], constraintNegate(elemEnc->variables[i]));
		}
	}
	return E_False;
}

Edge SATEncoder::getElementValueBinaryValueConstraint(Element *element, Polarity p, uint64_t value) {
	ASTNodeType type = element->type;
	ASSERT(type == ELEMSET || type == ELEMFUNCRETURN);
	ElementEncoding *elemEnc = element->getElementEncoding();
	if (elemEnc->low <= elemEnc->high) {
		if (value < elemEnc->low || value > elemEnc->high)
			return E_False;
	} else {
		//Range wraps around 0
		if (value < elemEnc->low && value > elemEnc->high)
			return E_False;
	}

	uint64_t valueminusoffset = value - elemEnc->offset;
	return generateBinaryConstraint(cnf, elemEnc->numVars, elemEnc->variables, valueminusoffset);
}

void allocElementConstraintVariables(ElementEncoding *This, uint numVars) {
	This->numVars = numVars;
	This->variables = (Edge *)ourmalloc(sizeof(Edge) * numVars);
}

void SATEncoder::generateBinaryValueEncodingVars(ElementEncoding *encoding) {
	ASSERT(encoding->type == BINARYVAL);
	allocElementConstraintVariables(encoding, encoding->numBits);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	if (encoding->element->anyValue)
		generateAnyValueBinaryValueEncoding(encoding);
}

void SATEncoder::freezeElementVariables(ElementEncoding *encoding) {
	ASSERT(encoding->element->frozen);
	for (uint i = 0; i < encoding->numVars; i++) {
		Edge e = encoding->variables[i];
		ASSERT(edgeIsVarConst(e));
		freezeVariable(cnf, e);
	}
	for(uint i=0; i< encoding->encArraySize; i++){
		if(encoding->isinUseElement(i) && encoding->encoding != EENC_NONE && encoding->numVars > 1){
			Edge e = encoding->edgeArray[i];
			if(!edgeIsNull(e)){
				ASSERT(edgeIsVarConst(e));
				freezeVariable(cnf, e);
			}
		}
	}
}

void SATEncoder::generateBinaryIndexEncodingVars(ElementEncoding *encoding) {
	ASSERT(encoding->type == BINARYINDEX);
	allocElementConstraintVariables(encoding, NUMBITS(encoding->encArraySize - 1));
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	if (encoding->element->anyValue) {
		uint setSize = encoding->element->getRange()->getSize();
		int maxIndex = getMaximumUsedSize(encoding);
		if (setSize == encoding->encArraySize && maxIndex == (int)setSize) {
			return;
		}
		double ratio = (setSize * (1 + 2 * encoding->numVars)) / (encoding->numVars * (encoding->numVars + maxIndex * 1.0 - setSize));
//		model_print("encArraySize=%u\tmaxIndex=%d\tsetSize=%u\tmetric=%f\tnumBits=%u\n", encoding->encArraySize, maxIndex, setSize, ratio, encoding->numVars);
		if ( ratio <  pow(2, (uint)solver->getTuner()->getTunable(MUSTVALUE, &mustValueBinaryIndex) - 3)) {
			generateAnyValueBinaryIndexEncodingPositive(encoding);
		} else {
			generateAnyValueBinaryIndexEncoding(encoding);
		}
	}
}

void SATEncoder::generateOneHotEncodingVars(ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	for (uint i = 0; i < encoding->numVars; i++) {
		for (uint j = i + 1; j < encoding->numVars; j++) {
			addConstraintCNF(cnf, constraintNegate(constraintAND2(cnf, encoding->variables[i], encoding->variables[j])));
		}
	}
	if (encoding->element->anyValue)
		addConstraintCNF(cnf, constraintOR(cnf, encoding->numVars, encoding->variables));
}

void SATEncoder::generateUnaryEncodingVars(ElementEncoding *encoding) {
	allocElementConstraintVariables(encoding, encoding->encArraySize - 1);
	getArrayNewVarsSATEncoder(encoding->numVars, encoding->variables);
	//Add unary constraint
	for (uint i = 1; i < encoding->numVars; i++) {
		addConstraintCNF(cnf, constraintOR2(cnf, encoding->variables[i - 1], constraintNegate(encoding->variables[i])));
	}
}

void SATEncoder::generateElementEncoding(Element *element) {
	ElementEncoding *encoding = element->getElementEncoding();
	ASSERT(encoding->type != ELEM_UNASSIGNED);
	if (encoding->variables != NULL)
		return;
	switch (encoding->type) {
	case ONEHOT:
		generateOneHotEncodingVars(encoding);
		return;
	case BINARYINDEX:
		generateBinaryIndexEncodingVars(encoding);
		return;
	case UNARY:
		generateUnaryEncodingVars(encoding);
		return;
	case BINARYVAL:
		generateBinaryValueEncodingVars(encoding);
		return;
	default:
		ASSERT(0);
	}
}

int SATEncoder::getMaximumUsedSize(ElementEncoding *encoding) {
	if(encoding->encArraySize == 1){
		return 1;
	}
	for (int i = encoding->encArraySize - 1; i >= 0; i--) {
		if (encoding->isinUseElement(i))
			return i + 1;
	}
	ASSERT(false);
	return -1;
}

void SATEncoder::generateAnyValueBinaryIndexEncoding(ElementEncoding *encoding) {
	if (encoding->numVars == 0)
		return;
	int index = getMaximumUsedSize(encoding);
	if ( index != (int)encoding->encArraySize ) {
		addConstraintCNF(cnf, generateLTValueConstraint(cnf, encoding->numVars, encoding->variables, index));
	}
	for (int i = index - 1; i >= 0; i--) {
		if (!encoding->isinUseElement(i)) {
			addConstraintCNF(cnf, constraintNegate( generateBinaryConstraint(cnf, encoding->numVars, encoding->variables, i)));
		}
	}
}

void SATEncoder::generateAnyValueBinaryIndexEncodingPositive(ElementEncoding *encoding) {
	if (encoding->numVars == 0)
		return;
	Edge carray[encoding->encArraySize];
	uint size = 0;
	for (uint i = 0; i < encoding->encArraySize; i++) {
		if (encoding->isinUseElement(i)) {
			carray[size] = generateBinaryConstraint(cnf, encoding->numVars, encoding->variables, i);
			size++;
		}
	}
	addConstraintCNF(cnf, constraintOR(cnf, size, carray));
}

void SATEncoder::generateAnyValueBinaryValueEncoding(ElementEncoding *encoding) {
	uint64_t minvalueminusoffset = encoding->low - encoding->offset;
	uint64_t maxvalueminusoffset = encoding->high - encoding->offset;
	model_print("This is minvalueminus offset: %lu", minvalueminusoffset);
	Edge lowerbound = generateLTValueConstraint(cnf, encoding->numVars, encoding->variables, maxvalueminusoffset);
	Edge upperbound = constraintNegate(generateLTValueConstraint(cnf, encoding->numVars, encoding->variables, minvalueminusoffset));
	addConstraintCNF(cnf, lowerbound);
	addConstraintCNF(cnf, upperbound);
}

