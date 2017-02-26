#include "../../CZetScript.h"

CVoid *CVoid::void_symbol = NULL;

CVoid * CVoid::getInstance(){
	if(void_symbol == NULL){
		void_symbol = new CVoid();
	}

	return void_symbol;

}

void CVoid::destroySingletons(){
	if(void_symbol != NULL){
		delete void_symbol;
		void_symbol = NULL;
	}
}

CVoid::CVoid(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
	init(CScriptClass::getRegisteredClassVoid(), (void *)this);
}
