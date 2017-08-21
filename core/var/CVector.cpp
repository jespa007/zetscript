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
				if(var != NULL && var != UNDEFINED_SYMBOL){

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
		tStackElement s={INS_PROPERTY_TYPE_UNDEFINED ,NULL,UNDEFINED_SYMBOL};
		m_objVector.push_back(s);
		return &m_objVector[m_objVector.size()-1];
	}

	/*void CVector::add(const tStackElement & v){
		m_objVector.push_back(v);
	}*/

	void CVector::add(tStackElement  * v){
		m_objVector.push_back(*v);

		// update n_refs +1
		if(v->properties&INS_PROPERTY_TYPE_SCRIPTVAR){
			CURRENT_VM->sharePointer(((CScriptVariable *)(v->varRef))->ptr_shared_pointer_node);


		}
	}

	void CVector::pop(){
		tStackElement info={INS_PROPERTY_TYPE_UNDEFINED ,NULL,UNDEFINED_SYMBOL};
		if(m_objVector.size()>0){
			info=m_objVector[m_objVector.size()-1];

			CScriptVariable *var = (CScriptVariable *)info.varRef;
			if(var != NULL && var != UNDEFINED_SYMBOL){
				if(!var->unrefSharedPtr()){
					zs_print_error("pop(): error doing unref var");
				}
			}

			m_objVector.pop_back();
		}else{
			zs_print_error("pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		CURRENT_VM->setCallResult(&info);
	}

	/*void CVector::add(int  * v){
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
	}*/


	int CVector::size(){
		return  m_objVector.size();
	}
}
