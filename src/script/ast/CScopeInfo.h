#pragma once

//#include "script/ast/ast.h"



class CContext;


class CScopeInfo: public CObject{
public:

	static CUndefined *UndefinedSymbol;
	static CVoid *VoidSymbol;



	static void createSingletons();
	static void destroySingletons();


	//---------------------------------
	// Register functions
	tInfoScopeVar *getInfoRegisteredSymbol(const string & v, bool print_msg=true);


	tInfoScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL);

	/**
	 * Adds indexed symbol only for parent scope.
	 */
	bool addIndexedSymbol(tInfoScopeVar *);


	//bool defineSymbol(const string & var_name, CObject *obj);
	CScopeInfo(CScopeInfo * m_parent=NULL);

	CScopeInfo * getMainScope();



	CScopeInfo * getParent();
	CScopeInfo * getCurrentScopePointer();
	CScopeInfo * getRootScope();

	CScopeInfo * pushScope();
	CScopeInfo * popScope();

	void resetScopePointer();

	//---------------------------------



	//bool eval(const string & s);

	// execute instructions at local scope ...
	static bool execute(CScopeInfo *lc);

	~CScopeInfo();

private:

	//--------------------------------------------------------------------
	// Register functions

	//char * parseKeyword_Switch(const char * str,int & m_line,tInfoCase & info_case,bool & error);

	map<string,tInfoScopeVar *> m_registeredSymbol;
	vector<tInfoScopeVar *> m_indexedSymbol;



	//static CUndefined *m_defaultSymbol;

	// each scope has its registered vars.... if the compiler tries to find a var and this is not found goes through parent scopes until
	// it finds it.



	vector<CScopeInfo *> m_scopeList;

	// The a parent scope ...
	CScopeInfo *m_parentScope,*m_mainScope, *m_currentScopePointer, *m_baseScope;



	tInfoScopeVar * existRegisteredSymbol(const string & var_name);


	//typedef struct _tLocalScope;

	//typedef struct _tLocalScope{

		/*vector<tinfostatementop> tinfostatementop *m_currentstatement;*/

	//_tLocalScope *m_parentScope;


	//}tLocalScope;

	//typedef struct{

		//vector<tLocalScope *> m_localScope;
		//tLocalScope *base_scope;
		//map<string,tLocalScope *>	m_label;

	//}tContext;


		void unregisterOperators();
		void insertNewStatment();
		//tLocalScope * createLocalScope(tLocalScope *m_parent);
		//tContext * createContext();

		static bool isVarDeclarationStatment(const char *statment, bool & error,char **eval_expression, int & m_line, CScopeInfo * _localScope);
		static char * evalRecursive(const char * s, int & m_line,bool & error,CScopeInfo *local_scope,int level_scope=0);

		void addLocalScope(CScopeInfo *_ls);

	//	vector<tInfoStatementOp> * getListStatments();




};
