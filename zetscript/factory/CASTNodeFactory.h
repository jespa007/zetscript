#pragma once


#define MAIN_AST_ROOT						(CASTNodeFactory::getAstNode(0))
#define AST_NODE(idx) 						CASTNodeFactory::getAstNode(idx)
#define AST_SCOPE_INFO_IDX(idx) 			CASTNodeFactory::getScopeIdx(idx)
#define AST_LINE_VALUE(idx) 				CASTNodeFactory::getAstLine(idx)
#define AST_SYMBOL_VALUE(idx) 				CASTNodeFactory::getAstSymbolName(idx)
#define AST_SYMBOL_VALUE_CONST_CHAR(idx) 	CASTNodeFactory::getAstSymbolNameConstChar(idx)


class CASTNodeFactory{

	static vector<tASTNode *> 						* vec_ast_node; // ast collection register...
	static string *aux_str;
public:

	static void createSingletons();
	static void set(vector<tASTNode *> 	& set_vec_ast_node);

	/**
	 * Get tASTNode Node by its idx, regarding current state.
	 */

	static tASTNode				*  	newAstNode();
	static vector<tASTNode *>	*	getVecAstNode(); // ast collection register...

	static tASTNode 			* 	getAstNode(int ast_idx);
	static int 				  		getScopeIdx(int ast_idx);
	static int 				  		getAstLine(int ast_idx);
	static const string     	& 	getAstSymbolName(int ast_idx);
	static const char 			*	getAstSymbolNameConstChar(int ast_idx);

	static void destroySingletons();


};
