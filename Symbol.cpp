#include "zetscript.h"

namespace zetscript{

StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol,void *ptr_variable){

		if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF){

			if(k_str_int_type_ptr==symbol->str_native_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};

			}else if(k_str_const_int_type_ptr==symbol->str_native_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY
				};

			}else if(k_str_float_type_ptr==symbol->str_native_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(k_str_const_float_type_ptr==symbol->str_native_type){
				return {
						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY
				};
			}else if(k_str_char_type_ptr==symbol->str_native_type){

				return {
							ptr_variable,
							0,
							MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING
				};
			}else if(k_str_const_char_type_ptr==symbol->str_native_type){

				return {
						ptr_variable,
						0,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY
				};
			}else if(k_str_string_type_ptr==symbol->str_native_type){

				return {
						(void *)((std::string *)ptr_variable)->c_str(),
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(k_str_bool_type_ptr==symbol->str_native_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
				};
			}else if(k_str_const_bool_type_ptr==symbol->str_native_type){
				return {

						0,
						ptr_variable,
						MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY
				};
			}else{
				ScriptClass *info_registered_class = zs->getScriptClassFactory()->getScriptClassByNativeClassPtr(symbol->str_native_type);//  ScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					ScriptObject *var = new ScriptObject(zs);
					var->init(info_registered_class,ptr_variable);

					return{

							NULL,
							var,
							MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C
					};
				}else{
					THROW_RUNTIME_ERROR("Native symbol \"%s\" has type \"%s\" that is not registered",symbol->name.c_str(),symbol->str_native_type.c_str());
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
