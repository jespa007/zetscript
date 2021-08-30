/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IDX_SCRIPT_SCOPE_MAIN				0
#define MAIN_SCOPE(data)					(((data)->scope_factory))->getMainScope()
#define NEW_SCOPE(data,sf,scope_parent)		(((data)->scope_factory))->newScope(sf,scope_parent)


namespace zetscript{

	class ScriptFunctionFactory;
	class ScopeFactory{
		public:
			ScopeFactory(ZetScript 		*zs);

			Scope	    				*	newScope(int idx_sf,Scope * scope_parent=NULL,bool is_c_node=false);
			Scope 						* 	getMainScope(){return main_scope;}
			std::vector<Scope *>		* 	getScopes();
			void clear();
			void saveState();

			~ScopeFactory();

		private:

			ZetScript 					*zs;

			/**
			 * Vector of script scopes. This std::vector is removed when ZetScript reevaluates all scrips.
			 */
			std::vector<Scope *> 		 *scopes;
			Scope *main_scope;
			int idx_clear_checkpoint;
			int idx_clear_global_checkpoint_global_symbol_registered_variables;
			int idx_clear_global_checkpoint_global_symbol_registered_functions;
			int idx_clear_global_checkpoint_global_scopes;

			void clearGlobalSymbols(int _idx_start=ZS_IDX_UNDEFINED);

	};

};
