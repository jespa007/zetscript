#include "script/zg_script.h"


CString::CString(){

    m_classStr="string";
    m_pointerClassStr="string";

	m_varType = CVariable::VAR_TYPE::STRING;
	m_value="unknow";
	m_ptr=&m_value;
}

string CString::toString(){string s; return s;}
