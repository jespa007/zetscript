#pragma once

class CScriptFunction: public CObject{

public:
	CScriptFunction(CScope *_globalscope=NULL);
	vector<CVirtualMachine::tInfoStatementOp> * getCompiledCode();

	CScope *getScope();
	/*
	 * eval: Evaluates its body.
	 */
//	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:

	vector<CVirtualMachine::tInfoStatementOp>  m_listStatements;
	vector<CScriptFunction *> m_function; // 0 has the scope base.
	CScope	*m_scope; // base scope...



};
