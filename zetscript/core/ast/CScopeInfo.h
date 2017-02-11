#pragma once




class CContext;


#define SCOPE_INFO_NODE(idx) CScopeInfo::getScopeInfoByIdx(idx)
#define INFO_SCOPE_VAR_NODE(idx) CScopeInfo::getInfoScopeVarByIdx(idx)

class CScopeInfo{
	//int totalScopes;
public:

	static void createSingletons();
	static int insertScopeInfoNode(CScopeInfo *scp);
	static int insertInfoScopeVar(tInfoScopeVar *isv);
	static CScopeInfo * getScopeInfoByIdx(int index);
	static tInfoScopeVar * getInfoScopeVarByIdx(int index);
	static vector<CScopeInfo *> * getVecIndexScopeNode();
	static void destroySingletons();

	//int getScopeIndex();


	//---------------------------------
	// Register functions
	int getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tInfoScopeVar * registerAnonymouseFunction(PASTNode ast);
	int registerSymbol(const string & var_name, PASTNode ast=NULL);
	int existRegisteredSymbol(const string & var_name);
	vector<tInfoScopeVar *> * getRegisteredSymbolsList();

	CScopeInfo(CScopeInfo * m_parent);//, int _index);

	CScopeInfo * getBaseScope();
	CScopeInfo * getParent();
	CScopeInfo * getCurrentScopePointer();
	//void         generateScopeList(vector<CScopeInfo *> & vector);
	int          getIndex();
	vector<CScopeInfo *> * getScopeList();

	vector<tInfoScopeVar *> * getRegisteredVariableList();

	//int incTotalScopes();

	CScopeInfo * pushScope();
	CScopeInfo * popScope();

	void destroyScopes();

	void resetScopePointer();

	~CScopeInfo();

private:

	void deleteScopeRecursive(CScopeInfo *scope_info);

	//--------------------------------------------------------------------
	// Register functions
	vector<tInfoScopeVar *> m_registeredVariable; // vars registered from base.
	vector<tInfoScopeVar *> m_registeredAnoymouseFunction; // anonymous functions registered from base.
	//void generateScopeListRecursive(CScopeInfo * scope, vector<CScopeInfo *> & vector);
	//static int getScopeIndexRecursive(CScopeInfo * current_scope, CScopeInfo *scope_to_find,int & _index);

	vector<CScopeInfo *> m_scopeList;
	static vector<CScopeInfo *> 	* vec_index_scope_node;
	static vector<tInfoScopeVar *> 	* vec_index_registered_var;

	// The a parent scope ...
	CScopeInfo *m_parentScope, *m_currentScopePointer, *m_baseScope;
	int m_index;




};
