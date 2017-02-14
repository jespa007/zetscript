#include "zs_core.h"

vector<CState *> *  CState::vec_saved_state=NULL;
CState * CState::current_state=NULL;
//vector<tASTNode *> 							* current_vec_ast_node=NULL; // ast collection register...
//vector<tScopeVar *> 					* current_vec_info_scope_var_node=NULL;





string * CState::aux_str= NULL;//"1234_unknow_1234";

void CState::createSingletons(){
	if(vec_saved_state == NULL){
		vec_saved_state = new vector<CState *> ();
		// create state by default ...
		//current_state = new CState();
		//current_vec_ast_node = current_state->getVecAstNode();
		//current_vec_scope_node = current_state->getVecScopeNode();
		//current_vec_info_scope_var_node = current_state->getVecScopeVarNode();
		//current_vec_registered_class_node= current_state->getVecClassNode();
		//current_vec_info_registered_function_symbol_node = current_state->getVecInfoFunctionSymbolNode();
	}
}

bool  CState::setState(int idx){
	if(idx < 0 || (unsigned)idx >= vec_saved_state->size()){
		print_error_cr("index out of bounds");
		return false;
	}

	current_state = vec_saved_state->at(idx);

	/*current_vec_ast_node = current_state->getVecAstNode();
	current_vec_scope_node = current_state->getVecScopeNode();
	current_vec_info_scope_var_node = current_state->getVecScopeVarNode();
	current_vec_registered_class_node= current_state->getVecRegisteredClassNode();
	current_vec_info_registered_function_symbol_node = current_state->getVecInfoRegisteredFunctionFunctionNode();*/

	// TODO:maybe is better to re-run vm to optimize vec acceses

	return false;
}

int   CState::saveState(){

	CState * state = new CState();

	// 1- Copy current state...
	// 1.1-Copy AST...
	for(unsigned int i = 0; i < current_vec_ast_node->size(); i++){

	}

	// 1.2-Copy Scope...
	for(unsigned int i = 0; i < current_vec_scope_node->size(); i++){

	}

	// 1.3-Copy InfoVar...
	for(unsigned int i = 0; i < current_vec_info_scope_var_node->size(); i++){

	}

	// 1.4-Copy Class...
	for(unsigned int i = 0; i < current_vec_registered_class_node->size(); i++){

	}

	// 1.5-Copy FunctionInfo...
	for(unsigned int i = 0; i < current_vec_info_registered_function_symbol_node->size(); i++){

	}


	vec_saved_state->push_back(state);

	return -1;
}

CState * CState::currentState(){
	return current_state;
}






/*
//   _____       __        _____                        _   _
//	|_   _|     / _|      /  ___|                      | | | |
// 	  | | _ __ | |_ ___   \ `--.  ___ ___  _ __   ___  | | | | __ _ _ __
// 	  | || '_ \|  _/ _ \   `--. \/ __/ _ \| '_ \ / _ \ | | | |/ _` | '__|
//   _| || | | | || (_) | /\__/ / (_| (_) | |_) |  __/ \ \_/ / (_| | |
//   \___/_| |_|_| \___/  \____/ \___\___/| .__/ \___|  \___/ \__,_|_|
//                                        | |
//                                        |_|
// 	_________________________________________________
// 	 	__________________________________
*/
/*
vector<tScopeVar *> 				*	CState::getCurrentVecScopeVarNode(){
	return current_vec_scope_var_node;
}

int 				  	CState::registerScopeVar(tScopeVar *info_scope_var){
	current_vec_info_scope_var_node->push_back(info_scope_var);
	info_scope_var->idxScopeVar = current_vec_info_scope_var_node->size()-1;
	return info_scope_var->idxScopeVar;
}

tScopeVar 	* CState::getScopeVarNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= current_vec_info_scope_var_node->size()){
		print_error_cr("tScopeVar node out of bound");
		return NULL;
	}

	return current_vec_info_scope_var_node->at(idx);
}
*/




//------------------------------------------------------------------------------------------------------

CState::CState(){

}


vector<tASTNode *> 		*		CState::getVecAstNode(){
	return &vec_ast_node;
}
vector<CScope *> 	*		CState::getVecScopeNode(){
	return &vec_scope_node;
}

/*
vector<tScopeVar *> 	*	CState::getVecScopeVarNode(){
	return &vec_scope_var_node;
}*/

vector<tInfoRegisteredClass *> 	*CState::getVecClassNode(){
	return &vec_registered_class_node;
}

vector<tScriptFunctionObject *> 	*CState::getVecInfoFunctionSymbolNode(){
	return &vec_scipt_function_object_node;
}

CState::~CState(){

}

