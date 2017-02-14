#include "zs_core.h"

CState * CState::vec_saved_state=NULL;
CState * CState::current_state=NULL;


CState::CState(){

}

bool  CState::setState(){
	return false;
}

int   CState::saveState(){

	// 1- Copy current state...
	// 1.1-Copy AST...
	for(unsigned int i = 0; i < vec_index_ast_node.size(); i++){

	}

	// 1.2-Copy ScopeInfo...
	for(unsigned int i = 0; i < vec_index_scope_node.size(); i++){

	}

	// 1.3-Copy InfoVar...
	for(unsigned int i = 0; i < vec_index_registered_var.size(); i++){

	}


	// 1.4-Copy FunctionInfo...
	for(unsigned int i = 0; i < vec_index_registered_var.size(); i++){

	}


	return -1;
}

