/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace zetscript{

	void  WriteError(const char *filename, int line, const  char  *string_text, ...);
	int GetErrorLine();
	const char * GetErrorDescription();
	const char * GetErrorSourceFilename();


	ScriptVarDictionary::ScriptVarDictionary(ZetScript *_zs):ScriptVar(_zs){//ScriptClass *info_registered_class):ScriptVar(info_registered_class, this){
		init(SCRIPT_CLASS_DICTIONARY, (void *)this);
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



	void ScriptVarDictionary::add_attr(const char *attr_name, StackElement  * v){
		if(addVariableSymbol(attr_name,virtual_machine->GetCurrent_C_FunctionCall(),NULL,v)==NULL){
			virtual_machine->CancelExecution();
		}
	}

	void ScriptVarDictionary::remove_attr(const char *attr_name){

		if(!removeVariableSymbolByName(std::string(attr_name),virtual_machine->GetCurrent_C_FunctionCall())){
			virtual_machine->CancelExecution();
		}
	}


	int ScriptVarDictionary::size(){
		return  this->variable.size();
	}

	void ScriptVarDictionary::destroy(){

		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				//ScriptVar *svar = (ScriptVar *)m_variableSymbol[i].object.var_ref;
				//svar->unrefSharedPtr();

				if((variable[i].properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){ // deallocate but not if is c ref
					if(variable[i].var_ref != NULL){
						// Remove property if not referenced anymore
						virtual_machine->UnrefSharedScriptVar(((ScriptVar *)(variable[i].var_ref))->ptr_shared_pointer_node,true);
					}
				}

			}
		}
	}

	ScriptVarDictionary::~ScriptVarDictionary(){
		destroy();
	}

}
