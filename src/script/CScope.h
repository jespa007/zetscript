#pragma once

class CContext;
class CScriptFunction;
class CScope: public CObject{
public:



	static void createSingletons();
	static void destroySingletons();


	//---------------------------------
	// Register functions
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool registerVariable(const string & var_name, int m_line);
	bool defineVariable(const string & var_name, CObject *obj);

	CScope(CScriptFunction *_comtext,CScope * m_parent=NULL);

	CScope * getMainScope();
	CScriptFunction * getScriptFunction();

	CScope * getParent();
	CScope * getRootScope();

	//---------------------------------



	bool eval(const string & s);

	// execute instructions at local scope ...
	static bool execute(CScope *lc);

	~CScope();

private:

	//--------------------------------------------------------------------
	// Register functions

	typedef struct{
		int m_line;
		CObject *m_obj;
	}tInfoRegisteredVar;

	map<string,tInfoRegisteredVar *> m_registeredVariable;

	//static CUndefined *m_defaultVar;

	// each scope has its registered vars.... if the compiler tries to find a var and this is not found goes through parent scopes until
	// it finds it.

	static CUndefined *m_defaultVar;
	vector<CScope *> m_scopeList;

	// The a parent scope ...
	CScope *m_parentScope,*m_mainScope;
	CScriptFunction * m_scriptFunction;


	tInfoRegisteredVar * existRegisteredVariable(const string & var_name);

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

		static bool isVarDeclarationStatment(const char *statment, bool & error,char **eval_expression, int & m_line, CScope * _localScope);
		static char * evalRecursive(const char * s, int & m_line,CScope *local_scope,int level_scope=0);

		void addLocalScope(CScope *_ls);

	//	vector<tInfoStatementOp> * getListStatments();




};
