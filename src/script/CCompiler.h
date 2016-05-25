#pragma once
class CContext;
class CScope;
class CScriptFunction;
class CCompiler{

public:

	static CCompiler * getInstance();

	static void destroySingletons();


	bool compileExpression(const char *expression_str, int & m_line,CScriptFunction * sf, CScope *currentEvaluatingScope);
	bool compile(const string & s, CScriptFunction * pr);

	//int generateAsmCode(PASTNode op, int & numreg, bool & error, CScope * _lc);


	CVirtualMachine::ASM_OPERATOR getNumberOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getIntegerOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getBoleanOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVirtualMachine::VAR_TYPE & result_type);
	CVirtualMachine::ASM_OPERATOR getStringOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVirtualMachine::VAR_TYPE & result_type);

	CVirtualMachine::ASM_OPERATOR getNumberOperatorId_OneOp(PUNCTUATOR_TYPE op);
	CVirtualMachine::ASM_OPERATOR getBoleanOperatorId_OneOp(PUNCTUATOR_TYPE op);
	CVirtualMachine::ASM_OPERATOR getStringOperatorId_OneOp(PUNCTUATOR_TYPE op);

	/**
	 * Load value or symbol and insert asm operation at current statment.
	 */
	CVirtualMachine::tInfoStatementOp  *newStatment();
	bool insertLoadValueInstruction(const string & value, string & type_ptr, CScope * _lc, int var_at_line);
	bool insertMovVarInstruction(CObject *var, int right);
	CVirtualMachine::tInfoAsmOp * insert_JMP_Instruction();
	CVirtualMachine::tInfoAsmOp * insert_JNT_Instruction();
	CVirtualMachine::tInfoAsmOp * insert_JT_Instruction();
	void insert_NOP_Instruction();

	bool insertOperatorInstruction(tInfoPunctuator *  op, string & error_str, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	bool *getObjectResultCurrentStatmentAsBoolean();
	int getCurrentInstructionIndex();
	int getCurrentStatmentIndex();

	CVirtualMachine::VAR_TYPE getTypeAsmResult(int index);

	void insertPushScopeInstruction(CScope * _goto_scope);
	void insertPopScopeInstruction();

private:
	static CCompiler *m_compiler;


	//---------------------------------------------------------------------------------------------------------------------------------------
	// DEBUG TOOLS

	typedef struct{
		string src;
		int asm_statment_idx;
	}tDebugInformation;

	vector<tDebugInformation>	m_debugInfo;

	void insertDebugInformation(int _asm_stament_idx, const char *src_str);
	void printDebugInformation();

	// DEBUG TOOLS
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	int  gacExpression_Recursive(PASTNode op, int & numreg, bool & error, CScriptFunction *sf);
	bool  gacExpression(PASTNode op, CScriptFunction *sf);

	bool gacFor(PASTNode _node);
	bool gacWhile(PASTNode _node);
	bool gacIfElse(PASTNode _node);
	bool gacIf(PASTNode _node);
	bool generateAsmCode_Recursive(PASTNode _node);
	bool ast2asm(PASTNode _node, CScriptFunction *sf);
	bool ast2asm_Recursive(PASTNode _node, CScriptFunction *sf);

	vector<CVirtualMachine::tInfoStatementOp > 	*m_currentListStatements;
	CScriptFunction 							*m_currentScriptFunction;


/*
	//tInfoRegisteredVar * existRegisteredSymbolRecursive(const string & var_name);
	tInfoRegisteredVar * existRegisteredSymbol(const string & var_name);
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool defineSymbol(const string & var_name, CObject *obj);*/
	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
