/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{

	void  write_error(const char *filename, int line, const  char  *string_text, ...);
	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorFilename();


	CVectorScriptVariable::CVectorScriptVariable(CZetScript *_zs):CScriptVariable(_zs){
		this->init(SCRIPT_CLASS_VECTOR, (void *)this);
	}

	bool CVectorScriptVariable::unrefSharedPtr(){

		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				CScriptVariable *var = (CScriptVariable *)variable[i].varRef;
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



	StackElement *CVectorScriptVariable::push(){
		StackElement s={NULL,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		variable.push_back(s);
		return &variable[variable.size()-1];
	}

	void CVectorScriptVariable::push(StackElement  * v){
		variable.push_back(*v);

		// update n_refs +1
		if(v->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
			if(!virtual_machine->sharePointer(((CScriptVariable *)(v->varRef))->ptr_shared_pointer_node)){
				return;
			}
		}
	}



	StackElement * CVectorScriptVariable::pop(){
		return_callc={NULL,NULL,STK_PROPERTY_TYPE_UNDEFINED};
		if(variable.size()>0){
			return_callc=variable[variable.size()-1];
			CScriptVariable *var = (CScriptVariable *)return_callc.varRef;
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


	int CVectorScriptVariable::size(){
		return  variable.size();
	}

	void CVectorScriptVariable::destroy(){


		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){

				StackElement si=variable[i];

				if((si.properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(si.varRef != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((CScriptVariable *)(si.varRef))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	CVectorScriptVariable::~CVectorScriptVariable(){
		destroy();
	}
}
