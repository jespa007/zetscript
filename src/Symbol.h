/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NO_PARAMS_SYMBOL_ONLY			-1


//typedef tInfoStatementOp *PInfoStatementOp;
typedef enum:unsigned short  {
	SYMBOL_PROPERTY_C_OBJECT_REF 				= 0x0001,// ref_ptr holds C object
	SYMBOL_PROPERTY_STATIC 						= 0x0002,// C function or C++ static functions
	SYMBOL_PROPERTY_TYPE						= 0x0004,// symbol is type
	SYMBOL_PROPERTY_FUNCTION					= 0x0008,// ref_ptr holds script function ptr
	SYMBOL_PROPERTY_CONST 						= 0x0010,// This symbol is not assignable
	SYMBOL_PROPERTY_MEMBER_PROPERTY				= 0x0020,// Member attrib
	SYMBOL_PROPERTY_ARG_BY_REF					= 0x0040,// Is a reference
	//SYMBOL_PROPERTY_ALLOCATED_STK				= 0x0080,// Is a reference
}SymbolProperty;

namespace zetscript{

	class	ZetScript;
	class 	Scope;
	struct Symbol {
		const char *file;   // file where symbol was registered
		short  line;		// line where symbol was registered
		int   idx_position;  // used as position where local var/function is located VM (VM reserves space required for all vars)

		Scope *scope;		// scope where symbol was registered
		unsigned short properties; // symbol properties using SymbolProperty bits

		zs_string name; 	// symbol name
		zs_int ref_ptr; 	// pointer ref that holds rellevant information
		zs_string str_native_type; // In case is C, we need to know its type ...

		int8_t n_params; // max params in case of function

		Symbol *overrided_symbol; // for virtual functions

		Symbol(const zs_string & _name);

		Symbol(const Symbol & _symbol);

		Symbol operator = (const Symbol & _symbol);

		~Symbol();

	protected:

		void copy(const Symbol & _symbol);

	};

	StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol,void *ptr_variable);

}
