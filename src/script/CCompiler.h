#pragma once
class CContext;
class CScriptFunction;
class CCompiler{

public:

	static CCompiler * getInstance();

	static void destroySingletons();


	bool compileExpression(const char *advanced_expression, int & m_line,CScriptFunction * sf, CScope *currentEvaluatingScope);

	int generateAsmCode(PASTNode op, int & numreg, bool & error, CScope * _lc);


	CVirtualMachine::ASM_OPERATOR getNumberOperatorId_TwoOps(const string & op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getIntegerOperatorId_TwoOps(const string & op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getBoleanOperatorId_TwoOps(const string & op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getStringOperatorId_TwoOps(const string & op,CVirtualMachine::VAR_TYPE & result_type);

	CVirtualMachine::ASM_OPERATOR getNumberOperatorId_OneOp(const string & op);
	CVirtualMachine::ASM_OPERATOR getBoleanOperatorId_OneOp(const string & op);
	CVirtualMachine::ASM_OPERATOR getStringOperatorId_OneOp(const string & op);

	bool insertLoadValueInstruction(const string & v, string & type_ptr, CScope * _lc);
	bool insertMovVarInstruction(CObject *var, int right);

	bool insertOperatorInstruction(const string &op, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	CVirtualMachine::VAR_TYPE getTypeAsmResult(int index);

	void insertPushScopeInstruction(CScope * _goto_scope);
	void insertPopScopeInstruction();

private:
	static CCompiler *m_compiler;


	vector<CVirtualMachine::tInfoStatementOp > 	*m_currentListStatements;
	CScriptFunction 							*m_currentScriptFunction;


/*
	//tInfoRegisteredVar * existRegisteredVariableRecursive(const string & var_name);
	tInfoRegisteredVar * existRegisteredVariable(const string & var_name);
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool defineVariable(const string & var_name, CObject *obj);*/
	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
