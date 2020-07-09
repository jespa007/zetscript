/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{

	#pragma pack(push, 1)

	struct SymbolInfo { // it can be a variable or function
		intptr_t ref_ptr; // pointer ref just in case is C var/function
		Symbol *symbol; // symbol name
		//short idx_class; //ScriptClass		 *class_info;
		//short idx_scope;

		short idx_symbol; // idx of class function/variable symbol that keeps.

		unsigned short symbol_info_properties; // SymbolInfoProperty
		std::string c_type; // In case is C, we need to know its type ...

		SymbolInfo() {
			symbol_info_properties = 0;
			c_type = "";
			//idx_scope = -1;
			ref_ptr = 0;
			symbol=NULL;
			//class_info=NULL;

			//idx_class = -1;
			//idxScopeVar=-1;
			idx_symbol = -1;
		}
	};

	#pragma pack(pop)

}
