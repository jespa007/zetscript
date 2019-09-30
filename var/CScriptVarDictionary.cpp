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


	CScriptVarDictionary::CScriptVarDictionary(CZetScript *_zs):CScriptVar(_zs){//CScriptClass *info_registered_class):CScriptVar(info_registered_class, this){
		init(SCRIPT_CLASS_DICTIONARY, (void *)this);
	}




	bool CScriptVarDictionary::unrefSharedPtr(){ // unref each element...
		if(CScriptVar::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
					CScriptVar *var = (CScriptVar *)variable[i].varRef;
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



	void CScriptVarDictionary::add_attr(const char *attr_name, StackElement  * v){
		if(addVariableSymbol(attr_name,virtual_machine->getCurrent_C_FunctionCall(),NULL,v)==NULL){
			virtual_machine->cancelExecution();
		}
	}

	void CScriptVarDictionary::remove_attr(const char *attr_name){

		if(!removeVariableSymbolByName(std::string(attr_name),virtual_machine->getCurrent_C_FunctionCall())){
			virtual_machine->cancelExecution();
		}
	}


	int CScriptVarDictionary::size(){
		return  this->variable.size();
	}

	void CScriptVarDictionary::destroy(){

		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
				//CScriptVar *svar = (CScriptVar *)m_variableSymbol[i].object.varRef;
				//svar->unrefSharedPtr();

				if((variable[i].properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(variable[i].varRef != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((CScriptVar *)(variable[i].varRef))->ptr_shared_pointer_node,true);
					}
				}

			}
		}
	}

	CScriptVarDictionary::~CScriptVarDictionary(){
		destroy();
	}

}
