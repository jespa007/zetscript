#pragma once




class CContext;




class CScope{
	//int totalScopes;
public:

	//static void createSingletons();
	//static int insertScopeNode(CScope *scp);
	//static int insertScopeVar(tScopeVar *isv);
	//static CScope * getScopeByIdx(int index);
	//static tScopeVar * getScopeVarByIdx(int index);
	//static vector<CScope *> * getVecIndexScopeNode();
	//static void destroySingletons();

	//int getScopeIndex();

	int idxScope;

	//---------------------------------
	// Register functions
	//int getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tScopeVar * registerAnonymouseFunction(PASTNode ast);
	bool registerSymbol(const string & var_name, PASTNode ast=NULL);
	bool existRegisteredSymbol(const string & var_name);
	vector<tScopeVar *> * getRegisteredSymbolsList();

	CScope(CScope * m_parent);//, int _index);

	CScope * getBaseScope();
	CScope * getParent();
	CScope * getCurrentScopePointer();
	//void         generateScopeList(vector<CScope *> & vector);
	//int          getIndex();
	vector<CScope *> * getScopeList();

	vector<tScopeVar *> * getRegisteredVariableList();

	//int incTotalScopes();

	CScope * pushScope();
	CScope * popScope();

	//void destroyScopes();

	void resetScopePointer();

	~CScope();

private:

	//void deleteScopeRecursive(CScope *scope_info);

	//--------------------------------------------------------------------
	// Register functions
	vector<tScopeVar> m_registeredVariable; // vars registered from base.
	vector<tScopeVar> m_registeredAnoymouseFunction; // anonymous functions registered from base.
	//void generateScopeListRecursive(CScope * scope, vector<CScope *> & vector);
	//static int getScopeIndexRecursive(CScope * current_scope, CScope *scope_to_find,int & _index);

	vector<CScope *> m_scopeList;

	// The a parent scope ...
	CScope *m_parentScope, *m_currentScopePointer, *m_baseScope;
	//int m_index;




};
