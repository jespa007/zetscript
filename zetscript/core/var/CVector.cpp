#include "../../CZetScript.h"


CVector::CVector(){
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassVector(), (void *)this);
	_i_size = 0;
}


CVector::~CVector(){
	for(unsigned i = 0; i < m_objVector.size(); i++){ // unref values ...
		CScriptVariable *var = m_objVector[i];

		if(var->idx_shared_ptr == -1){ // not referenced so we can remove safetely ...
			delete var;
		}
		else{
			CSharedPointerManager::getInstance()->unrefSharedPointer(var->idx_shared_ptr);
		}

		//delete m_value[i];
	}
}

bool CVector::unrefSharedPtr(){

	if(CScriptVariable::unrefSharedPtr()){

		for(unsigned i = 0; i < m_objVector.size(); i++){
			if(!m_objVector[i]->unrefSharedPtr()){
				return false;
			}
		}

		return true;
	}

	return false;
}

bool CVector::refSharedPtr(){

	if(CScriptVariable::refSharedPtr()){

		for(unsigned i = 0; i < m_objVector.size(); i++){
			if(!m_objVector[i]->refSharedPtr()){
				return false;
			}
		}

		return true;
	}

	return false;
}


void CVector::add(CScriptVariable *v){
	m_objVector.push_back(v);
}


int * CVector::size(){

	_i_size = m_objVector.size();
	return &_i_size;
}
