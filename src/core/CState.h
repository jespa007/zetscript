
#pragma once







class CState{

	static vector<CState *> 						* vec_saved_state;
	static CState 									* current_state;


	//static vector<tScopeVar *> 					* current_vec_scope_var_node;




	static string * aux_str;

	vector<tASTNode *> 							vec_ast_node; // ast collection register...
	vector<CScope *> 							vec_scope_node;
	vector<tScopeVar *> 						vec_scope_var_node;
	vector<tInfoScriptClass *> 				vec_registered_class_node;
	vector<tScriptFunctionObject *> 			vec_scipt_function_object_node;

public:

	static void createSingletons();
	static bool  setState(int idx);

	/**
	 * Save current state
	 */
	static int   saveState();

	static CState 			* currentState();





	/**
	 * Get tScopeVar Node by its idx, regarding current state.
	 */
	/*static vector<tScopeVar *> 				*	getCurrentVecScopeVarNode();
	static int 				  registerScopeVar(tScopeVar *info_scope_var);
	static tScopeVar 	* getScopeVarNodeByIdx(int idx);*/







	static void destroySingletons();

	CState();

	vector<tASTNode *> 					*	getVecAstNode(); // ast collection register...
	vector<CScope *> 					*	getVecScopeNode();
	vector<tScopeVar *> 				*	getVecScopeVarNode();
	vector<tInfoScriptClass *> 		*	getVecClassNode();
	vector<tScriptFunctionObject *> 	*	getVecScriptFunctionObjectNode();

	~CState();

};
