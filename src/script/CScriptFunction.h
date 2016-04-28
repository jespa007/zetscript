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
	/**
	 * Return variable is assigned at the begin as undefined and when return keyword occurs,
	 * returnVariable is assigned as the result of expression after return. By default return variable is type void.
	 */
	CObject *returnVariable;


	CScope	*m_scope; // base scope...



};
