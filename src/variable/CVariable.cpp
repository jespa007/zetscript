#include "core/zg_core.h"

CVariable::CVariable(){
	m_varType = CVariable::VAR_TYPE::OBJECT;
	m_ptr=NULL;
	m_strValue="unknown";
	idxClass = -1;
}

CVariable::CVariable(int idxClass, void *ptr_obj, const string & m_pointerClassStr){

	m_varType = CVariable::VAR_TYPE::OBJECT;
	m_strValue="unknown";
	this->idxClass = idxClass;
	this->m_ptr=ptr_obj;
	this->m_pointerClassStr = m_pointerClassStr;

}


CVariable::VAR_TYPE CVariable::getVariableType(){
	return m_varType;
}


void CVariable::setPtr(void *ptr){
	m_ptr = ptr;
}


