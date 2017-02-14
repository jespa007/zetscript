
#pragma once

#define AST_NODE(idx) 						CState::getAstNodeByIdx(idx)
#define AST_SCOPE_INFO_IDX(idx) 			CState::getAstScopeIndexByIdx(idx)
#define AST_LINE_VALUE(idx) 				CState::getAstLineByIdx(idx)
#define AST_SYMBOL_VALUE(idx) 				CState::getAstSymbolNameByIdx(idx)
#define AST_SYMBOL_VALUE_CONST_CHAR(idx) 	CState::getAstSymbolNameConstCharByIdx(idx)

#define SCOPE_INFO_NODE(idx) 				CState::getScopeNodeByIdx(idx)
#define INFO_SCOPE_VAR_NODE(idx) 			CState::getScopeVarNodeByIdx(idx)


#define REGISTERED_CLASS_NODE(idx) 				CState::getRegisteredClassNodeByIdx(idx)
#define MAIN_CLASS_NODE							(CState::getRegisteredClassNodeByIdx(0))    // 0 is the main class

#define SCRIPT_FUNCTION_NODE(idx) 					CState::getScriptFunctionNodeByIdx(idx)
#define FUNCTION_SYMBOL_NODE(idx) 					CState::getFunctionSymbolNodeByIdx(idx)


class CState{

	static vector<CState *> 						* vec_saved_state;
	static CState 									* current_state;


	//static vector<tScopeVar *> 					* current_vec_scope_var_node;




	static string * aux_str;

	vector<tASTNode *> 							vec_ast_node; // ast collection register...
	vector<CScope *> 							vec_scope_node;
	vector<tScopeVar *> 						vec_scope_var_node;
	vector<tInfoRegisteredClass *> 				vec_registered_class_node;
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
	vector<tInfoRegisteredClass *> 		*	getVecClassNode();
	vector<tScriptFunctionObject *> 	*	getVecInfoFunctionSymbolNode();

	~CState();

};
