#pragma once
class CContext;
class CScope;

class CScriptFunction;

/**
 * The compiler does:
 * -Generates byte code instruction through AST and ScriptFunction object given.
 * -Registers classes.
 * -Registers symbols.
 */
class CCompiler{

public:





	static CCompiler * getInstance();

	static void destroySingletons();

	//---------------------------------------------------------------------------------------------------------------------------------------
	// CONSTANT TOOLS

	static CObject *getConstant(const string & const_name);
	static void addConstant(const string & const_name, CObject *obj);

	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMMON COMPILE FUNCTIONS

	bool compileExpression(const char *expression_str, int & m_line,CScriptFunction * sf, CScope *currentEvaluatingScope);
	bool compile(const string & s, CScriptFunction * pr);





	/**
	 * Load value or symbol and insert asm operation at current statment.
	 */
	tInfoStatementOp  *newStatment();
	bool insertLoadValueInstruction(PASTNode _node, CScope * _lc);
	bool insertMovVarInstruction( int left_index, int right_index);

	/**
	 * Unconditional Jump instruction
	 */
	tInfoAsmOp * insert_JMP_Instruction(int jmp_statement =0);

	/**
	 * Jump Not True (JNT) instruction
	 */
	tInfoAsmOp * insert_JNT_Instruction(int jmp_statement =0);

	/**
	 * Jump if True (JT) instruction
	 */
	tInfoAsmOp * insert_JT_Instruction(int jmp_statement =0);
	void insert_NOP_Instruction();

	/**
	 * IndexAccess
	 */

	void insert_CreateArrayObject_Instruction();
	//void insert_LoadArrayObject_Instruction(CObject *obj);
	void insert_ArrayObject_PushValueInstruction(int ref_vec_object_index, int index_instruction_to_push=-1);

	void insert_ArrayAccess_Instruction(int vect_object, int index_instrucction, tASTNode *_ast);


	/**
	 * Function instructions
	 */
	void insert_LoadFunctionObject_Instruction(CObject *obj);
	void insert_ClearArgumentStack_Instruction();
	void insert_PushArgument_Instruction();
	void insert_CallFunction_Instruction(int index_call);
	void insertRet(int index);


	bool insertOperatorInstruction(tInfoPunctuator *  op, PASTNode _node, string & error_str, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	bool *getObjectResultCurrentStatmentAsBoolean();
	int getCurrentInstructionIndex();
	int getCurrentStatmentIndex();

	CVariable::VAR_TYPE getTypeAsmResult(int index);

	void insertPushScopeInstruction(CScope * _goto_scope);
	void insertPopScopeInstruction();


private:
	static CCompiler *m_compiler;
	static map<string,CObject *> *constant_pool;


	//---------------------------------------------------------------------------------------------------------------------------------------
	// DEBUG TOOLS

	typedef struct{
		string src;
		int asm_statment_idx;
	}tDebugInformation;

	vector<tDebugInformation>	m_debugInfo;

	static char print_aux_load_value[512];

	void insertDebugInformation(int _asm_stament_idx, const char *src_str);
	void printDebugInformation();
	static const char * getStrTypeLoadValue(tInfoAsmOp * iao);
	static const char * getStrMovVar(tInfoAsmOp * iao);

	static void printGeneratedCode_Recursive(CScriptFunction *fs);
	static void printGeneratedCode(CScriptFunction *fs);

	// DEBUG TOOLS
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	ASM_OPERATOR puntuator2instruction(tInfoPunctuator * op);
	ASM_PRE_POST_OPERATORS preoperator2instruction(PUNCTUATOR_TYPE op);

	int gacExpression_ArrayObject_Recursive(PASTNode _node, CScope *_lc);
	int gacExpression_ArrayObject(PASTNode op, CScope *_lc);
	int gacExpression_FunctionObject(PASTNode op, CScope *_lc);
	int gacExpression_FunctionAccess(PASTNode op, CScope *_lc);
	int gacExpression_ArrayAccess(PASTNode op, CScope *_lc);
	int  gacExpression_Recursive(PASTNode op, CScope * _lc, int & numreg);
	bool  gacExpression(PASTNode op, CScope * _lc,int index_instruction=-1);

	bool gacKeyword(PASTNode _node, CScope * _lc);
	bool gacFor(PASTNode _node, CScope * _lc);
	bool gacVar(PASTNode _node, CScope * _lc);
	bool gacWhile(PASTNode _node, CScope * _lc);
	bool gacIfElse(PASTNode _node, CScope * _lc);
	bool gacFunction(PASTNode _node, CScope * _lc);
	bool gacReturn(PASTNode _node, CScope * _lc);
	bool gacIf(PASTNode _node, CScope * _lc);
	bool gacSwitch(PASTNode _node, CScope * _lc);
	bool gacBody(PASTNode _node, CScope * _lc);


	bool generateAsmCode_Recursive(PASTNode _node);
	bool ast2asm(PASTNode _node, CScriptFunction *sf);
	bool ast2asm_Recursive(PASTNode _node, CScope * _lc);

	vector<tInfoStatementOp > 	*m_currentListStatements;
	CScope										*m_treescope;
	CScriptFunction 							*m_currentScriptFunction;
	vector <CScriptFunction *>  		 stk_scriptFunction;


/*
	//tInfoScopeVar * existRegisteredSymbolRecursive(const string & var_name);
	tInfoScopeVar * existRegisteredSymbol(const string & var_name);
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool defineSymbol(const string & var_name, CObject *obj);*/
	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
