#include "core/zg_core.h"


CString::CString(tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class){

   // m_classStr=typeid(CString).name();
   // m_pointerClassStr=typeid(CString *).name();

	//m_varType = CVariable::VAR_TYPE::STRING;
	m_value="unknow";
	//m_ptr=&m_value;
}


