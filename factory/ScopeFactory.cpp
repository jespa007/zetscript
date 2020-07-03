#include "ZetScript.h"

namespace ZetScript{


	//-----------

	ScopeFactory::ScopeFactory(ZetScript *zs){
		this->zs=zs;
	}

	Scope *	 ScopeFactory::newScope(short idxParentScope,bool is_c_node){

		if(is_c_node){
			if(vec_scope_node.size() > 1){ // if greather than 1 check if node consecutive...
				if(!vec_scope_node.at(vec_scope_node.size()-1)->is_c_node){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Scopes should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Scope * scope_node = new Scope(this->zs,(short)vec_scope_node.size(), idxParentScope,is_c_node);
		vec_scope_node.push_back(scope_node);
		return scope_node;
	}


	Scope 		* ScopeFactory::getScope(short idx){
		if(idx < 0 || (unsigned)idx >= vec_scope_node.size()){
			THROW_RUNTIME_ERROR("Scope node out of bound");
			return NULL;
		}

		return vec_scope_node.at(idx);
	}

	std::vector<Scope *> 	*		ScopeFactory::getVectorScopeNode(){
		return &vec_scope_node;
	}

	void ScopeFactory::clear(){
		bool end=false;
		do{
			Scope * info_scope = vec_scope_node.at(vec_scope_node.size()-1);
			end=info_scope->is_c_node || vec_scope_node.size()==1;

			if(!end){


				vec_scope_node.pop_back();
				delete info_scope;

			}

		}while(!end);
	}


	ScopeFactory::~ScopeFactory(){

		// destroy all nodes ...
		for(unsigned i = 0; i < vec_scope_node.size(); i++){
					delete vec_scope_node.at(i);
		}
		vec_scope_node.clear();

	}
};
