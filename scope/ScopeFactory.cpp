#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope = newScope(NULL,false); // create global scope (scope 0)
		idx_clear_checkpoint=1;  // start from MAIN scope
	}

	Scope *	 ScopeFactory::newScope(Scope * scope_parent,bool is_c_node){

		if(is_c_node){
			if(scopes->count > 1){ // if greather than 1 check if node consecutive...
				Scope *scope=(Scope *)scopes->get(scopes->count-1);
				if(!scope->is_c_node){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Scopes should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Scope * scope_node = new Scope(this->zs,scope_parent,is_c_node);
		scopes->push_back((zs_int)scope_node);
		return scope_node;
	}



	zs_vector 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clear(int _idx_start){
		int idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=idx_start;
			v < scopes->count; // avoid main scope
			v++
		){
			Scope * info_scope = (Scope *)scopes->get(v);
			if(info_scope->is_c_node == false){
				// if c breaks
				scopes->pop_back();
				delete info_scope;
			}

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
