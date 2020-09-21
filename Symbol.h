/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define NO_PARAMS_SYMBOL_ONLY			-1
#define SYMBOL_VALUE_THIS				"this"
#define SYMBOL_VALUE_SUPER				"super"

//typedef tInfoStatementOp *PInfoStatementOp;
typedef enum {
	SYMBOL_PROPERTY_IS_FUNCTION						= 0x1 << 0, // ref_ptr holds script function ptr
	SYMBOL_PROPERTY_C_OBJECT_REF 					= 0x1 << 1,
	SYMBOL_PROPERTY_C_STATIC_REF 					= 0x1 << 2, // C function or C++ static functions
	SYMBOL_PROPERTY_CONST 							= 0x1 << 3, // This symbol is not assignable
	SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS		= 0x1 << 4  // will pass object this as first parameter
}SymbolProperty;

namespace zetscript{

	class	ZetScript;
	struct Symbol {
		std::string file;   // file where symbol was registered
		short  line;		// line where symbol was registered
		short  idx_position;  // used as position where local var/function is located VM (VM reserves space required for all vars)

		Scope *scope;		// scope where symbol was registered
		unsigned short properties; // symbol properties using SymbolProperty bits

		std::string name; 	// symbol name
		intptr_t ref_ptr; 	// pointer ref just in case is C var/function
		std::string str_native_type; // In case is C, we need to know its type ...

		char n_params; // max params in case of function

		Symbol(){
			file="";
			line=-1;
			idx_position = ZS_IDX_UNDEFINED; // in principle is not on stack

			scope = NULL;
			name="";
			n_params = NO_PARAMS_SYMBOL_ONLY;
			properties = 0;
			str_native_type = "";
			ref_ptr = 0;
		}

	};

	StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol,void *ptr_variable);

}
