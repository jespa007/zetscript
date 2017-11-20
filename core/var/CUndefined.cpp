#include "../../CZetScript.h"

CUndefined *CUndefined::undefined_symbol = NULL;

CUndefined * UNDEFINED_SYMBOL{
	if(undefined_symbol == NULL){
		undefined_symbol = new CUndefined();
		undefined_symbol->init(CScriptClass::getRegisteredClassUndefined(), (void *)undefined_symbol);
	}

	return undefined_symbol;

}

void CUndefined::destroySingletons(){
	if(undefined_symbol != NULL){
		delete undefined_symbol;
		undefined_symbol = NULL;
	}
}

CUndefined::CUndefined(){ //CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){

}
