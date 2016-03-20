#pragma once

class CScriptFunction{

public:
	CScriptFunction(CScriptFunction *_base=NULL);
	vector<CVirtualMachine::tInfoStatementOp> * getCompiledCode();

	CScriptFunction * getBase();

	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:

	vector<CVirtualMachine::tInfoStatementOp>  m_listStatements;
	vector<CScriptFunction *> m_function; // 0 is the base scope.
	CScriptFunction * m_base;


	CScope	*m_scope; // base scope...
	CScope *m_currentEvaluatingScope;




};
