#include "core/zg_core.h"

CVector::CVector(tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class){
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
