#include "core/zg_core.h"



void CString::setup(){
	m_value = &m_stringValue;
	m_stringValue = "";
}


CString::CString(){

	setup();
}


CString::CString(tInfoRegisteredClass *info_registered_class, string * _value):CScriptVariable(info_registered_class, this){

	setup();

	if( _value != NULL){
	m_value = _value;
	}

	//m_ptr=&m_value;
}
