#pragma once

#define SCOPE_INFO_NODE(idx) 				CScope::getScopeNodeByIdx(idx)


class CContext;




class CScope{

public:



	static void 					setVectorScopeNode(vector<CScope *> 	* set_vec);
	static vector<CScope *> 	* 	getVectorScopeNode();

	/**
	 * Get CScope Node by its idx, regarding current state.
	 */

	static CScope	    		*	newScope(CScope *parent_scope);
	static CScope 				* 	getScopeNodeByIdx(int idx);

	int idxScope;

	//---------------------------------
	// Register functions

	//--------------------------------------------------------------------
	// Register functions
	vector<tScopeVar> m_registeredVariable; // vars registered from base.
	vector<tScopeVar> m_registeredAnoymouseFunction; // anonymous functions registered from base.

	tScopeVar * getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tScopeVar * registerAnonymouseFunction(PASTNode ast);
	tScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL);
	tScopeVar * existRegisteredSymbol(const string & var_name);


	CScope(CScope * m_parent);//, int _index);

	CScope * getBaseScope();
	CScope * getParent();
	CScope * getCurrentScopePointer();
	//void         generateScopeList(vector<CScope *> & vector);
	//int          getIndex();
	vector<int> * getLocalScopeList();

	vector<tScopeVar *> * getRegisteredVariableList();

	//int incTotalScopes();

	CScope * pushScope();
	CScope * popScope();

	//void destroyScopes();

	void resetScopePointer();

	~CScope();

private:

	//void deleteScopeRecursive(CScope *scope_info);

	//void generateScopeListRecursive(CScope * scope, vector<CScope *> & vector);
	//static int getScopeIndexRecursive(CScope * current_scope, CScope *scope_to_find,int & _index);

	vector<int> m_localScopeList;

	// The a parent scope ...
	CScope *m_parentScope, *m_currentScopePointer, *m_baseScope;
	//int m_index;

	static vector<CScope *> 					* vec_scope_node;


};
