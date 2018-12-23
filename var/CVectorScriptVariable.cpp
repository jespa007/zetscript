/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../CZetScript.h"

namespace zetscript{

	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);
	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorFilename();


	CVectorScriptVariable::CVectorScriptVariable(){
		this->init(SCRIPT_CLASS_VECTOR, (void *)this);
	}

	bool CVectorScriptVariable::unrefSharedPtr(){

		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_variable.size(); i++){
				CScriptVariable *var = (CScriptVariable *)m_variable[i].varRef;
				if(var != NULL){

					if(!var->unrefSharedPtr()){
						return false;
					}
				}
			}

			return true;
		}

		return false;
	}



	tStackElement *CVectorScriptVariable::push(){
		tStackElement s={STK_PROPERTY_TYPE_UNDEFINED ,NULL,NULL};
		m_variable.push_back(s);
		return &m_variable[m_variable.size()-1];
	}

	void CVectorScriptVariable::push(tStackElement  * v){
		m_variable.push_back(*v);

		// update n_refs +1
		if(v->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
			if(!CURRENT_VM->sharePointer(((CScriptVariable *)(v->varRef))->ptr_shared_pointer_node)){
				return;
			}
		}
	}



	tStackElement * CVectorScriptVariable::pop(){
		return_callc={STK_PROPERTY_TYPE_UNDEFINED ,NULL,NULL};
		if(m_variable.size()>0){
			return_callc=m_variable[m_variable.size()-1];
			CScriptVariable *var = (CScriptVariable *)return_callc.varRef;
			if(var){
				if(!var->unrefSharedPtr()){
					writeErrorMsg(NULL,0,"pop(): error doing unref var");
				}
			}

			m_variable.pop_back();
		}else{
			writeErrorMsg(NULL,0,"pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		return &return_callc;
	}


	int CVectorScriptVariable::size(){
		return  m_variable.size();
	}

	void CVectorScriptVariable::destroy(){


		for(unsigned i = 0; i < m_variable.size(); i++){
			if(m_variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){

				tStackElement si=m_variable[i];

				if((si.properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(si.varRef != NULL){
						// remove property if not referenced anymore
						CURRENT_VM->unrefSharedScriptVar(((CScriptVariable *)(si.varRef))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	CVectorScriptVariable::~CVectorScriptVariable(){
		destroy();
	}
}
