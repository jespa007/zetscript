#pragma once

#define SCOPE_FACTORY					zetscript::CScopeFactory::getInstance()

#define IDX_GLOBAL_SCOPE				0
#define IDX_INVALID_SCOPE				ZS_UNDEFINED_IDX
#define IDX_ANONYMOUSE_SCOPE			-3



namespace zetscript{

	class CScopeFactory{
		public:
			CScopeFactory(CZetScript 					*zs);

			CScope	    		*	newScope(short idx_parent_scope=ZS_UNDEFINED_IDX,bool is_c_node=false);
			CScope 				* 	getScope(short idx);
			std::vector<CScope *> 	* 	getVectorScopeNode();
			void clear();

			~CScopeFactory();

		private:


			CZetScript 					*zs;
			/**
			 * Vector of script scopes. This std::vector is removed when zetscript reevaluates all scrips.
			 */
			std::vector<CScope *> 				 vec_scope_node;

	};

};
