#pragma once

#define SCOPE_FACTORY					zetscript::CScopeFactory::getInstance()

#define IDX_GLOBAL_SCOPE				0
#define IDX_INVALID_SCOPE				ZS_UNDEFINED_IDX
#define IDX_ANONYMOUSE_SCOPE			-3


#define GET_SCOPE(idx) 				zetscript::CScopeFactory::getInstance()->getScope(idx)
#define MAIN_SCOPE						zetscript::CScopeFactory::getInstance()->getScope(IDX_GLOBAL_SCOPE)

#define NEW_SCOPE						(zetscript::CScopeFactory::getInstance()->newScope)

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)		((zetscript::CScopeFactory::getInstance()->getScope(idx))->getIdxBaseScope()==IDX_GLOBAL_SCOPE)

namespace zetscript{

	class CScopeFactory{


		public:

			static CScopeFactory * getInstance();
			static void destroySingleton();


			CScope	    		*	newScope(short idx_parent_scope=ZS_UNDEFINED_IDX,bool is_c_node=false);
			CScope 				* 	getScope(short idx);
			std::vector<CScope *> 	* 	getVectorScopeNode();
			void clear();

		private:

			static CScopeFactory 		*	scope_factory_singleton;


			/**
			 * Vector of script scopes. This std::vector is removed when zetscript reevaluates all scrips.
			 */
			std::vector<CScope *> 				 vec_scope_node;

			CScopeFactory();
			~CScopeFactory();
	};

};
