/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../../CZetScript.h"

namespace zetscript{

	CStructScriptVariable::CStructScriptVariable(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
		init(CScriptClass::getRegisteredClassStruct(), (void *)this);
	}

	tSymbolInfo * CStructScriptVariable::exist(const char *c){
		for(unsigned i = 0; i < m_variableSymbol.size(); i++){
			//CScriptVariable *var = (CScriptVariable *)m_variableSymbol[i].object.varRef;
			if(m_variableSymbol[i].symbol_value == string(c)){
				return &m_variableSymbol[i];
			}

		}

		return NULL;
	}


	bool CStructScriptVariable::unrefSharedPtr(){ // unref each element...
		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_variableSymbol.size(); i++){
				if(m_variableSymbol[i].object.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
					CScriptVariable *var = (CScriptVariable *)m_variableSymbol[i].object.varRef;
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

	bool CStructScriptVariable::initSharedPtr(bool is_assigned){ // ref every element in list ...
		if(CScriptVariable::initSharedPtr(is_assigned)){

			for(unsigned i = 0; i < m_variableSymbol.size(); i++){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"struct symbol.size() > 0. internal error!");
				return false;
			}
			return true;
		}
		return false;
	}

	void CStructScriptVariable::add_attr(const char *attr_name, tStackElement  * v){
		if(addVariableSymbol(string(attr_name),CURRENT_VM->getCurrentAstNodeCall_C_Function(),v)==NULL){
			CURRENT_VM->cancelExecution();
		}
	}

	void CStructScriptVariable::remove_attr(const char *attr_name){

		if(!removeVariableSymbolByName(string(attr_name),CURRENT_VM->getCurrentAstNodeCall_C_Function())){
			CURRENT_VM->cancelExecution();
		}
	}


	int CStructScriptVariable::size(){
		return  this->m_variableSymbol.size();
	}

	void CStructScriptVariable::destroy(){

		for(unsigned i = 0; i < m_variableSymbol.size(); i++){
			if(m_variableSymbol[i].object.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
				CScriptVariable *svar = (CScriptVariable *)m_variableSymbol[i].object.varRef;
				svar->unrefSharedPtr();
				if(svar->isCreatedByContructor()){
					svar->setDelete_C_ObjectOnDestroy(true);
				}
			}
		}
	}

	CStructScriptVariable::~CStructScriptVariable(){
		destroy();
	}

}
