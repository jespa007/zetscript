
#include "zs_factory.h"

vector<CScope *> 						* current_vec_scope_node=NULL;

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
		vec_scope_node = new vector<tASTNode *>();
	}
}

vector<CScope *> 	*		CScopeFactory::getVecScopeNode(){
	return &vec_scope_node;


vector<CScope *> 	*	CScopeFactory::getCurrentVecScopeNode(){
	return current_vec_scope_node;
}

int	 CScopeFactory::newScope(CScope *scope_node){
	current_vec_scope_node->push_back(scope_node);
	scope_node->idxScope = current_vec_scope_node->size()-1;
	return scope_node->idxScope;
}


CScope 		* CScopeFactory::getScopeNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= current_vec_scope_node->size()){
		print_error_cr("CScope node out of bound");
		return NULL;
	}

	return current_vec_scope_node->at(idx);
}
