#include "zetscript.h"

namespace zetscript{


	//-----------

	CScopeFactory::CScopeFactory(CZetScript *zs){
		this->zs=zs;
	}

	CScope *	 CScopeFactory::newScope(short idxParentScope,bool is_c_node){

		if(is_c_node){
			if(vec_scope_node.size() > 1){ // if greather than 1 check if node consecutive...
				if(!vec_scope_node.at(vec_scope_node.size()-1)->is_c_node){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Scopes should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		CScope * scope_node = new CScope(this->zs,(short)vec_scope_node.size(), idxParentScope,is_c_node);
		vec_scope_node.push_back(scope_node);
		return scope_node;
	}


	CScope 		* CScopeFactory::getScope(short idx){
		if(idx < 0 || (unsigned)idx >= vec_scope_node.size()){
			THROW_RUNTIME_ERROR("CScope node out of bound");
			return NULL;
		}

		return vec_scope_node.at(idx);
	}

	std::vector<CScope *> 	*		CScopeFactory::getVectorScopeNode(){
		return &vec_scope_node;
	}

	void CScopeFactory::clear(){
		bool end=false;
		do{
			CScope * info_scope = vec_scope_node.at(vec_scope_node.size()-1);
			end=info_scope->is_c_node || vec_scope_node.size()==1;

			if(!end){


				vec_scope_node.pop_back();
				delete info_scope;

			}

		}while(!end);
	}


	CScopeFactory::~CScopeFactory(){

		// destroy all nodes ...
		for(unsigned i = 0; i < vec_scope_node.size(); i++){
					delete vec_scope_node.at(i);
		}
		vec_scope_node.clear();

	}
};
