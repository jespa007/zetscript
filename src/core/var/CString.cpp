#include "core/zg_core.h"



void CString::setup(){
	m_value = &m_stringValue;
	m_stringValue = "";
}


CString::CString(){

	setup();
}


CString::CString(tInfoRegisteredClass *info_registered_class, string * _value):CScriptVariable(info_registered_class, this){

   // m_classStr=typeid(CString).name();
   // m_pointerClassStr=typeid(CString *).name();

	//m_varType = CVariable::VAR_TYPE::STRING;

	setup();

	if( _value != NULL){
	m_value = _value;
	}

	//m_ptr=&m_value;
}
