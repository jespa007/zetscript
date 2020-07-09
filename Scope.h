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
	class ScopeFactory;
	class  Scope{

	public:

		short idx_scope;
		bool is_c_node;
		ScriptClass *script_class;

		//--------------------------------------------------------------------
		// Register functions
		std::vector<Symbol *> m_scopeSymbol; // vars registered from base.

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerSymbol(const std::string & file, short line,const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);


		Scope(ZetScript * _zs,short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX,bool is_c_node=false);

		short 	 	getIdxBaseScope();
		short	 	getIdxParent();
		Scope * 	getCurrentScopePointer();
		short		getIdxCurrentScopePointer();


		void setScriptClass(ScriptClass *sc);
		ScriptClass * getScriptClass();

		std::vector<Symbol *> * getRegisteredVariableList();
		std::vector<short> 	  * getLocalScopeList();

		Scope * pushScope();
		Scope * popScope();
		void resetScopePointer();

		~Scope();

	private:
		std::vector<short> m_localScopeList;
		ZetScript *zs;
		ScopeFactory *scope_factory;

		// The a parent scope ...
		short idxParentScope, idxCurrentScopePointer, idxBaseScope;

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);


	};
}
