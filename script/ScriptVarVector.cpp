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
		this->init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}

	bool ScriptVarVector::unrefSharedPtr(){

		if(ScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < stk_properties->count; i++){
				StackElement *stk=(StackElement *)stk_properties->items[i];
				ScriptVar *var = (ScriptVar *)stk->var_ref;
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
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk={NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		stk_properties->push_back((intptr_t)stk);
		return stk;
	}

	void ScriptVarVector::push(StackElement  * _stk){
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		stk_properties->push_back((intptr_t)new_stk);

		// update n_refs +1
		if(_stk->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
			if(!virtual_machine->sharePointer(((ScriptVar *)(_stk->var_ref))->ptr_shared_pointer_node)){
				return;
			}
		}
	}

	StackElement * ScriptVarVector::pop(){
		return_callc={NULL,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		if(stk_properties->count>0){
			return_callc=*((StackElement *)stk_properties->items[stk_properties->count-1]);
			ScriptVar *var = (ScriptVar *)return_callc.var_ref;
			if(var){
				if(!var->unrefSharedPtr()){
					writeError(NULL,0,"pop(): error doing unref var");
				}
			}

			StackElement *stk=(StackElement *)stk_properties->pop_back();
			free(stk);
		}else{
			writeError(NULL,0,"pop(): error stack already empty");
		}

		// due the call is void we are doing the operations behind...
		return &return_callc;
	}


	int ScriptVarVector::size(){
		return  stk_properties->count;
	}

	void ScriptVarVector::destroy(){

		for(unsigned i = 0; i < stk_properties->count; i++){
			StackElement *si=(StackElement *)stk_properties->items[i];
			if(si->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				if((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){ // deallocate but not if is c ref
					if(si->var_ref != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((ScriptVar *)(si->var_ref))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	ScriptVarVector::~ScriptVarVector(){
		destroy();
	}
}
