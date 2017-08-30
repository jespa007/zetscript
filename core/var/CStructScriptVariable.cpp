#include "../../CZetScript.h"

namespace zetscript{

	CStructScriptVariable::CStructScriptVariable(){//CScriptClass *info_registered_class):CScriptVariable(info_registered_class, this){
		init(CScriptClass::getRegisteredClassStruct(), (void *)this);
	}


	bool CStructScriptVariable::unrefSharedPtr(){ // unref each element...
		if(CScriptVariable::unrefSharedPtr()){

			for(unsigned i = 0; i < m_variableSymbol.size(); i++){
				unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(m_variableSymbol[i].object.properties);
				if(((var_type) == INS_PROPERTY_TYPE_SCRIPTVAR || (var_type) == INS_PROPERTY_TYPE_STRING)){
					if(!((CScriptVariable *)m_variableSymbol[i].object.varRef)->unrefSharedPtr()){
						return false;
					}
				}
			}

			return true;
		}

		return false;
	}

	bool CStructScriptVariable::initSharedPtr(){ // ref every element in list ...
		if(CScriptVariable::initSharedPtr()){

			for(unsigned i = 0; i < m_variableSymbol.size(); i++){
				zs_print_error_cr("struct symbol.size() > 0. internal error!");
				return false;

				/*unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(m_variableSymbol[i].object.properties);
				if(((var_type) == INS_PROPERTY_TYPE_SCRIPTVAR || (var_type) == INS_PROPERTY_TYPE_STRING)){
					if(!((CScriptVariable *)m_variableSymbol[i].object.varRef)->initSharedPtr()){
						return false;
					}
				}*/
			}

			return true;
		}

		return false;
	}

}
