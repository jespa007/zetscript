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


	CStructScriptVariable::CStructScriptVariable(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
		init(SCRIPT_CLASS_STRUCT, (void *)this);
	}




	bool CStructScriptVariable::unrefSharedPtr(){ // unref each element...
		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_variable.size(); i++){
				if(m_variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
					CScriptVariable *var = (CScriptVariable *)m_variable[i].varRef;
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



	void CStructScriptVariable::add_attr(const char *attr_name, StackElement  * v){
		if(addVariableSymbol(attr_name,CURRENT_VM->getCurrent_C_FunctionCall(),NULL,v)==NULL){
			CURRENT_VM->cancelExecution();
		}
	}

	void CStructScriptVariable::remove_attr(const char *attr_name){

		if(!removeVariableSymbolByName(std::string(attr_name),CURRENT_VM->getCurrent_C_FunctionCall())){
			CURRENT_VM->cancelExecution();
		}
	}


	int CStructScriptVariable::size(){
		return  this->m_variable.size();
	}

	void CStructScriptVariable::destroy(){

		for(unsigned i = 0; i < m_variable.size(); i++){
			if(m_variable[i].properties & STK_PROPERTY_TYPE_SCRIPTVAR){
				//CScriptVariable *svar = (CScriptVariable *)m_variableSymbol[i].object.varRef;
				//svar->unrefSharedPtr();

				if((m_variable[i].properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR){ // deallocate but not if is c ref
					if(m_variable[i].varRef != NULL){
						// remove property if not referenced anymore
						CURRENT_VM->unrefSharedScriptVar(((CScriptVariable *)(m_variable[i].varRef))->ptr_shared_pointer_node,true);
					}
				}

			}
		}
	}

	CStructScriptVariable::~CStructScriptVariable(){
		destroy();
	}

}
