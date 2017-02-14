#pragma once


class CScopeFactory{
	static vector<CScope *> 					* vec_scope_node;
public:

	static void createSingletons();

	/**
	 * Get CScope Node by its idx, regarding current state.
	 */

	static int	    		 newScope(CScope * scope_node);
	static vector<CScope *> 					*	getCurrentVecScopeNode();

	static CScope 		* getScopeNodeByIdx(int idx);

	static void destroySingletons();
};
