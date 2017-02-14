#pragma once


class CASTNodeFactory{

	static vector<tASTNode *> 						* vec_ast_node; // ast collection register...

public:

	static void createSingletons();

	/**
	 * Get tASTNode Node by its idx, regarding current state.
	 */


	static vector<tASTNode *>	*	getVecAstNode(); // ast collection register...
	static int				  newAstNode(tASTNode *ast);
	static tASTNode 		* getAstNodeByIdx(int idx);
	static int 				  getAstScopeIndexByIdx(int idx);
	static int 				  getAstLineByIdx(int idx);
	static const string     & getAstSymbolNameByIdx(int idx);
	static const char 		* getAstSymbolNameConstCharByIdx(int idx);

	static void destroySingletons();


};
