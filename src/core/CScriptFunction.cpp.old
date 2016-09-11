#include "zg_core.h"

CScriptFunction::CScriptFunction(CScriptClass *this_object,tInfoRegisteredFunctionSymbol *irsv){

	this->irsv = irsv;
	this->this_object = this_object;
	returnVariable = CScopeInfo::UndefinedSymbol;
}

CVariable *	 CScriptFunction::getReturnObject(){

	return returnVariable;
}

CVariable ** CScriptFunction::getReturnObjectPtr(){
	return &returnVariable;
}


void CScriptFunction::setReturnObject(CVariable *obj){

	returnVariable = obj;
}
