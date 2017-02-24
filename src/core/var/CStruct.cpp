#include "../../CZetScript.h"



CStruct::CStruct(){//tInfoScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
	this->init(CScriptClass::getInstance()->getRegisteredClassStruct(), (void *)this);
}


bool CStruct::unrefSharedPtr(){ // unref each element...
	if(CScriptVariable::unrefSharedPtr()){

		for(unsigned i = 0; i < m_variableSymbol.size(); i++){
			if(!((CScriptVariable *)m_variableSymbol[i].object)->unrefSharedPtr()){
				return false;
			}
		}

		return true;
	}

	return false;
}

bool CStruct::refSharedPtr(){ // ref every element in list ...
	if(CScriptVariable::refSharedPtr()){

		for(unsigned i = 0; i < m_variableSymbol.size(); i++){
			if(!((CScriptVariable *)m_variableSymbol[i].object)->refSharedPtr()){
				return false;
			}
		}

		return true;
	}

	return false;
}
