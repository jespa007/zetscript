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
		vector<tSymbol *> m_scopeSymbol; // vars registered from base.

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		tSymbol * registerSymbol(const string & file, short line,const string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		tSymbol * getSymbol(const string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);


		CScope(short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX,bool is_c_node=false);

		short 	 	getIdxBaseScope();
		short	 	getIdxParent();
		CScope * 	getCurrentScopePointer();
		short		getIdxCurrentScopePointer();


		void setScriptClass(CScriptClass *sc);
		CScriptClass * getScriptClass();

		vector<tSymbol *> * getRegisteredVariableList();
		vector<short> 	  * getLocalScopeList();

		CScope * pushScope();
		CScope * popScope();
		void resetScopePointer();

		~CScope();

	private:
		vector<short> m_localScopeList;

		// The a parent scope ...
		short idxParentScope, idxCurrentScopePointer, idxBaseScope;

		tSymbol * getSymbolRecursive(const string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		tSymbol * getSymbolRecursiveDownScope(const string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);
		tSymbol * getSymbolRecursiveUpScope(const string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);


	};
}
