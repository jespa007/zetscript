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

	CScopeInfo * getMainScope();
	CScopeInfo * getParent();
	CScopeInfo * getCurrentScopePointer();
	void         generateScopeList(vector<CScopeInfo *> & vector);
	vector<CScopeInfo *> * getScopeList();

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

	vector<CScopeInfo *> m_scopeList;

	// The a parent scope ...
	CScopeInfo *m_parentScope,*m_mainScope, *m_currentScopePointer, *m_baseScope;



};
