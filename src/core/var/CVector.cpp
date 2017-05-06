#include "../../CZetScript.h"


CVector::CVector(){
	this->init(CScriptClass::getRegisteredClassVector(), (void *)this);
	_i_size = 0;
}


CVector::~CVector(){
	for(unsigned i = 0; i < m_objVector.size(); i++){ // unref values ...

		if(m_objVector[i].properties & INS_PROPERTY_TYPE_SCRIPTVAR){
			CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;

			if(var->ptr_shared_pointer_node == NULL){ // not referenced so we can remove safetely ...
				delete var;
			}
			else{
				CURRENT_VM->unrefSharedPointer(var->ptr_shared_pointer_node);
			}
		}


		//delete m_value[i];
	}
}

bool CVector::unrefSharedPtr(){

	if(CScriptVariable::unrefSharedPtr()){

		for(unsigned i = 0; i < m_objVector.size(); i++){
			if(m_objVector[i].properties & INS_PROPERTY_TYPE_SCRIPTVAR){
				CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;
				if(!var->unrefSharedPtr()){
					return false;
				}
			}
		}

		return true;
	}

	return false;
}

bool CVector::initSharedPtr(){

	if(CScriptVariable::initSharedPtr()){

		for(unsigned i = 0; i < m_objVector.size(); i++){
			if(m_objVector[i].properties & INS_PROPERTY_TYPE_SCRIPTVAR){
				CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;
				if(!var->initSharedPtr()){
					return false;
				}
			}
		}

		return true;
	}

	return false;
}


void CVector::add(const tStackElement & v){
	m_objVector.push_back(v);
}


void CVector::add(int  * v){
	m_objVector.push_back({INS_PROPERTY_TYPE_INTEGER,(void *)(*v),0});
}
void CVector::add(bool * v){
	m_objVector.push_back({INS_PROPERTY_TYPE_BOOLEAN,(void *)v,0});
}
void CVector::add( string * v){
	CString *s=new CString();
	s->m_strValue =*v;
	s->initSharedPtr();
	m_objVector.push_back({INS_PROPERTY_TYPE_STRING,&s->m_strValue,s});
}
void CVector::add(float *f){
	tStackElement v;
	v.properties=INS_PROPERTY_TYPE_NUMBER;
	memcpy(v.stkValue,f,sizeof(float));
	v.varRef=0;

	m_objVector.push_back(v);
}
void CVector:: add(CScriptVariable * v){
	m_objVector.push_back({INS_PROPERTY_TYPE_SCRIPTVAR,0,v});
}


int * CVector::size(){

	_i_size = m_objVector.size();
	return &_i_size;
}
