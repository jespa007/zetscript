/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL		0x1
#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN				0x2
#define	REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP				0x4
#define	REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS				0x8 // special case for c functions that can have same parameters but diferent signatures
//#define	SCOPE_DIRECTION_AVOID_MAIN_SCOPE		0x8
#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN		(REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN | REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP)
#define MAX_INNER_SCOPES_FUNCTION					10


namespace zetscript{

	class ScopeFactory;
	class ZetScript;
	class ScriptClass;
	class  Scope{
	public:

		bool is_c_node;
		bool is_scope_function; // this will ignore symbols within scopes where functions starts to avoid conflicts with global functions...
		ScriptClass *script_class;
		int tmp_idx_instruction_push_scope;

		bool unusued;
		Scope 		*scope_parent, *scope_base;

		zs_vector				*registered_scopes;  // local scopes from starting block { }
		zs_vector				*symbol_variables; // variable symbols from starting block { }
		zs_vector				*symbol_functions; // function symbols from starting block { }
		zs_vector				*symbol_classes; // function symbols from starting block { }

		//--------------------------------------------------------------------
		// Functions

		Scope(ZetScript * _zs, int idx_sf, Scope * _scope_parent=NULL,bool is_c_node=false);

		//--------------------------------------------------------------------
		// Register functions

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		//Symbol * addSymbol(const char * file, short line,const std::string & symbol_name, char n_params);

		Symbol * registerSymbolVariable(const char * file, short line,const std::string & symbol_name, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * registerSymbolFunction(const char * file, short line,const std::string & symbol_name, char n_params, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * registerSymbolClass(const char * file, short line,const std::string & symbol_name, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);

		bool 	unregisterSymbol(Symbol *symbol);

		void						    markAsUnusued();


		void setScriptClass(ScriptClass *sc);
		ScriptClass * getScriptClass();
		int getIdxScriptFunction();
		int maxInnerScopes();


		~Scope();

	private:

		ZetScript *zs;
		ScopeFactory *scope_factory;
		int 			idx_script_function;

		void 	 checkPreRegisterSymbol(const char * file,short line, const std::string & symbol_name, char n_params, uint16_t check_repeated_symbols_direction);

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);

		int maxInnerScopesRecursive(Scope *sc, int n);


	};
}
