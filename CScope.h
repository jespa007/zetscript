/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

/*
//   _____       __        _____                        _   _
//	|_   _|     / _|      /  ___|                      | | | |
// 	  | | _ __ | |_ ___   \ `--.  ___ ___  _ __   ___  | | | | __ _ _ __
// 	  | || '_ \|  _/ _ \   `--. \/ __/ _ \| '_ \ / _ \ | | | |/ _` | '__|
//   _| || | | | || (_) | /\__/ / (_| (_) | |_) |  __/ \ \_/ / (_| | |
//   \___/_| |_|_| \___/  \____/ \___\___/| .__/ \___|  \___/ \__,_|_|
//                                        | |
//                                        |_|
// 	_________________________________________________
// 	 	__________________________________
*/

namespace zetscript{

	class  CScope{

	public:

		short idxScope;
		bool is_c_node;
		CScriptClass *script_class;

		//--------------------------------------------------------------------
		// Register functions
		std::vector<Symbol *> m_scopeSymbol; // vars registered from base.

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerSymbol(const std::string & file, short line,const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);


		CScope(short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX,bool is_c_node=false);

		short 	 	getIdxBaseScope();
		short	 	getIdxParent();
		CScope * 	getCurrentScopePointer();
		short		getIdxCurrentScopePointer();


		void setScriptClass(CScriptClass *sc);
		CScriptClass * getScriptClass();

		std::vector<Symbol *> * getRegisteredVariableList();
		std::vector<short> 	  * getLocalScopeList();

		CScope * pushScope();
		CScope * popScope();
		void resetScopePointer();

		~CScope();

	private:
		std::vector<short> m_localScopeList;

		// The a parent scope ...
		short idxParentScope, idxCurrentScopePointer, idxBaseScope;

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);


	};
}
