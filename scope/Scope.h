/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class ScopeFactory;
	class ZetScript;
	class  Scope{

	public:

		short idx_scope;
		bool is_c_node;
		ScriptClass *script_class;

		//--------------------------------------------------------------------
		// Register functions
		std::vector<Symbol *> scope_symbols; // vars registered from base.

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerSymbol(const std::string & file, short line,const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);


		Scope(ZetScript * _zs,short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX,bool is_c_node=false);

		short 	 	getIdxScopeBase();
		short	 	getIdxScopeParent();
		Scope * 	getScopePtrCurrent();
		short		getIdxScopePtrCurrent();


		void setScriptClass(ScriptClass *sc);
		ScriptClass * getScriptClass();

		std::vector<Symbol *> * getRegisteredVariableList();
		std::vector<short> 	  * getLocalScopeList();

		Scope * pushScope();
		Scope * popScope();
		void resetScopePointer();

		~Scope();

	private:
		std::vector<short> idx_local_scopes;
		ZetScript *zs;
		ScopeFactory *scope_factory;

		// The a parent scope ...
		short idx_scope_parent, idx_scope_ptr_current, idx_scope_base;

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_IS_VARIABLE);


	};
}
