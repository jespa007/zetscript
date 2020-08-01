/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define NO_PARAMS_IS_VARIABLE			-1
//#define NO_PARAMS_SYMBOL_ONLY			-2
#define NO_PARAMS_IS_CLASS				-2

namespace zetscript{

	class	ZetScript;
	struct Symbol {
		std::string file;   // file where symbol was registered
		short  line;		// line where symbol was registered
		short  idx_position;  // used as position where local var/function is located VM (VM reserves space required for all vars)

		Scope *scope;		// scope where symbol was registered
		unsigned short symbol_properties; // symbol properties using SymbolProperty bits

		std::string name; 	// symbol name
		intptr_t ref_ptr; 	// pointer ref just in case is C var/function
		std::string c_type; // In case is C, we need to know its type ...

		char n_params; // max params in case of function

		Symbol(){
			file="";
			line=-1;
			idx_position = ZS_IDX_UNDEFINED; // in principle is not on stack

			scope = NULL;
			name="";
			n_params = NO_PARAMS_IS_VARIABLE;
			symbol_properties = 0;
			c_type = "";
			ref_ptr = 0;
		}

	};

	StackElement convertSymbolToStackElement(ZetScript * zs, Symbol *symbol,void *ptr_variable);

}
