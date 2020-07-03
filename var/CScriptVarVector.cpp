/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace ZetScript{

	void  write_error(const char *filename, int line, const  char  *string_text, ...);
	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorFilename();


	CScriptVarVector::CScriptVarVector(ZetScript *_zs):CScriptVar(_zs){
		this->init(SCRIPT_CLASS_VECTOR, (void *)this);
	}

	bool CScriptVarVector::unrefSharedPtr(){

		if(CScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				CScriptVar *var = (CScriptVar *)variable[i].varRef;
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



	StackElement *CScriptVarVector::push(){
		StackElement s={NULL,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		variable.push_back(s);
		return &variable[variable.size()-1];
	}

	void CScriptVarVector::push(StackElement  * v){
		variable.push_back(*v);

		// update n_refs +1
		if(v->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
			if(!virtual_machine->sharePointer(((CScriptVar *)(v->varRef))->ptr_shared_pointer_node)){
				return;
			}
		}
	}



	StackElement * CScriptVarVector::pop(){
		return_callc={NULL,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		if(variable.size()>0){
			return_callc=variable[variable.size()-1];
			CScriptVar *var = (CScriptVar *)return_callc.varRef;
			if(var){
				if(!var->unrefSharedPtr()){
					write_error(NULL,0,"pop(): error doing unref var");
				}
			}

			variable.pop_back();
		}else{
			write_error(NULL,0,"pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		return &return_callc;
	}


	int CScriptVarVector::size(){
		return  variable.size();
	}

	void CScriptVarVector::destroy(){


		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){

				StackElement si=variable[i];

				if((si.properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(si.varRef != NULL){
						// Remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((CScriptVar *)(si.varRef))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	CScriptVarVector::~CScriptVarVector(){
		destroy();
	}
}
