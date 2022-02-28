/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IDX_SCRIPT_SCOPE_MAIN										0
#define MAIN_SCOPE(data)											(((data)->scope_factory))->getMainScope()
#define NEW_SCOPE(data,idx_function,scope_parent,properties)		(((data)->scope_factory))->newScope(idx_function,scope_parent,properties)
#define CLEAR_UNUSUED_SCOPES(data)									(((data)->scope_factory))->clearUnusuedScopes()


namespace zetscript{

	class ScriptFunctionFactory;
	class ScopeFactory{
		public:
			ScopeFactory(ZetScript 		*zs);
			void init();
			Scope	    				*	newScope(int idx_sf,Scope * scope_parent=NULL,uint16_t _properties=0);
			Scope 						* 	getMainScope(){return main_scope;}
			zs_vector 					* 	getScopes();
			void clear();
			void clearUnusuedScopes();
			void saveState();

			~ScopeFactory();

		private:

			ZetScript 					*zs;

			/**
			 * Vector of script scopes.
			 */
			zs_vector 		 *scopes;
			Scope *main_scope;
			int idx_clear_checkpoint;
			int idx_clear_global_checkpoint_global_symbol_registered_variables;
			int idx_clear_global_checkpoint_global_symbol_registered_functions;
			int idx_clear_global_checkpoint_global_symbol_registered_types;

			void clearGlobalSymbols(int _idx_start=ZS_IDX_UNDEFINED);

	};

}
