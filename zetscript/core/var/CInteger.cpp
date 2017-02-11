#include "../../CZetScript.h"
/*
CInteger * CInteger::Parse(const string & val){

	CInteger *n=NULL;
	int type_number = CStringUtils::isNumber(val);
	int numberValue=0;
	if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 10);
	}else if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 16);
	}else{
		return NULL;
	}


   	n=new CInteger();
   	n->m_value = numberValue;

	// TODO: develop exception handler.
	return n;
}
*/

void CInteger::setup(){

	CScriptVariable::setup();

	m_intValue = 0;
	m_value = &m_intValue;
}

int *  CInteger::Parse(const string & val){

	int *n=NULL;
	int type_number = CStringUtils::isNumber(val);
	int numberValue=0;
	if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 10);
	}else if(type_number == CStringUtils::STRING_IS_HEXA){
		numberValue=strtol(val.c_str(), NULL, 16);
	}else{
		return NULL;
	}

    n=new int;
    *n = numberValue;


	// TODO: develop exception handler.
	return n;
}

std::string * CInteger::toString(){
	return &this->m_strValue;
}


CInteger::CInteger(){
	m_intValue = 0;
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassInteger(), (void *)this);
}

CInteger::CInteger(int i){
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassInteger(), (void *)this);
	m_intValue = i;
}


