#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptClass *this_object,tInfoRegisteredFunctionSymbol *irsv){

	this->irsv = irsv;
	this->this_object = this_object;
	returnVariable = CScopeInfo::UndefinedSymbol;
}

CObject *	 CScriptFunction::getReturnObject(){

	return returnVariable;
}

CObject ** CScriptFunction::getReturnObjectPtr(){
	return &returnVariable;
}


void CScriptFunction::setReturnObject(CObject *obj){

	returnVariable = obj;
}
