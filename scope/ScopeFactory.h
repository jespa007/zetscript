/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define MAIN_SCOPE(data)				((data->scope_factory))->getMainScope()
#define NEW_SCOPE(data,scope_child)	((data->scope_factory))->newScope(scope_child)


namespace zetscript{

	class ScopeFactory{
		public:
			ScopeFactory(ZetScript 		*zs);

			Scope	    				*	newScope(Scope * scope_child=NULL,bool is_c_node=false);


			Scope 						* 	getMainScope(){return main_scope;}
			zs_vector 					* 	getScopes();
			void clear();

			~ScopeFactory();

		private:
			ZetScript 					*zs;

			/**
			 * Vector of script scopes. This std::vector is removed when ZetScript reevaluates all scrips.
			 */
			zs_vector 		 *scopes;
			Scope *main_scope;

	};

};
