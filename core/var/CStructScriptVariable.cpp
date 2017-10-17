/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../../CZetScript.h"

namespace zetscript{

	CStructScriptVariable::CStructScriptVariable(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
		init(CScriptClass::getRegisteredClassStruct(), (void *)this);
	}


	bool CStructScriptVariable::unrefSharedPtr(){ // unref each element...
		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_variableSymbol.size(); i++){
				CScriptVariable *var = (CScriptVariable *)m_variableSymbol[i].object.varRef;
				if(var){
					if(!var->unrefSharedPtr()){
						return false;
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

	int CStructScriptVariable::size(){
		return  this->m_variableSymbol.size();
	}

}
