#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope=newScope(IDX_SCRIPT_FUNCTION_MAIN,NULL,false); // create global scope (scope 0)
		idx_clear_checkpoint=1;  // start from MAIN scope
	}




	Scope *	 ScopeFactory::newScope(int idx_sf,Scope * scope_parent,bool is_c_node){
		Scope * scope_node = new Scope(this->zs,idx_sf,scope_parent,is_c_node);
		scopes->push_back((zs_int)scope_node);
		return scope_node;
	}

	zs_vector 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clear(int _idx_start){
		int idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=scopes->count-1;
			v >= idx_start; // avoid main scope
			v--
		){
			Scope * info_scope = (Scope *)scopes->get(v);
			delete info_scope;
			scopes->pop_back();
		}
	}

	void ScopeFactory::setClearCheckpoint(){
		idx_clear_checkpoint=scopes->count-1;
	}


	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(unsigned i = 0; i < scopes->count; i++){
			delete (Scope *)scopes->get(i);
		}
		scopes->clear();

		delete scopes;

	}
};
