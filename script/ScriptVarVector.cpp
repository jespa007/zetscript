/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptVarVector::ScriptVarVector(ZetScript *_zs):ScriptVar(_zs){
		this->init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}

	void ScriptVarVector::unrefSharedPtr(){

		ScriptVar::unrefSharedPtr();

		for(unsigned i = 0; i < stk_properties->count; i++){
			StackElement *stk=(StackElement *)stk_properties->items[i];
			ScriptVar *var = (ScriptVar *)stk->var_ref;
			if(var != NULL){
				var->unrefSharedPtr();
			}
		}
	}

	StackElement *ScriptVarVector::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk={NULL,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		stk_properties->push_back((intptr_t)stk);
		return stk;
	}

	void ScriptVarVector::push(StackElement  * _stk){
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		stk_properties->push_back((intptr_t)new_stk);

		// update n_refs +1
		if(_stk->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
			virtual_machine->sharePointer(((ScriptVar *)(_stk->var_ref))->ptr_shared_pointer_node);
		}
	}

	StackElement * ScriptVarVector::pop(){
		return_callc={NULL,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		if(stk_properties->count>0){
			return_callc=*((StackElement *)stk_properties->items[stk_properties->count-1]);
			ScriptVar *var = (ScriptVar *)return_callc.var_ref;
			if(var){
				var->unrefSharedPtr();
			}

			StackElement *stk=(StackElement *)stk_properties->pop_back();
			free(stk);
		}else{
			THROW_RUNTIME_ERROR("pop(): error stack already empty");
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
					if(si->var_ref != 0){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((ScriptVar *)(si->var_ref))->ptr_shared_pointer_node,true);
					}
				}
			}

			free(si);
		}
	}

	ScriptVarVector::~ScriptVarVector(){
		destroy();
	}
}
