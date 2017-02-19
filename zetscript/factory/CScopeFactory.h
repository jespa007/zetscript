#pragma once


#define SCOPE_INFO_NODE(idx) 				CScopeFactory::getScopeNodeByIdx(idx)


class CScopeFactory{
	static vector<CScope *> 					* vec_scope_node;
public:

	static void createSingletons();
	static void set(vector<CScope *> 	& set_vec);

	/**
	 * Get CScope Node by its idx, regarding current state.
	 */

	static CScope	    		*	newScope(CScope *parent_scope);
	static vector<CScope *> 	*	getVecScopeNode();
	static CScope 				* 	getScopeNodeByIdx(int idx);

	static void destroySingletons();
};
