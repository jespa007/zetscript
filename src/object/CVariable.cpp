#include "script/zg_script.h"

CVariable::CVariable(){
	m_varType = CVariable::VAR_TYPE::OBJECT;
	m_type = CObject::OBJECT_TYPE::VARIABLE;
	m_ptr=NULL;
}


CVariable::VAR_TYPE CVariable::getVariableType(){
	return m_varType;
}


void CVariable::setPtr(void *ptr){
	m_ptr = ptr;
}


