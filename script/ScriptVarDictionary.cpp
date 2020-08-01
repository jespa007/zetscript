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


	ScriptVarDictionary::ScriptVarDictionary(ZetScript *_zs):ScriptVar(_zs){//ScriptClass *info_registered_class):ScriptVar(info_registered_class, this){
		init(SCRIPT_CLASS_DICTIONARY(this), (void *)this);
	}

	bool ScriptVarDictionary::unrefSharedPtr(){ // unref each element...
		if(ScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < stk_properties->count; i++){
				StackElement *stk=(StackElement *)stk_properties->items[i];
				if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					ScriptVar *var = (ScriptVar *)stk->var_ref;
					if(var){
						if(!var->unrefSharedPtr()){
							return false;
						}
					}
				}
			}
			return true;
		}
		return false;
	}

	void ScriptVarDictionary::addAttr(const char *attr_name, StackElement  * v){
		if(addProperty(attr_name,virtual_machine->getCurrent_C_FunctionCall(),NULL,v)==NULL){
			virtual_machine->cancelExecution();
		}
	}

	void ScriptVarDictionary::removeAttr(const char *attr_name){

		if(!eraseProperty(std::string(attr_name),virtual_machine->getCurrent_C_FunctionCall())){
			virtual_machine->cancelExecution();
		}
	}


	int ScriptVarDictionary::size(){
		return  this->stk_properties->count;
	}

	void ScriptVarDictionary::destroy(){

		for(unsigned i = 0; i < stk_properties->count; i++){
			StackElement *stk = (StackElement *)stk_properties->items[i];
			if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				if((stk->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){ // deallocate but not if is c ref
					if(stk->var_ref != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((ScriptVar *)(stk->var_ref))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	ScriptVarDictionary::~ScriptVarDictionary(){
		destroy();
	}
}
