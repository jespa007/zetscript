#pragma once



class CContext;


class CScopeInfo{
	int totalScopes;
public:


	//---------------------------------
	// Register functions
	tInfoScopeVar * getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tInfoScopeVar * registerAnonymouseFunction(PASTNode ast);
	tInfoScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL);
	tInfoScopeVar * existRegisteredSymbol(const string & var_name);
	vector<tInfoScopeVar *> * getRegisteredSymbolsList();

	CScopeInfo(CScopeInfo * m_parent, int _index);

	CScopeInfo * getBaseScope();
	CScopeInfo * getParent();
	CScopeInfo * getCurrentScopePointer();
	void         generateScopeList(vector<CScopeInfo *> & vector);
	int          getIndex();
	vector<CScopeInfo *> * getScopeList();
	vector<tInfoScopeVar *> * getRegisteredVariableList();
	static int getScopeIndex(CScopeInfo * scope);
	int incTotalScopes();

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
	void generateScopeListRecursive(CScopeInfo * scope, vector<CScopeInfo *> & vector);
	static int getScopeIndexRecursive(CScopeInfo * current_scope, CScopeInfo *scope_to_find,int & _index);

	vector<CScopeInfo *> m_scopeList;

	// The a parent scope ...
	CScopeInfo *m_parentScope, *m_currentScopePointer, *m_baseScope;
	int m_index;




};
