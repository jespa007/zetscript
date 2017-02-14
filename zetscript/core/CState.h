
#pragma once

class CState{

	static vector<CState *> *	vec_saved_state;
	static CState 			* 	current_state;


	vector<tASTNode *> 				vec_index_ast_node; // ast collection register...
	vector<CScopeInfo *> 			vec_index_scope_node;
	vector<tInfoScopeVar *> 		vec_index_registered_var;
	vector<tInfoRegisteredClass *> 	vec_registered_class;

public:

	static void createSingletons();
	static bool  setState();
	static int   saveState();
	static CState * currentState();
	static void destroySingletons();

};
