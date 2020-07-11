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
		Init(SCRIPT_CLASS_DICTIONARY(this), (void *)this);
	}

	bool ScriptVarDictionary::unrefSharedPtr(){ // unref each element...
		if(ScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				if(variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					ScriptVar *var = (ScriptVar *)variable[i].var_ref;
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
		if(addVariableSymbol(attr_name,virtual_machine->getCurrent_C_FunctionCall(),NULL,v)==NULL){
			virtual_machine->cancelExecution();
		}
	}

	void ScriptVarDictionary::removeAttr(const char *attr_name){

		if(!removeVariableSymbolByName(std::string(attr_name),virtual_machine->getCurrent_C_FunctionCall())){
			virtual_machine->cancelExecution();
		}
	}


	int ScriptVarDictionary::size(){
		return  this->variable.size();
	}

	void ScriptVarDictionary::destroy(){

		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				if((variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){ // deallocate but not if is c ref
					if(variable[i].var_ref != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((ScriptVar *)(variable[i].var_ref))->ptr_shared_pointer_node,true);
					}
				}
			}
		}
	}

	ScriptVarDictionary::~ScriptVarDictionary(){
		destroy();
	}
}
