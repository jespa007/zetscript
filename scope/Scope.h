/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


#define SCOPE_DIRECTION_CURRENT					0x1
#define SCOPE_DIRECTION_DOWN					0x2
#define	SCOPE_DIRECTION_UP						0x4
//#define	SCOPE_DIRECTION_AVOID_MAIN_SCOPE		0x8
#define SCOPE_DIRECTION_BOTH					(SCOPE_DIRECTION_DOWN | SCOPE_DIRECTION_UP)


	class ScopeFactory;
	class ZetScript;
	class ScriptClass;
	class  Scope{
	public:

		bool is_c_node;
		bool is_scope_function; // this will ignore symbols within scopes where functions starts to avoid conflicts with global functions...
		ScriptClass *script_class;
		int tmp_idx_instruction_push_scope;
		unsigned int n_registered_symbols_as_variables;
		// This flag is used to remove usign eraseUnusedScopes...
		bool unusued;
		//void		*eval_scope_tmp; // aux variable used on evaluation
		Scope 		*scope_parent, *scope_base;

		zs_vector				*registered_scopes;  // local scopes from starting block { }
		zs_vector				*registered_symbols; // local symbols from starting block { }
		//std::vector<Scope *> local_scopes;
		//std::vector<Symbol *> symbols; // vars registered from base.


		//--------------------------------------------------------------------
		// Functions

		Scope(ZetScript * _zs, int idx_sf, Scope * _scope_parent=NULL,bool is_c_node=false);

		//--------------------------------------------------------------------
		// Register functions

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * addSymbol(const char * file, short line,const std::string & symbol_name, char n_params);
		Symbol * registerSymbol(const char * file, short line,const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, uint16_t scope_direction_repeated_symbols=SCOPE_DIRECTION_BOTH);
		Symbol * getSymbol(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY, uint16_t scope_direction_repeated_symbols=SCOPE_DIRECTION_BOTH);

		void						    markAsUnusued();


		void setScriptClass(ScriptClass *sc);
		ScriptClass * getScriptClass();
		int getIdxScriptFunction();

		unsigned int			numRegisteredSymbolsAsVariable();
		~Scope();

	private:

		ZetScript *zs;
		ScopeFactory *scope_factory;
		int 			idx_script_function;

		Symbol * getSymbolRecursive(const std::string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveDownScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveUpScope(const std::string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);


	};
}
