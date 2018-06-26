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
		this->init(CScriptClass::getRegisteredClassVector(), (void *)this);
		_i_size = 0;
	}



	bool CVectorScriptVariable::unrefSharedPtr(){

		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_objVector.size(); i++){
				CScriptVariable *var = (CScriptVariable *)m_objVector[i].varRef;
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

	bool CVectorScriptVariable::initSharedPtr(bool is_assigned){

		if(CScriptVariable::initSharedPtr(is_assigned)){

			for(unsigned i = 0; i < m_objVector.size(); i++){
				//writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"vec symbol.size() > 0. internal error!");
				//return false;
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
		tStackElement s={STK_PROPERTY_TYPE_UNDEFINED ,NULL,NULL};
		m_objVector.push_back(s);
		return &m_objVector[m_objVector.size()-1];
	}

	void CVectorScriptVariable::add(tStackElement  * v){
		m_objVector.push_back(*v);

		// update n_refs +1
		if(v->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
			if(!CURRENT_VM->sharePointer(((CScriptVariable *)(v->varRef))->ptr_shared_pointer_node)){
				return;
			}
		}
	}

	tStackElement * CVectorScriptVariable::pop(){
		return_callc={STK_PROPERTY_TYPE_UNDEFINED ,NULL,NULL};
		if(m_objVector.size()>0){
			return_callc=m_objVector[m_objVector.size()-1];
			CScriptVariable *var = (CScriptVariable *)return_callc.varRef;
			if(var){
				if(!var->unrefSharedPtr()){
					writeErrorMsg(NULL,0,"pop(): error doing unref var");
				}
			}

			m_objVector.pop_back();
		}else{
			writeErrorMsg(NULL,0,"pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		return &return_callc;
	}


	int CVectorScriptVariable::size(){
		return  m_objVector.size();
	}

	void CVectorScriptVariable::destroy(){


		for(unsigned i = 0; i < m_objVector.size(); i++){
			if(m_objVector[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){

				tStackElement si=m_objVector[i];

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
