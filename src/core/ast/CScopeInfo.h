#pragma once



class CContext;


class CScopeInfo{
public:


	//---------------------------------
	// Register functions
	tInfoScopeVar * getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tInfoScopeVar * registerAnonymouseFunction(PASTNode ast);
	tInfoScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL);
	tInfoScopeVar * existRegisteredSymbol(const string & var_name);
	vector<tInfoScopeVar *> * getRegisteredSymbolsList();

	CScopeInfo(CScopeInfo * m_parent);

	CScopeInfo * getBaseScope();
	CScopeInfo * getParent();
	CScopeInfo * getCurrentScopePointer();
	void         generateScopeList(vector<CScopeInfo *> & vector);
	vector<CScopeInfo *> * getScopeList();
	static int getScopeIndex(CScopeInfo * scope);

	CScopeInfo * pushScope();
	CScopeInfo * popScope();

	void resetScopePointer();

	~CScopeInfo();

private:


	//--------------------------------------------------------------------
	// Register functions
	vector<tInfoScopeVar *> m_registeredVariable; // vars registered from base.
	vector<tInfoScopeVar *> m_registeredAnoymouseFunction; // anonymous functions registered from base.
	void generateScopeListRecursive(CScopeInfo * scope, vector<CScopeInfo *> & vector);
	static int getScopeIndexRecursive(CScopeInfo * current_scope, CScopeInfo *scope_to_find,int & _index);

	vector<CScopeInfo *> m_scopeList;

	// The a parent scope ...
	CScopeInfo *m_parentScope, *m_currentScopePointer, *m_baseScope;




};
