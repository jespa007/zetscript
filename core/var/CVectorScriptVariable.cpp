#include "../../CZetScript.h"

namespace zetscript{

	CVectorScriptVariable::CVectorScriptVariable(){
		this->init(CScriptClass::getRegisteredClassVector(), (void *)this);
		_i_size = 0;
	}



	bool CVectorScriptVariable::unrefSharedPtr(){

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

	bool CVectorScriptVariable::initSharedPtr(bool is_assigned){

		if(CScriptVariable::initSharedPtr(is_assigned)){

			for(unsigned i = 0; i < m_objVector.size(); i++){
				zs_print_error_cr("vec symbol.size() > 0. internal error!");
				return false;
				/*if(m_objVector[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
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

	tStackElement *CVectorScriptVariable::push(){
		tStackElement s={STK_PROPERTY_TYPE_UNDEFINED ,NULL,UNDEFINED_SYMBOL};
		m_objVector.push_back(s);
		return &m_objVector[m_objVector.size()-1];
	}

	/*void CVectorScriptVariable::add(const tStackElement & v){
		m_objVector.push_back(v);
	}*/

	void CVectorScriptVariable::add(tStackElement  * v){
		m_objVector.push_back(*v);

		// update n_refs +1
		if(v->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
			CURRENT_VM->sharePointer(((CScriptVariable *)(v->varRef))->ptr_shared_pointer_node);


		}
	}

	void CVectorScriptVariable::pop(){
		tStackElement info={STK_PROPERTY_TYPE_UNDEFINED ,NULL,UNDEFINED_SYMBOL};
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

	/*void CVectorScriptVariable::add(int  * v){
		m_objVector.push_back({STK_PROPERTY_TYPE_INTEGER,(void *)((intptr_t)(*v)),0});
	}
	void CVectorScriptVariable::add(bool * v){
		m_objVector.push_back({STK_PROPERTY_TYPE_BOOLEAN,(void *)((intptr_t)(*v)),0});
	}
	void CVectorScriptVariable::add( string * v){
		CStringScriptVariable *s=new CStringScriptVariable();
		s->m_strValue =*v;
		s->initSharedPtr();
		m_objVector.push_back({STK_PROPERTY_TYPE_STRING,&s->m_strValue,s});
	}
	void CVectorScriptVariable::add(float *f){
		tStackElement v = { STK_PROPERTY_TYPE_NUMBER,0,0 };
		memcpy(v.stkValue,f,sizeof(float));
		v.varRef=0;

		m_objVector.push_back(v);
	}
	void CVectorScriptVariable:: add(CScriptVariable * v){
		m_objVector.push_back({STK_PROPERTY_TYPE_SCRIPTVAR,0,v});
	}*/


	int CVectorScriptVariable::size(){
		return  m_objVector.size();
	}
}
