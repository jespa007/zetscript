/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL		0x01
#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN				0x02
#define	REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP				0x04
#define	REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS				0x08 // special case for c functions that can have same parameters but diferent signatures
#define	REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS					0x10 // special case for c functions that can have same parameters but diferent signatures
//#define	SCOPE_DIRECTION_AVOID_MAIN_SCOPE		0x8
#define REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN		(REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN | REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP)
#define MAX_INNER_SCOPES_FUNCTION								10


#define SCOPE_PROPERTY_IS_C_OBJECT_REF							0x01
#define SCOPE_PROPERTY_IS_SCOPE_FUNCTION						0x02
#define SCOPE_PROPERTY_IS_SCOPE_CLASS							0x04
#define SCOPE_PROPERTY_IS_SCOPE_BLOCK							0x08
#define SCOPE_PROPERTY_UNUSUED									0x10


namespace zetscript{

	class ScopeFactory;
	class ZetScript;
	class ScriptType;
	class  Scope{
	public:
		uint16_t properties;
		//bool is_c_node;
		//bool is_scope_function; // this will ignore symbols within scopes where functions starts to avoid conflicts with global functions...
		ScriptType *script_type;
		int tmp_idx_instruction_push_scope;


		Scope 					*scope_parent, *scope_base;

		zs_vector				*scopes;  // local scopes from starting block { }
		zs_vector				*symbol_variables; // variable symbols from starting block { }
		zs_vector				*symbol_functions; // function symbols from starting block { }
		zs_vector				*symbol_types; // function symbols from starting block { }

		//--------------------------------------------------------------------
		// Functions

		Scope(ZetScript * _zs, int _idx_script_function, Scope * _scope_parent=NULL,uint16_t _properties=0);

		//--------------------------------------------------------------------
		// Register functions

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerSymbolVariable(const char * file, short line,const zs_string & symbol_name, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * registerSymbolFunction(const char * file, short line,const zs_string & symbol_name, char n_params,uint16_t  _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * registerSymbolType(const char * file, short line,const zs_string & symbol_name, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN);
		Symbol * getSymbol(
				const zs_string & var_name
				, char n_params=NO_PARAMS_SYMBOL_ONLY
				, uint16_t _check_repeteaded_symbols=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN
		);

		bool 	unregisterSymbol(Symbol *symbol);

		void						    markAsUnusued();

		void removeChildrenBlockTypes();
		void setScriptClass(ScriptType *sc);
		ScriptType * getScriptType();
		int getIdxScriptFunction();
		int numInnerScopes();

		int countVariables(bool _recursive=false);


		~Scope();

	private:

		ZetScript *zs;
		ScopeFactory *scope_factory;
		int 			idx_script_function;

		void 	 checkPreRegisterSymbol(const char * file,short line, const zs_string & symbol_name, char n_params, uint16_t check_repeated_symbols_direction);

		Symbol * getSymbolRecursive(const zs_string & var_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveDownScope(const zs_string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);
		Symbol * getSymbolRecursiveUpScope(const zs_string & ref_symbol, char n_params=NO_PARAMS_SYMBOL_ONLY);

		void markBlockScopeAsUnusuedScopesRecursive(Scope *_sc);

		int numInnerScopesRecursive(Scope *sc, int n);
		int countVariablesRecursive(Scope *sc, int idx_script_function_reference);


	};
}
