/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define NO_PARAMS_SYMBOL_ONLY			-1
#define ANY_PARAMS_SYMBOL_ONLY			-2
#define SYMBOL_VALUE_THIS				"this"
#define SYMBOL_VALUE_SUPER				"super"

//typedef tInfoStatementOp *PInfoStatementOp;
typedef enum {
	SYMBOL_PROPERTY_ATTRIB						= 0x1 << 1, // 0x0001: ref_ptr holds structure get/set functions
	SYMBOL_PROPERTY_FUNCTION					= 0x1 << 2, // 0x0002: ref_ptr holds script function ptr
	SYMBOL_PROPERTY_C_OBJECT_REF 				= 0x1 << 3, // 0x0004: ref_ptr holds C object
	SYMBOL_PROPERTY_STATIC 						= 0x1 << 4, // 0x0008: C function or C++ static functions
	SYMBOL_PROPERTY_CONST 						= 0x1 << 5, // 0x0010: This symbol is not assignable
	SYMBOL_PROPERTY_MEMBER_FUNCTION				= 0x1 << 6, // 0x0020: Member function
	SYMBOL_PROPERTY_MEMBER_ATTRIBUTE			= 0x1 << 7, // 0x0040: Member attrib
	SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME			= 0x1 << 8, // 0x0080: Deduce runtime functions becuse is C and it has functions with same name but different signature
	SYMBOL_PROPERTY_ARG_BY_REF							= 0x1 << 9,  // 0x0100: Is a reference
}SymbolProperty;

#define IS_SYMBOL_VAR_MEMBER(s) ((s)->properties == 0)

namespace zetscript{

	class	ZetScript;
	struct Symbol {
		std::string file;   // file where symbol was registered
		short  line;		// line where symbol was registered
		short  idx_position;  // used as position where local var/function is located VM (VM reserves space required for all vars)

		Scope *scope;		// scope where symbol was registered
		unsigned short properties; // symbol properties using SymbolProperty bits

		std::string name; 	// symbol name
		zs_int ref_ptr; 	// pointer ref that holds rellevant information
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
