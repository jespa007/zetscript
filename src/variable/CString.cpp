#include "core/zg_core.h"


CString::CString(){

    m_classStr=typeid(CString).name();
    m_pointerClassStr=typeid(CString *).name();

	m_varType = CVariable::VAR_TYPE::STRING;
	m_value="unknow";
	m_ptr=&m_value;
}


