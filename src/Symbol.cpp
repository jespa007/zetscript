/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	// STATIC

	StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol, void *ptr_variable) {

		if ((symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0) {
			THROW_RUNTIME_ERROR(
				"Variable %s is not c referenced as C symbol"
				, symbol->name.c_str()
			);
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
			StringScriptObject *s = ZS_NEW_STRING_OBJECT(zs);

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
		
		// it should be script object
		ScriptType *info_registered_class = zs->getScriptTypeFactory()->getScriptTypeFromTypeNamePtr(symbol->str_native_type);//  ScriptType::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

		if (info_registered_class == NULL) {
			THROW_RUNTIME_ERROR(
				"Native symbol '%s' has type '%s' that is not registered"
				, symbol->name.c_str()
				, symbol->str_native_type.c_str()
			);
		}

		ClassScriptObject *var = ClassScriptObject::newScriptObjectClass(zs, info_registered_class->idx_script_type, ptr_variable);

		return{
				(zs_int)var,
				STK_PROPERTY_SCRIPT_OBJECT
		};
	}

	// PRIVATE

	void Symbol::copy(const Symbol & _symbol){
		name=_symbol.name;
		str_native_type=_symbol.str_native_type;


		file=_symbol.file;
		line=_symbol.line;
		idx_position = _symbol.idx_position; // in principle is not on stack

		scope = _symbol.scope;
		n_params = _symbol.n_params;
		properties = _symbol.properties;
		ref_ptr = _symbol.ref_ptr;
		overrided_symbol=_symbol.overrided_symbol;
	}

	// PUBLIC
	Symbol::Symbol(const zs_string & _name){
		name = _name;
		str_native_type = "";
		file="";
		line=-1;
		idx_position = ZS_IDX_UNDEFINED; // in principle is not on stack

		scope = NULL;
		n_params = NO_PARAMS_SYMBOL_ONLY;
		properties = 0;
		ref_ptr = 0;
		overrided_symbol=NULL;
	}

	Symbol::Symbol(const Symbol & _symbol){
		copy(_symbol);
	}

	Symbol Symbol::operator = (const Symbol & _symbol){
		copy(_symbol);
		return *this;
	}

	Symbol::~Symbol() {

	}


}
