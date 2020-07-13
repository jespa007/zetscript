/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);
	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorSourceFilename();


	ScriptVarVector::ScriptVarVector(ZetScript *_zs):ScriptVar(_zs){
		this->Init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}

	bool ScriptVarVector::unrefSharedPtr(){

		if(ScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				ScriptVar *var = (ScriptVar *)variable[i].var_ref;
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



	StackElement *ScriptVarVector::newSlot(){
		StackElement s={NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		variable.push_back(s);
		return &variable[variable.size()-1];
	}

	void ScriptVarVector::push(StackElement  * v){
		variable.push_back(*v);

		// update n_refs +1
		if(v->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
			if(!virtual_machine->sharePointer(((ScriptVar *)(v->var_ref))->ptr_shared_pointer_node)){
				return;
			}
		}
	}

	StackElement * ScriptVarVector::pop(){
		return_callc={NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		if(variable.size()>0){
			return_callc=variable[variable.size()-1];
			ScriptVar *var = (ScriptVar *)return_callc.var_ref;
			if(var){
				if(!var->unrefSharedPtr()){
					writeError(NULL,0,"pop(): error doing unref var");
				}
			}

			variable.pop_back();
		}else{
			writeError(NULL,0,"pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		return &return_callc;
	}


	int ScriptVarVector::size(){
		return  variable.size();
	}

	void ScriptVarVector::destroy(){


		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){

				StackElement si=variable[i];

				if((si.properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){ // deallocate but not if is c ref
					if(si.var_ref != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((ScriptVar *)(si.var_ref))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	ScriptVarVector::~ScriptVarVector(){
		destroy();
	}
}
