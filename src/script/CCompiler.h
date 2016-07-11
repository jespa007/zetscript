#pragma once
class CContext;
class CScope;
class CScriptFunction;
class CCompiler{

public:



	enum LOAD_TYPE{

		LOAD_TYPE_NOT_DEFINED=0,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_ARGUMENT
	};


	enum ASM_OPERATOR{
		INVALID_OP=-1,
		NOP=0,
		MOV, // mov expression to var
		LOAD, // primitive value like number/string or boolean...
		EQU,  // ==
		LT,  // <
		LTE,  // <=
		NOT, // !
		GT,  // >
		GTE, // >=

		ADD, // +
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		DIV, // /
		MUL, // *
		MOD,  // %
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		// special internal ops...
		JMP,
		JNT, // goto if not true ... goes end to conditional.
		JT, // goto if true ... goes end to conditional.
		CALL, // calling function after all of args are processed...
		PUSH, // push arg
		CLR, // clear args
		VGET, // vector access after each index is processed...

		VPUSH, // Value push for vector
		VPOP, // Value pop for vector

		VEC, // Vector object
		RET, // ret instruction ..
		MAX_OPERATORS


	};

	enum ASM_PRE_POST_OPERATORS{
		UNKNOW_PRE_POST_OPERATOR=0,
		NEG, // -a
		PRE_INC, // ++
		POST_INC, // ++
		PRE_DEC, // --
		POST_DEC // --

	};

	typedef struct{
		const char *op_str;
		ASM_OPERATOR op_id;
		int n_ops;
	}tDefOperator;

	static tDefOperator def_operator[MAX_OPERATORS];


	class tInfoAsmOp{

	public:


	     //int type_op;
	     //tInfoObjectOperator *funOp;
	     //CObject *left_var_obj;
	    // void *result_obj; // can be float/bool/string or variable.

	    // string symbol_name;
		CVariable::VAR_TYPE variable_type;
	     PASTNode ast_node; // define ast node for give some information at run time
	    // int definedLine;
	     //string type_res;

	     //------------------

	     ASM_OPERATOR operator_type;
	     ASM_PRE_POST_OPERATORS pre_post_operator_type;
	     //void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_op1,index_op2; // left and right respectively

	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			//variable_type=VAR_TYPE::NOT_DEFINED;
			operator_type=ASM_OPERATOR::INVALID_OP;
			pre_post_operator_type =ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
			ast_node = NULL;
			//isconvertable=NULL;
			//left_var_obj=NULL;
		  //   type_op=0;
		   //  funOp=NULL;
			//result_obj=NULL; // oject type...

		   // type_res="none";
			index_op1=index_op2=-1;
		   // ptr_value=NULL;
		}

	};

	typedef struct{

	    vector<tInfoAsmOp *> asm_op;
	}tInfoStatementOp;

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

	//int generateAsmCode(PASTNode op, int & numreg, bool & error, CScope * _lc);


	ASM_OPERATOR getNumberOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVariable::VAR_TYPE & result_type);
	ASM_OPERATOR getIntegerOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVariable::VAR_TYPE & result_type);
	ASM_OPERATOR getBoleanOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVariable::VAR_TYPE & result_type);
	ASM_OPERATOR getStringOperatorId_TwoOps(PUNCTUATOR_TYPE op,CVariable::VAR_TYPE & result_type);

	ASM_PRE_POST_OPERATORS getNumberOperatorId_OneOp(PUNCTUATOR_TYPE op);
	ASM_OPERATOR getBoleanOperatorId_OneOp(PUNCTUATOR_TYPE op);
	ASM_OPERATOR getStringOperatorId_OneOp(PUNCTUATOR_TYPE op);

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
	static const char * getStrMovVar(CCompiler::tInfoAsmOp * iao);

	static void printGeneratedCode_Recursive(CScriptFunction *fs);
	static void printGeneratedCode(CScriptFunction *fs);

	// DEBUG TOOLS
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	ASM_OPERATOR puntuator2asmop(tInfoPunctuator * op);

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
	//tInfoRegisteredVar * existRegisteredSymbolRecursive(const string & var_name);
	tInfoRegisteredVar * existRegisteredSymbol(const string & var_name);
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool defineSymbol(const string & var_name, CObject *obj);*/
	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
