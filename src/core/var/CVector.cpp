#include "core/zg_core.h"

CVector::CVector(tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	_i_size = 0;
}

CVector::~CVector(){
	for(unsigned i = 0; i < m_objVector.size(); i++){ // unref values ...
		CScriptVariable *var = m_objVector[i];
		CSharedPointerManager::getInstance()->unrefSharedPointer(var->idx_shared_ptr);

		//delete m_value[i];
	}
}

void CVector::add(CScriptVariable *v){
	m_objVector.push_back(v);
}


int * CVector::size(){

	_i_size = m_objVector.size();
	return &_i_size;
}
