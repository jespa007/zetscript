#include "zetscript.h"

namespace zetscript{

StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol,void *ptr_variable){

		if(symbol->symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF){

			if(k_str_int_type_ptr==symbol->c_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};

			}else if(k_str_float_type_ptr==symbol->c_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(k_str_const_char_type_ptr==symbol->c_type){

				return {
						ptr_variable,
						0,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING
				};
			}else if(k_str_string_type_ptr==symbol->c_type){

				return {
						(void *)((std::string *)ptr_variable)->c_str(),
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(k_str_bool_type_ptr==symbol->c_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else{
				ScriptClass *info_registered_class = zs->getScriptClassFactory()->getScriptClassBy_C_ClassPtr(symbol->c_type);//  ScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					ScriptVar *var = new ScriptVar(zs);
					var->init(info_registered_class,ptr_variable);

					return{

							NULL,
							var,
							MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR("Variable %s is not c referenced as C symbol",symbol->name.c_str());
		}

		return{
			0,
			0,
			MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
	}

}
