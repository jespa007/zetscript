#include "core/zg_core.h"
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


void CInteger::setup(){
	m_intValue = 0;
	m_value = &m_intValue;
}

CInteger::CInteger(){
	setup();
}


CInteger::CInteger(tInfoRegisteredClass *info_registered_class,void *_value):CScriptVariable(info_registered_class,this){
	setup();

	if(_value != NULL){
		m_value = _value;
	}
}
