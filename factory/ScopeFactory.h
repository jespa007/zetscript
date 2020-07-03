#pragma once

#define SCOPE_FACTORY					ZetScript::ScopeFactory::getInstance()

#define IDX_GLOBAL_SCOPE				0
#define IDX_INVALID_SCOPE				ZS_UNDEFINED_IDX
#define IDX_ANONYMOUSE_SCOPE			-3



namespace ZetScript{

	class ScopeFactory{
		public:
			ScopeFactory(ZetScript 					*zs);

			Scope	    		*	newScope(short idx_parent_scope=ZS_UNDEFINED_IDX,bool is_c_node=false);
			Scope 				* 	getScope(short idx);
			std::vector<Scope *> 	* 	getVectorScopeNode();
			void clear();

			~ScopeFactory();

		private:


			ZetScript 					*zs;
			/**
			 * Vector of script scopes. This std::vector is removed when ZetScript reevaluates all scrips.
			 */
			std::vector<Scope *> 				 vec_scope_node;

	};

};
