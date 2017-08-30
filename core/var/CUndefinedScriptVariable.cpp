#include "../../CZetScript.h"

namespace zetscript{

	CUndefinedScriptVariable *CUndefinedScriptVariable::undefined_symbol = NULL;

	CUndefinedScriptVariable * UNDEFINED_SYMBOL{
		if(undefined_symbol == NULL){
			undefined_symbol = new CUndefinedScriptVariable();
			undefined_symbol->init(CScriptClass::getRegisteredClassUndefined(), undefined_symbol);
		}

		return undefined_symbol;

	}

	void CUndefinedScriptVariable::destroySingletons(){
		if(undefined_symbol != NULL){
			delete undefined_symbol;
			undefined_symbol = NULL;
		}
	}

	CUndefinedScriptVariable::CUndefinedScriptVariable(){ //CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){

	}

}
