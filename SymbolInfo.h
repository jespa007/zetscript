/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{

	#pragma pack(push, 1)

	class	ZetScript;
	struct	SymbolInfo { // it can be a variable or function
		intptr_t ref_ptr; // pointer ref just in case is C var/function
		Symbol *symbol; // symbol name
		short idx_symbol; // idx local variable/function or idx member variable/function.

		unsigned short symbol_info_properties; // SymbolInfoProperty
		std::string c_type; // In case is C, we need to know its type ...

		SymbolInfo() {
			symbol_info_properties = 0;
			c_type = "";
			ref_ptr = 0;
			symbol=NULL;
			idx_symbol = -1;
		}
	};

	StackElement convertSymbolInfoToStackElement(ZetScript * zs, SymbolInfo *symbol,void *ptr_variable);

	#pragma pack(pop)

}
