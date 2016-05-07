#pragma once

class CScriptFunction: public CObject{

public:


	typedef struct{
		int m_line;
		CScriptFunction *m_scriptFunction;
	}tInfoRegisteredClass;


	tInfoRegisteredClass *getRegisteredClass(const string & v, bool print_msg=true);
	CScriptFunction * registerClass(const string & class_name, int line);


	CScriptFunction(CScriptFunction *_parentFunction=NULL);
	vector<CVirtualMachine::tInfoStatementOp> * getCompiledCode();

	CScriptFunction *getParent();

	CScope *getScope();

	/*
	 * eval: Evaluates its body.
	 */
//	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:


	vector<CVirtualMachine::tInfoStatementOp>  m_listStatements;
	map<string,tInfoRegisteredClass *>  	 m_registeredClass;
	/**
	 * Return variable is assigned at the begin as undefined and when return keyword occurs,
	 * returnVariable is assigned as the result of expression after return. By default return variable is type void.
	 */
	CObject *returnVariable;
	tInfoRegisteredClass * existRegisteredClass(const string & class_name);


	CScope	*m_scope; // base scope...
	CScriptFunction *m_parentFunction;



};
