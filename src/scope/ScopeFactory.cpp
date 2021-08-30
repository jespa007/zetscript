#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new std::vector<Scope *>;
		main_scope=newScope(IDX_SCRIPT_FUNCTION_MAIN,NULL,false); // create global scope (scope 0)
		idx_clear_checkpoint=1;  // start from MAIN scope
		idx_clear_global_checkpoint_global_symbol_registered_variables=0;
		idx_clear_global_checkpoint_global_symbol_registered_functions=0;
		idx_clear_global_checkpoint_global_scopes=0;

	}




	Scope *	 ScopeFactory::newScope(int idx_sf,Scope * scope_parent,bool is_c_node){
		Scope * scope_node = new Scope(this->zs,idx_sf,scope_parent,is_c_node);
		scopes->push_back(scope_node);
		return scope_node;
	}

	std::vector<Scope *> 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clear(){
		int idx_start = idx_clear_checkpoint;

		int idx_start_global_scopes=idx_clear_global_checkpoint_global_scopes;


		std::string global_symbol="";

		for(
			int v=scopes->size()-1;
			v > idx_start; // avoid main scope
			v--
		){
			Scope * info_scope=scopes->at(v);
			delete info_scope;
			scopes->pop_back();
		}

		// erase all local scopes in the main scope
		for (
				int v = main_scope->registered_scopes->size()-1;
				v > idx_start_global_scopes ;
				v--) {
			// Only pop-back, don't delete because they were deleted on previous loop
			main_scope->registered_scopes->pop_back();

		}
	}

	void ScopeFactory::saveState(){
		Scope *main_scope= (Scope *)this->scopes->at(IDX_SCRIPT_SCOPE_MAIN);
		idx_clear_checkpoint=scopes->size()-1;


		idx_clear_global_checkpoint_global_symbol_registered_variables=main_scope->symbol_registered_variables->size()-1;
		idx_clear_global_checkpoint_global_symbol_registered_functions=main_scope->symbol_registered_functions->size()-1;

		idx_clear_global_checkpoint_global_scopes=main_scope->registered_scopes->size()-1;

	}

	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(unsigned i = 0; i < scopes->size(); i++){
			delete (Scope *)scopes->at(i);
		}
		scopes->clear();

		delete scopes;

	}
};
