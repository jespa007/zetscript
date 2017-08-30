#include "../../CZetScript.h"

namespace zetscript{

	CNullScriptVariable *CNullScriptVariable::null_symbol = NULL;

	CNullScriptVariable * NULL_SYMBOL{
		if(null_symbol == NULL){
			null_symbol = new CNullScriptVariable();
			null_symbol->init(CScriptClass::getRegisteredClassNull(), (void *)null_symbol);
		}

		return null_symbol;

	}

	void CNullScriptVariable::destroySingletons(){
		if(null_symbol != NULL){
			delete null_symbol;
			null_symbol = NULL;
		}
	}

	CNullScriptVariable::CNullScriptVariable(){

		//m_classStr="undefined";
	   // m_pointerClassStr="undefined";


	}

}
