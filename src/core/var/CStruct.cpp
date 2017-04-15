#include "../../CZetScript.h"



CStruct::CStruct(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
	init(CScriptClass::getRegisteredClassStruct(), (void *)this);
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

bool CStruct::initSharedPtr(){ // ref every element in list ...
	if(CScriptVariable::initSharedPtr()){

		for(unsigned i = 0; i < m_variableSymbol.size(); i++){
			if(!((CScriptVariable *)m_variableSymbol[i].object)->initSharedPtr()){
				return false;
			}
		}

		return true;
	}

	return false;
}
