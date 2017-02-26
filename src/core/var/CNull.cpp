#include "../../CZetScript.h"


CNull *CNull::null_symbol = NULL;

CNull * NULL_SYMBOL{
	if(null_symbol == NULL){
		null_symbol = new CNull();
	}

	return null_symbol;

}

void CNull::destroySingletons(){
	if(null_symbol != NULL){
		delete null_symbol;
		null_symbol = NULL;
	}
}

CNull::CNull(){

    //m_classStr="undefined";
   // m_pointerClassStr="undefined";
	init(CScriptClass::getRegisteredClassNull(), (void *)this);

}
