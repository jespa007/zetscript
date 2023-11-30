/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_IDX_SCRIPT_SCOPE_MAIN									0
#define ZS_MAIN_SCOPE(data)											(((data)->scope_factory))->getMainScope()
#define ZS_NEW_SCOPE(data,idx_function,scope_parent,properties)		(((data)->scope_factory))->newScope(idx_function,scope_parent,properties)
#define ZS_CLEAR_UNUSUED_SCOPES(data)								(((data)->scope_factory))->clearUnusuedScopes()


namespace zetscript{

	class ScriptFunctionsFactory;
	class ScriptScopesFactory{
		public:
			ScriptScopesFactory(ZetScript 		*zs);
			void init();
			ScriptScope	    				*	newScope(int idx_sf,ScriptScope * scope_parent=NULL,uint16_t _properties=0);
			ScriptScope 						* 	getMainScope(){return main_scope;}
			Vector<ScriptScope *> 			* 	getScopes();
			void clear();
			void clearUnusuedScopes();
			void saveState();

			~ScriptScopesFactory();

		private:

			ZetScript 					*zs;

			/**
			 * Array of script scopes.
			 */
			Vector<ScriptScope *> 		 *scopes;
			ScriptScope *main_scope;
			int		idx_clear_checkpoint;
			//int idx_clear_global_checkpoint_global_symbol_registered_variables;
			//int idx_clear_global_checkpoint_global_symbol_registered_functions;
			//int idx_clear_global_checkpoint_global_symbol_registered_types;

			void clearGlobalSymbols(int _idx_start=ZS_UNDEFINED_IDX);

	};

}
