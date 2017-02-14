#include "zs_factory.h"


vector<tASTNode *> 							* CASTNodeFactory::vec_ast_node=NULL;

/*
//		  ___   _____ _____   _   _           _
//		 / _ \ /  ___|_   _| | \ | |         | |
//		/ /_\ \\ `--.  | |   |  \| | ___   __| | ___
//		|  _  | `--. \ | |   | . ` |/ _ \ / _` |/ _ \
//		| | | |/\__/ / | |   | |\  | (_) | (_| |  __/
//		\_| |_/\____/  \_/   \_| \_/\___/ \__,_|\___|
// _________________________________________________
//  __________________________________
*/

void CASTNodeFactory::createSingletons(){
	if(vec_ast_node==NULL){
		vec_ast_node = new vector<tASTNode *>();
	}
}

vector<tASTNode *> 		*		CASTNodeFactory::getVecAstNode(){
	return &vec_ast_node;
}

int	CASTNodeFactory::newAstNode(tASTNode *ast_node){

	vec_ast_node->push_back(ast_node);
	ast_node->idxAstNode = vec_ast_node->size()-1;
	return ast_node->idxAstNode;
}


/**
 * Get tASTNode Node by its idx, regarding current state.
 */
tASTNode 		* CASTNodeFactory::getAstNodeByIdx(int idx){
	if(idx==-1){
		return NULL;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return NULL;
	}
	 return vec_ast_node->at(idx);
}

int 	CASTNodeFactory::getAstScopeIndexByIdx(int idx){
	if(idx==-1){
		return -1;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return -1;
	}

	return vec_ast_node->at(idx)->idxScope;
}

int 		CASTNodeFactory::getAstLineByIdx(int idx){
	if(idx==-1){
		return -1;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return -1;
	}

	return vec_ast_node->at(idx)->line_value;
}

const string & CASTNodeFactory::getAstSymbolNameByIdx(int idx){
	if(idx==-1){
		return *aux_str;
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return *aux_str;
	}

	return vec_ast_node->at(idx)->symbol_value;
}

const char 		* CASTNodeFactory::getAstSymbolNameConstCharByIdx(int idx){
	if(idx==-1){
		return aux_str->c_str();
	}

	if(idx < 0 || (unsigned)idx >= vec_ast_node->size()){
		print_error_cr("Ast node out of bound");
		return NULL;
	}

	return vec_ast_node->at(idx)->symbol_value.c_str();
}
