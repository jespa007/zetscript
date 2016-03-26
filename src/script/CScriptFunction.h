#pragma once

class CScriptFunction: public CObject{

public:
	CScriptFunction(CScriptFunction *_globalscope=NULL);
	vector<CVirtualMachine::tInfoStatementOp> * getCompiledCode();

	CScriptFunction * getGlobalScope();
	CScriptFunction * getMainScope();

	/*
	 * eval: Evaluates its body.
	 */
	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:

	vector<CVirtualMachine::tInfoStatementOp>  m_listStatements;
	vector<CScriptFunction *> m_function; // 0 has the scope base.
	CScriptFunction * m_globalscope;


	CScope	*m_scope; // base scope...
	CScope *m_currentEvaluatingScope;




};
