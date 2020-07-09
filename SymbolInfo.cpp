#include "ZetScript.h"

namespace zetscript{

	StackElement SymbolInfo::PointerRefToStackElement(void *ptr_variable){

		if(ir_var->symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF){

			if(str_int_type_ptr==ir_var->c_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};

			}else if(str_float_type_ptr==ir_var->c_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(str_const_char_type_ptr==ir_var->c_type){

				return {

						ptr_variable,
						0,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING
				};
			}else if(str_string_type_ptr==ir_var->c_type){

				return {

						(void *)((std::string *)ptr_variable)->c_str(),
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(str_bool_type_ptr==ir_var->c_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else{
				ScriptClass *info_registered_class = GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(ir_var->c_type);//  ScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					ScriptVar *var = new ScriptVar(this);
					var->init(info_registered_class,ptr_variable);

					return{

							NULL,
							var,
							MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR(zs_strutils::Format("Variable %s is not c referenced as C symbol",ir_var->symbol->name.c_str()));
		}

		return{
			0,
			NULL,
			MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
	}

}
