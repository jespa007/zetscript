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


	CDictionaryScriptVariable::CDictionaryScriptVariable(CZetScript *_zs):CScriptVariable(_zs){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
		init(SCRIPT_CLASS_STRUCT, (void *)this);
	}




	bool CDictionaryScriptVariable::unrefSharedPtr(){ // unref each element...
		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < variable.size(); i++){
				if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
					CScriptVariable *var = (CScriptVariable *)variable[i].varRef;
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



	void CDictionaryScriptVariable::add_attr(const char *attr_name, StackElement  * v){
		if(addVariableSymbol(attr_name,virtual_machine->getCurrent_C_FunctionCall(),NULL,v)==NULL){
			virtual_machine->cancelExecution();
		}
	}

	void CDictionaryScriptVariable::remove_attr(const char *attr_name){

		if(!removeVariableSymbolByName(std::string(attr_name),virtual_machine->getCurrent_C_FunctionCall())){
			virtual_machine->cancelExecution();
		}
	}


	int CDictionaryScriptVariable::size(){
		return  this->variable.size();
	}

	void CDictionaryScriptVariable::destroy(){

		for(unsigned i = 0; i < variable.size(); i++){
			if(variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
				//CScriptVariable *svar = (CScriptVariable *)m_variableSymbol[i].object.varRef;
				//svar->unrefSharedPtr();

				if((variable[i].properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(variable[i].varRef != NULL){
						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptVar(((CScriptVariable *)(variable[i].varRef))->ptr_shared_pointer_node,true);
					}
				}

			}
		}
	}

	CDictionaryScriptVariable::~CDictionaryScriptVariable(){
		destroy();
	}

}
