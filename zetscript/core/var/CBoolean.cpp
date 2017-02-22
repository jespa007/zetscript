#include "../../CZetScript.h"
/*
CBoolean * CBoolean::Parse(const string & s){



		if(CStringUtils::toLower(s)=="true"){
			CBoolean *b=new CBoolean();
			b->m_value=true;
			return b;
			
		}else if(CStringUtils::toLower(s)=="false"){
			CBoolean *b=new CBoolean();
			b->m_value=false;
			return b;
		}

		// TODO: develop exception handler.
		return NULL;
}*/

bool * CBoolean::Parse(const string & s){

	if(CStringUtils::toLower(s)=="true"){
		bool *b=new bool;
		*b=true;
		return b;

	}else if(CStringUtils::toLower(s)=="false"){
		bool *b=new bool;
		*b=false;
		return b;
	}

	// TODO: develop exception handler.
	return NULL;
}



void CBoolean::setup(){

	CScriptVariable::setup();

	m_boolValue = false;
	m_value = &m_boolValue;




}


CBoolean::CBoolean(){
	m_boolValue = false;
	this->init(CScriptClass::getInstance()->getRegisteredClassBoolean(), (void *)this);

}

CBoolean::CBoolean(bool _value){
	m_boolValue = _value;
	this->init(CScriptClass::getInstance()->getRegisteredClassBoolean(), (void *)this);
}
/*
CBoolean::CBoolean(tInfoScriptClass *info_registered_class, bool *_value):CScriptVariable(info_registered_class,this){

    //m_classStr=typeid(CBoolean).name();
    //m_pointerClassStr=typeid(CBoolean *).name();


	setup();

	if( _value != NULL){
	m_value = _value;
	}
	//m_ptr=&m_value;
}*/


