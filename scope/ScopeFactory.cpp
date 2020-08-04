#include "zetscript.h"

namespace zetscript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
		main_scope = newScope(NULL,false); // create global scope (scope 0)
	}

	Scope *	 ScopeFactory::newScope(Scope * scope_parent,bool is_c_node){

		if(is_c_node){
			if(scopes.size() > 1){ // if greather than 1 check if node consecutive...
				if(!scopes.at(scopes.size()-1)->is_c_node){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Scopes should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Scope * scope_node = new Scope(this->zs,scope_parent,is_c_node);
		scopes.push_back(scope_node);
		return scope_node;
	}



	std::vector<Scope *> 	*		ScopeFactory::getScopes(){
		return &scopes;
	}

	void ScopeFactory::clear(){
		bool end=false;
		do{
			Scope * info_scope = scopes.at(scopes.size()-1);
			end=info_scope->is_c_node || scopes.size()==1;

			if(!end){
				scopes.pop_back();
				delete info_scope;
			}

		}while(!end);
	}


	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(unsigned i = 0; i < scopes.size(); i++){
					delete scopes.at(i);
		}
		scopes.clear();

	}
};
