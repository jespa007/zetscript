/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IDX_GLOBAL_SCOPE				0
#define IDX_INVALID_SCOPE				ZS_IDX_UNDEFINED
#define IDX_ANONYMOUSE_SCOPE			-3

#define GET_SCOPE(data,idx)				((data->scope_factory))->getScope(idx)
#define MAIN_SCOPE(data)				((data->scope_factory))->getScope(IDX_GLOBAL_SCOPE)
#define NEW_SCOPE(data,idx)				((data->scope_factory))->newScope(idx)


namespace zetscript{

	class ScopeFactory{
		public:
			ScopeFactory(ZetScript 		*zs);

			Scope	    				*	newScope(short idx_scope_parent=ZS_IDX_UNDEFINED,bool is_c_node=false);
			Scope 						* 	getScope(short idx);
			std::vector<Scope *> 		* 	getScopes();
			void clear();

			~ScopeFactory();

		private:


			ZetScript 					*zs;
			/**
			 * Vector of script scopes. This std::vector is removed when ZetScript reevaluates all scrips.
			 */
			std::vector<Scope *> 		 scopes;

	};

};
