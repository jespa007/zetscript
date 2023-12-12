/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


//#define	SCOPE_DIRECTION_AVOID_MAIN_SCOPE		0x8

#define ZS_SCOPE_MAX_INNER_SCOPES_FUNCTION								10
#define SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN		(SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN | SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP)

namespace zetscript{

	typedef enum{
		SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL=0x01
		,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN		=0x02
		,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP			=0x04
		,SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_REPEATED_SYMBOLS			=0x08 // special case for c functions that can have same parameters but diferent signatures
		,SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_CLASS_SYMBOLS				=0x10 // special case for c functions that can have same parameters but diferent signatures
	}ScriptScopeRegisterProperty;

	typedef enum{
		SCOPE_PROPERTY_IS_C_OBJECT_REF			=0x01
		,SCOPE_PROPERTY_IS_SCOPE_FUNCTION		=0x02
		,SCOPE_PROPERTY_IS_SCOPE_CLASS			=0x04
		,SCOPE_PROPERTY_IS_SCOPE_BLOCK			=0x08
		,SCOPE_PROPERTY_UNUSUED					=0x10
	}ScriptScopeProperty;


	class ScriptScopesFactory;
	class ScriptEngine;
	class ScriptType;
	class  ScriptScope{
	public:
		uint16_t properties;
		//bool is_c_node;
		//bool is_scope_function; // this will ignore symbols within scopes where functions starts to avoid conflicts with global functions...
		ScriptType *owner_type;
		int offset_instruction_push_scope;


		ScriptScope 					*scope_parent, *scope_base;

		Vector<Symbol *>		*symbol_variables; // variable symbols from starting block { }
		Vector<Symbol *>		*symbol_functions; // function symbols from starting block { }
		Vector<Symbol *>		*symbol_types; // function symbols from starting block { }

		//--------------------------------------------------------------------
		// Functions

		ScriptScope(
			ScriptEngine * _se
			, int _idx_script_function
			, ScriptScope * _scope_parent=NULL
			,uint16_t _properties=0
		);

		//--------------------------------------------------------------------
		// Register functions

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		Symbol * registerSymbolVariable(
				const char * file
				, short line
				,const String & symbol_name
				, uint16_t _check_repeteaded_symbols=SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN
		);

		Symbol * registerSymbolFunction(
				const char * file
				, short line
				,const String & symbol_name
				, int8_t n_params
				,uint16_t  _check_repeteaded_symbols=SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN
		);

		Symbol * registerSymbolType(
				const char * file
				,short line
				,const String & symbol_name
				, uint16_t _check_repeteaded_symbols=SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN
		);

		Symbol * getSymbol(
				const String & var_name
				, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY
				, uint16_t _check_repeteaded_symbols=SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN
		);

		bool 	unregisterSymbol(Symbol *symbol);

		void						    markAsUnusued();
		Vector<ScriptScope *>	*getScopes();

		void removeChildrenBlockTypes();
		void setTypeOwner(ScriptType *_owner_type);
		ScriptType * getOwnerType();
		int getIdxFunction();
		int numInnerScopes();

		int countVariables(bool _recursive=false);


		~ScriptScope();

	private:

		ScriptEngine *zs;

		Vector<ScriptScope *>		*scopes;  // local scopes from starting block { }
		ScriptScopesFactory 			*scope_factory;
		int 					id;

		void 	 checkPreRegisterSymbol(
				const char * file
				,short line
				, const String & symbol_name
				, int8_t n_params
				, uint16_t check_repeated_symbols_direction
		);

		void markBlockScopeAsUnusuedScopesRecursive(ScriptScope *_sc);

		int numInnerScopesRecursive(ScriptScope *sc, int n);
		int countVariablesRecursive(ScriptScope *sc, int idx_script_function_reference);


	};
}
