#pragma once
class CContext;
class CScope;
class CScriptFunction;
class CCompiler{

public:

	enum VAR_TYPE{
		NOT_DEFINED=0,
		BOOL,
		INTEGER,
		NUMBER,
		STRING,
		OBJ,

		// function
		FUNCTION,

		// vector (to get int primitives)
		VECTOR1D_INT,
		VECTOR1D_BOOL,
		VECTOR1D_FLOAT,
		VECTOR1D_OBJ,
		VECTOR2D_INT,
		VECTOR2D_BOOL,
		VECTOR2D_FLOAT,
		VECTOR2D_OBJ,
		VECTOR3D_INT,
		VECTOR3D_BOOL,
		VECTOR3D_FLOAT,
		VECTOR3D_OBJ
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
		NEG, // !
		ADD, // +
		PRE_INC, // ++
		POST_INC, // ++
		PRE_DEC, // ++
		POST_DEC, // ++
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		DIV, // /
		MUL, // *
		MOD,  // %
		CAT,  // str+str
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		PUSH_SCOPE,
		POP_SCOPE,
		JMP,
		JNT, // goto if not true ... goes end to conditional.
		JT, // goto if true ... goes end to conditional.
		MAX_OPERATORS


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
	     void *result_obj; // can be float/bool/string or variable.
	     string result_str;
	     VAR_TYPE result_type;
	     //string type_res;

	     //------------------

	     ASM_OPERATOR operator_type;
	     void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_left,index_right;

	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			result_type=VAR_TYPE::NOT_DEFINED;
			operator_type=ASM_OPERATOR::INVALID_OP;
			//isconvertable=NULL;
			//left_var_obj=NULL;
		  //   type_op=0;
		   //  funOp=NULL;
			result_obj=NULL; // must be created before.

		   // type_res="none";
		    index_left=index_right=-1;
		    ptr_value=NULL;
		}

	};

	typedef struct{

	    vector<tInfoAsmOp *> asm_op;
	    string expression_str;
	    int m_line;
	}tInfoStatementOp;

	static CCompiler * getInstance();

	static void destroySingletons();

	//---------------------------------------------------------------------------------------------------------------------------------------
	// CONSTANT TOOLS

	static CObject *getConstant(const string & const_name);
	static bool addConstant(const string & const_name, CObject *obj);

	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMMON COMPILE FUNCTIONS

	bool compileExpression(const char *expression_str, int & m_line,CScriptFunction * sf, CScope *currentEvaluatingScope);
	bool compile(const string & s, CScriptFunction * pr);

	//int generateAsmCode(PASTNode op, int & numreg, bool & error, CScope * _lc);


	ASM_OPERATOR getNumberOperatorId_TwoOps(PUNCTUATOR_TYPE op,VAR_TYPE & result_type);
	ASM_OPERATOR getIntegerOperatorId_TwoOps(PUNCTUATOR_TYPE op,VAR_TYPE & result_type);
	ASM_OPERATOR getBoleanOperatorId_TwoOps(PUNCTUATOR_TYPE op,VAR_TYPE & result_type);
	ASM_OPERATOR getStringOperatorId_TwoOps(PUNCTUATOR_TYPE op,VAR_TYPE & result_type);

	ASM_OPERATOR getNumberOperatorId_OneOp(PUNCTUATOR_TYPE op);
	ASM_OPERATOR getBoleanOperatorId_OneOp(PUNCTUATOR_TYPE op);
	ASM_OPERATOR getStringOperatorId_OneOp(PUNCTUATOR_TYPE op);

	/**
	 * Load value or symbol and insert asm operation at current statment.
	 */
	tInfoStatementOp  *newStatment();
	bool insertLoadValueInstruction(const string & value, CScope * _lc, int var_at_line);
	bool insertMovVarInstruction(CObject *var, int right);

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

	bool insertOperatorInstruction(tInfoPunctuator *  op, string & error_str, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	bool *getObjectResultCurrentStatmentAsBoolean();
	int getCurrentInstructionIndex();
	int getCurrentStatmentIndex();

	VAR_TYPE getTypeAsmResult(int index);

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

	void insertDebugInformation(int _asm_stament_idx, const char *src_str);
	void printDebugInformation();
	static void printGeneratedCode(CScriptFunction *fs);

	// DEBUG TOOLS
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	int  gacExpression_Recursive(PASTNode op, int & numreg, bool & error, CScope * _lc);
	bool  gacExpression(PASTNode op, CScope * _lc);

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
