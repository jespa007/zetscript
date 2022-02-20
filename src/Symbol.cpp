/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol, void *ptr_variable) {

		if ((symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0) {
			THROW_RUNTIME_ERROR("Variable %s is not c referenced as C symbol", symbol->name.c_str());
		}

		if (k_str_zs_int_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_ZS_INT_PTR
			};

		}
		else if (k_str_const_zs_int_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_ZS_INT_PTR | STK_PROPERTY_READ_ONLY
			};

		}
		else if (k_str_zs_float_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_ZS_FLOAT_PTR
			};
		}
		else if (k_str_const_zs_float_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_ZS_FLOAT_PTR | STK_PROPERTY_READ_ONLY
			};
		}
		else if (k_str_char_type_ptr == symbol->str_native_type
			|| k_str_const_char_type_ptr == symbol->str_native_type
			|| k_str_zs_string_type_ptr == symbol->str_native_type
			) {
			char *input_s = (char *)ptr_variable;
			ScriptObjectString *s = ZS_NEW_OBJECT_STRING(zs);

			if (k_str_zs_string_type_ptr == symbol->str_native_type) {
				s->value = (void *)ptr_variable;
				input_s = (char *)(((zs_string *)ptr_variable)->c_str());
			}

			*((zs_string *)(s->value)) = input_s;
			return {
				(zs_int)(s),
				STK_PROPERTY_SCRIPT_OBJECT
			};
		}
		else if (k_str_bool_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_BOOL_PTR
			};
		}
		else if (k_str_const_bool_type_ptr == symbol->str_native_type) {
			return {
					(zs_int)ptr_variable,
					STK_PROPERTY_BOOL_PTR | STK_PROPERTY_READ_ONLY
			};
		}
		else {
			ScriptType *info_registered_class = zs->getScriptTypeFactory()->getScriptTypeFromTypeNamePtr(symbol->str_native_type);//  ScriptType::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

			if (info_registered_class == NULL) {
				THROW_RUNTIME_ERROR("Native symbol '%s' has type '%s' that is not registered", symbol->name.c_str(), symbol->str_native_type.c_str());
			}

			ScriptObjectClass *var = ScriptObjectClass::newScriptObjectClass(zs, info_registered_class->idx_type, ptr_variable);

			return{
					(zs_int)var,
					STK_PROPERTY_SCRIPT_OBJECT
			};

		}
		
		return k_stk_null;
	}

	

}
