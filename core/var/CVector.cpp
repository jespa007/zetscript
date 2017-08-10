#include "../../CZetScript.h"

namespace zetscript{

	CVector::CVector(){
		this->init(CScriptClass::getRegisteredClassVector(), (void *)this);
		_i_size = 0;
	}



	bool CVector::unrefSharedPtr(){

		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_objVector.size(); i++){
				CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;
				if(var != NULL){
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
				zs_print_error_cr("vec symbol.size() > 0. internal error!");
				return false;
				/*if(m_objVector[i].properties & INS_PROPERTY_TYPE_SCRIPTVAR){
					CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;
					if(!var->initSharedPtr()){
						return false;
					}
				}*/
			}

			return true;
		}

		return false;
	}

	tStackElement *CVector::push(){
		tStackElement s={INS_PROPERTY_TYPE_UNDEFINED ,NULL,NULL};
		m_objVector.push_back(s);
		return &m_objVector[m_objVector.size()-1];
	}

	void CVector::add(const tStackElement & v){
		m_objVector.push_back(v);
	}


	void CVector::add(int  * v){
		m_objVector.push_back({INS_PROPERTY_TYPE_INTEGER,(void *)((intptr_t)(*v)),0});
	}
	void CVector::add(bool * v){
		m_objVector.push_back({INS_PROPERTY_TYPE_BOOLEAN,(void *)((intptr_t)(*v)),0});
	}
	void CVector::add( string * v){
		CString *s=new CString();
		s->m_strValue =*v;
		s->initSharedPtr();
		m_objVector.push_back({INS_PROPERTY_TYPE_STRING,&s->m_strValue,s});
	}
	void CVector::add(float *f){
		tStackElement v = { INS_PROPERTY_TYPE_NUMBER,0,0 };
		memcpy(v.stkValue,f,sizeof(float));
		v.varRef=0;

		m_objVector.push_back(v);
	}
	void CVector:: add(CScriptVariable * v){
		m_objVector.push_back({INS_PROPERTY_TYPE_SCRIPTVAR,0,v});
	}


	int CVector::size(){
		return  m_objVector.size();
	}
}
