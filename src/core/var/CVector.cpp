#include "core/zg_core.h"

CVector::CVector(tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	_i_size = 0;

	//m_varType = CVariable::VAR_TYPE::VECTOR;
   // m_classStr=typeid(CVector).name();
    //m_pointerClassStr=typeid(CVector *).name();

	//m_ptr = &m_value;
}

CVector::~CVector(){
	/*for(int i = 0; i < m_value.size(); i++){
		delete m_value[i];
	}*/
}

void CVector::add(CScriptVariable *v){
	m_objVector.push_back(v);
}


int * CVector::size(){

	/*void *ptr_obj = (void *)get_C_ObjectPtr();
	if(ptr_obj != NULL){
	_i_size = ((CVector *)ptr_obj)->m_objVector.size();
	}
*/
	_i_size = m_objVector.size();
	return &_i_size;
}
