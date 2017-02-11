#include "../../CZetScript.h"



void CString::setup(){
	m_value = &m_strValue;
	m_strValue = "";


}

CString::CString(){
	m_strValue = "";
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassString(), (void *)this);
}

CString::CString(const string & s){


	this->init(CScriptClassFactory::getInstance()->getRegisteredClassString(), (void *)this);
	m_strValue = s;
}

