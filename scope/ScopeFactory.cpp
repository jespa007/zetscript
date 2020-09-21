#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		this->scopes = new zs_vector;
		main_scope = newScope(NULL,false); // create global scope (scope 0)
	}

	Scope *	 ScopeFactory::newScope(Scope * scope_child,bool is_c_node){

		if(is_c_node){
			if(scopes->count > 1){ // if greather than 1 check if node consecutive...
				Scope *scope=(Scope *)scopes->get(scopes->count-1);
				if(!scope->is_c_node){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Scopes should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Scope * scope_node = new Scope(this->zs,scope_child,is_c_node);
		scopes->push_back((intptr_t)scope_node);
		return scope_node;
	}



	zs_vector 	*		ScopeFactory::getScopes(){
		return scopes;
	}

	void ScopeFactory::clear(){

		for(int v=scopes->count-1;
			v>=1; // avoid main scope
			v--
		){
			Scope * info_scope = (Scope *)scopes->get(v);
			if(info_scope->is_c_node == false){
				// if c breaks
				scopes->pop_back();
				delete info_scope;
			}else{
				break;
			}

		}
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
