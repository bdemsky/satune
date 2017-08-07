#include "polarityassignment.h"

void assignPolarityAndBooleanValue(Boolean* boolean){
	GETBOOLEANPOLARITY(boolean) = P_TRUE;
	GETBOOLEANVALUE(boolean) = BV_MUSTBETRUE;
	computePolarityAndBooleanValue(boolean);
}

void computePolarityAndBooleanValue(Boolean* boolean){
	switch( GETBOOLEANTYPE(boolean)){
		case BOOLEANVAR:
		case ORDERCONST:
			return;
		case PREDICATEOP:
			return computePredicatePolarityAndBooleanValue(boolean);
		case LOGICOP:
			return computeLogicOpPolarityAndBooleanValue(boolean);
		default:
			ASSERT(0);
	}
}

void computePredicatePolarityAndBooleanValue(Boolean* boolean){
	ASSERT(GETBOOLEANTYPE(boolean) == PREDICATEOP);
	BooleanPredicate* border= (BooleanPredicate*)boolean;
	border->undefStatus->boolVal = GETBOOLEANVALUE(boolean);
	border->undefStatus->polarity = GETBOOLEANPOLARITY(boolean);
	computePolarityAndBooleanValue(border->undefStatus);
}
void computeLogicOpPolarityAndBooleanValue(Boolean* boolean){
	ASSERT(GETBOOLEANTYPE(boolean) == LOGICOP);
	computeLogicOpBooleanValue(boolean);
	computeLogicOpPolarity(boolean);
	uint size = getSizeArrayBoolean(& ((BooleanLogic*) boolean)->inputs);
	for(uint i=0; i<size; i++){
		computeLogicOpBooleanValue( getArrayBoolean(&((BooleanLogic*) boolean)->inputs, i) );
	}
}
void computeLogicOpPolarity(Boolean* boolean){
	BooleanLogic* This = (BooleanLogic*)boolean;
	switch(This->op){
		case L_AND:
		case L_OR:{
			uint size = getSizeArrayBoolean(& This->inputs);
			for(uint i=0; i<size; i++){
				Boolean* tmp= getArrayBoolean(&This->inputs, i);
				tmp->polarity = computePolarity(tmp->polarity, boolean->polarity);
			}
			break;
		}
		case L_NOT:{
			ASSERT( getSizeArrayBoolean(&This->inputs)==1);
			Boolean* tmp =getArrayBoolean(&This->inputs, 0);
			tmp->polarity = computePolarity(tmp->polarity, negatePolarity( boolean->polarity ) );
			break;
		}case L_XOR:
			ASSERT( getSizeArrayBoolean(&This->inputs)==2);
			getArrayBoolean(&This->inputs, 0)->polarity = P_BOTHTRUEFALSE;
			getArrayBoolean(&This->inputs, 1)->polarity = P_BOTHTRUEFALSE;
			break;
		case L_IMPLIES:{
			ASSERT( getSizeArrayBoolean(&This->inputs)==2);
			Boolean* tmp =getArrayBoolean(&This->inputs, 0);
			tmp->polarity = computePolarity(tmp->polarity, negatePolarity( boolean->polarity ));
			tmp = getArrayBoolean(&This->inputs, 1);  
			tmp->polarity= computePolarity(tmp->polarity, boolean->polarity) ;
			break;
		}
		default:
			ASSERT(0);
	}
	
}
void computeLogicOpBooleanValue(Boolean* boolean){
	BooleanLogic* This = (BooleanLogic*)boolean;
	switch(This->op){
		case L_AND:
		case L_OR:{
			uint size = getSizeArrayBoolean(& This->inputs);
			for(uint i=0; i<size; i++){
				Boolean* tmp= getArrayBoolean(&This->inputs, i);
				tmp->boolVal = computeBooleanValue(This->op, tmp->boolVal, boolean->boolVal);
			}
			return;
		}
		case L_XOR:
			ASSERT( getSizeArrayBoolean(&This->inputs)==2);
			getArrayBoolean(&This->inputs, 0)->boolVal = BV_UNKNOWN;
			getArrayBoolean(&This->inputs, 1)->boolVal = BV_UNKNOWN;
			return;
		case L_NOT:
			ASSERT( getSizeArrayBoolean(&This->inputs)==1);
			Boolean* tmp =getArrayBoolean(&This->inputs, 0);
			tmp->boolVal = computeBooleanValue(This->op, tmp->boolVal,  boolean->boolVal  );
			return;
		case L_IMPLIES:
			ASSERT( getSizeArrayBoolean(&This->inputs)==2);
			Boolean* p1= getArrayBoolean(&This->inputs, 0);
			Boolean* p2= getArrayBoolean(&This->inputs, 1);
			computeImplicationBooleanValue(p1, p2, boolean->boolVal);
			return;
		default:
			ASSERT(0);
	}
}

void computeImplicationBooleanValue(Boolean* first, Boolean* second, BooleanValue parent){
	switch(parent){
		case BV_MUSTBETRUE:
		case BV_UNKNOWN:
			first->boolVal = BV_UNKNOWN;
			second->boolVal = BV_UNKNOWN;
		case BV_MUSTBEFALSE:
			first->boolVal = BV_MUSTBETRUE;
			second->boolVal = BV_MUSTBEFALSE;
		default:
			ASSERT(0);
	}
}

Polarity computePolarity(Polarity childPol, Polarity parentPol){
	switch(childPol){
		case P_UNDEFINED:
			return parentPol;
		case P_TRUE: 
		case P_FALSE:
			if(parentPol == childPol)
				return parentPol;
			else
				return P_BOTHTRUEFALSE;
		case P_BOTHTRUEFALSE:
			return childPol;
		default:
			ASSERT(0);
	}
	exit(-1);
}

BooleanValue computeBooleanValue(LogicOp op, BooleanValue childVal, BooleanValue parentVal ){
	switch(op){
		case L_AND:
			if(childVal == BV_UNDEFINED && parentVal == BV_MUSTBETRUE){
				return parentVal;
			} else if(childVal != parentVal){
				return BV_UNKNOWN;
			} else
				return childVal;
		case L_OR:
			if(childVal == BV_UNDEFINED && parentVal == BV_MUSTBEFALSE){
				return parentVal;
			} else if(childVal != parentVal){
				return BV_UNKNOWN;
			} else 
				return childVal;
		case L_NOT:{
			BooleanValue newVal = negateBooleanValue(parentVal);
			if(childVal == BV_UNDEFINED){
				return newVal;
			} else if(childVal != newVal){
				return BV_UNKNOWN;
			} else 
				return childVal;
		}
		default:
			ASSERT(0);
	}
	exit(-1);
}