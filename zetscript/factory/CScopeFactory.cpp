
#include "zs_factory.h"

vector<CScope *> 						* CScopeFactory::vec_scope_node=NULL;

/*
//	 _____                        _____       __
//	/  ___|                      |_   _|     / _|
//	\ `--.  ___ ___  _ __   ___    | | _ __ | |_ ___
//	 `--. \/ __/ _ \| '_ \ / _ \   | || '_ \|  _/ _ \
//	/\__/ / (_| (_) | |_) |  __/  _| || | | | || (_) |
//	\____/ \___\___/| .__/ \___|  \___/_| |_|_| \___/
//               | |
//               |_|
// _________________________________________________
//  __________________________________
*/

void CScopeFactory::createSingletons(){
	if(vec_scope_node==NULL){
		vec_scope_node = new vector<CScope *>();
	}
}

void CScopeFactory::destroySingletons(){
	if(vec_scope_node!=NULL){
		delete vec_scope_node;
		vec_scope_node = NULL;
	}
}

void CScopeFactory::set(vector<CScope *> 	& set_vec){
	*vec_scope_node = set_vec;
}


vector<CScope *> 	*		CScopeFactory::getVecScopeNode(){
	return &vec_scope_node;
}



CScope *	 CScopeFactory::newScope(CScope *parent_scope){
	CScope * scope_node = new CScope(parent_scope);
	vec_scope_node->push_back(scope_node);
	scope_node->idxScope = vec_scope_node->size()-1;
	return scope_node;
}


CScope 		* CScopeFactory::getScopeNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= vec_scope_node->size()){
		print_error_cr("CScope node out of bound");
		return NULL;
	}

	return vec_scope_node->at(idx);
}
