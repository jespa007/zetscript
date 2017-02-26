
#pragma once







class CState{

	static vector<CState *> 						* vec_saved_state;
	static CState 									* current_state;


	//static vector<tScopeVar *> 					* current_vec_scope_var_node;




	static string * aux_str;

	void unloadRecursiveFunctions(CScriptFunctionObject * info_function);
	void destroyScriptClassNodes();

	vector<CASTNode *> 							vec_ast_node; // ast collection register...
	vector<CScope *> 							vec_scope_node;
	vector<tScopeVar *> 						vec_scope_var_node;
	vector<CScriptClass *> 						vec_script_class_node;
	vector<CScriptFunctionObject *> 			vec_scipt_function_object_node;
	vector<tInfoParsedSource> 				vec_info_parsed_source_node;

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

	vector<CASTNode *> 					*	getVecAstNode(); // ast collection register...
	vector<CScope *> 					*	getVecScopeNode();
	vector<tScopeVar *> 				*	getVecScopeVarNode();
	vector<CScriptClass *> 				*	getScriptClassNode();
	vector<CScriptFunctionObject *> 	*	getVecScriptFunctionObjectNode();
	vector<tInfoParsedSource> 		*	getVectorInfoParsedSourceNode();

	~CState();

};
