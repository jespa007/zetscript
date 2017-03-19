#pragma once


class CScope;

//class CScriptFunctionObject;

/**
 * The compiler compiles function structs and generates its asm instruction. The compile must know
 * the class structure in order to link symbols of its scope.
  */
class CCompiler{

public:

	typedef struct{
		void *ptr;
		unsigned int type;
	}tInfoConstantValue;

	static tDefOperator def_operator[MAX_OPERATORS];


	static CCompiler * getInstance();

	static void destroySingletons();


	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMMON COMPILE FUNCTIONS


	//bool compile(const string & s, CScriptFunctionObject * pr);


	/**
	 * Clears all compilation information...
	 */
	void clear();

	/**
	 * Compiles main ast node with base object info to store instruction related with function information.
	 */

	bool compile(PASTNode _ast_main_node, CScriptFunctionObject *sf);


private:
	static CCompiler *m_compiler;
	static map<string,tInfoConstantValue *> *constant_pool;

	struct tInfoAsmOpCompiler{
		unsigned char operator_type;
		char index_op1; // index/type/etc
		int  index_op2; // usually a pointer or index
		short idxAstNode;
		unsigned int var_type;
		unsigned int pre_post_op_type;
		unsigned int scope_type;
		unsigned int runtime_prop;

		tInfoAsmOpCompiler(){
			operator_type=0;
			index_op1=-1;
			index_op2=-1;
			idxAstNode=-1;

			var_type=0;
			pre_post_op_type=0;
			scope_type = 0;
			runtime_prop = 0;
		}

	};

	struct tInfoStatementOpCompiler{

		vector<tInfoAsmOpCompiler *> asm_op;
	};

	struct tInfoFunctionCompile{
		vector<tInfoStatementOpCompiler> 		stament;
		CScriptFunctionObject 				*  	function_info_object;

		tInfoFunctionCompile(CScriptFunctionObject	* _function_info_object){
			function_info_object = _function_info_object;
		}
	};


	//---------------------------------------------------------------------------------------------------------------------------------------
	// DEBUG TOOLS

	typedef struct{
		string src;
		int asm_statment_idx;
	}tDebugInformation;

	vector<tDebugInformation>	m_debugInfo;



	void insertDebugInformation(int _asm_stament_idx, const char *src_str);
	void printDebugInformation();
	static bool isThisScope(PASTNode _node);
	static bool isSuperScope(PASTNode _node);

	// DEBUG TOOLS
	//---------------------------------------------------------------------------------------------------------------------------------------
	// CONSTANT TOOLS

	static tInfoConstantValue * getConstant(const string & const_name);
	static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned int type);

	//---------------------------------------------------------------------------------------------------------------------------------------
	// LINK UTILS

	bool isFunctionNode(PASTNode node);

	/*
	 * gets idx argument by its name.
	 */
	int getIdxArgument(const string & var);

	int  addLocalVarSymbol(const string & name,CASTNode *ast);
	bool localVarSymbolExists(const string & name,CASTNode *ast);
	int  getIdxLocalVarSymbol(const string & name,CASTNode *ast, bool print_msg=true);

	CScriptFunctionObject *  addLocalFunctionSymbol(const string & name,CASTNode *ast);
	bool functionSymbolExists(const string & name,CASTNode *ast);
	int  getIdxFunctionObject(const string & name,CASTNode *param_ast,unsigned int & scope_type, bool print_msg=true);

	//---------------------------------------------------------------------------------------------------------------------------------------
	//
	// INSTRUCTION UTILS
	//




	/**
	 * Compile class struct main ast node with class base object info to store instruction related with function information.
	 */
	bool compile_class(PASTNode _ast_class_node, tFunctionInfo *sf);


	//bool parseExpression(const char *expression_str, int & m_line,CScriptFunctionObject * sf, CScope *currentEvaluatingScope);
	/**
	 * Load value or symbol and insert asm operation at current statment.
	 */
	tInfoStatementOpCompiler  *newStatment();
	void insertStringConstantValueInstruction(PASTNode _node, const string & v);
	bool insertLoadValueInstruction(PASTNode _node, CScope * _lc);
	bool insertMovVarInstruction(PASTNode _node, int left_index, int right_index);


	/**
	 * Unconditional Jump instruction
	 */
	tInfoAsmOpCompiler * insert_JMP_Instruction(int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);

	/**
	 * Jump Not True (JNT) instruction
	 */
	tInfoAsmOpCompiler * insert_JNT_Instruction(int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);

	/**
	 * Jump if True (JT) instruction
	 */
	tInfoAsmOpCompiler * insert_JT_Instruction(int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);
	void insert_NOP_Instruction();

	tInfoAsmOpCompiler * insert_Save_CurrentInstruction();
	tInfoAsmOpCompiler * insert_Load_CurrentInstruction();

	/**
	 * IndexAccess
	 */

	void insert_CreateArrayObject_Instruction(PASTNode _node);
	void insert_ArrayObject_PushValueInstruction(PASTNode _node,int ref_vec_object_index, int index_instruction_to_push=ZS_UNDEFINED_IDX);

	void insert_ArrayAccess_Instruction(int vect_object, int index_instrucction, PASTNode _ast);


	/**
	 * Function instructions
	 */
	void insert_LoadFunctionObject_Instruction(PASTNode _node,CScriptVariable *obj);
	void insert_ClearArgumentStack_Instruction(PASTNode _node);
	void insert_PushArgument_Instruction(PASTNode _node);
	void insert_ClearArgumentStack_And_PushFirstArgument_Instructions(PASTNode _node);

	/**
	 * insert call function
	 * index_object: index to take asm reference from. (-1 refers to main object)
	 * index_call: index where take function ref from.
	 */
	void insert_CallFunction_Instruction(PASTNode _node,int index_call,int index_object=ZS_UNDEFINED_IDX);
	void insertRet(PASTNode _node,int index);

	/**
	 * Class instructions.
	 */

	bool insert_NewObject_Instruction(PASTNode _node, const string & class_name);
	bool insertObjectMemberAccessFrom(PASTNode _node, int ref_node_index);




	bool insertOperatorInstruction(tPunctuatorInfo *  op, PASTNode _node, string & error_str, int left, int right=ZS_UNDEFINED_IDX);


	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	bool *getObjectResultCurrentStatmentAsBoolean();
	int getCurrentInstructionIndex();
	int getCurrentStatmentIndex();


	void insertPushScopeInstruction(CScope * _goto_scope);
	void insertPopScopeInstruction(PASTNode _node,int scope_idx);


	void insert_DeclStruct_Instruction(PASTNode _node);
	void insert_PushAttribute_Instruction(PASTNode _node,int ref_object,int ref_result_expression);
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	ASM_OPERATOR puntuator2instruction(tPunctuatorInfo * op);
	unsigned int preoperator2instruction_property(PUNCTUATOR_TYPE op);

	int gacExpression_FunctionOrArrayAccess(PASTNode _node_access, CScope *_lc);
	int gacExpression_ArrayObject_Recursive(PASTNode _node, CScope *_lc);
	int gacExpression_ArrayObject(PASTNode op, CScope *_lc);
	int gacExpression_FunctionObject(PASTNode op, CScope *_lc);
	int gacExpression_FunctionAccess(PASTNode op, CScope *_lc);

	int gacExpression_Struct(PASTNode _node, CScope *_lc);
	int gacExpression_StructAttribute(PASTNode _node, CScope *_lc, int index_calling_object);


	int gacExpression_ArrayAccess(PASTNode op, CScope *_lc);
	int  gacExpression_Recursive(PASTNode op, CScope * _lc, int & numreg);
	bool  gacExpression(PASTNode op, CScope * _lc,int index_instruction=ZS_UNDEFINED_IDX);

	bool gacKeyword(PASTNode _node, CScope * _lc);

	/**
	 * Adds class info into factory
	 */
	bool doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class);
	bool gacClass(PASTNode _node, CScope * _lc);

	int gacNew(PASTNode _node, CScope * _lc);
	bool gacFor(PASTNode _node, CScope * _lc);
	bool gacVar(PASTNode _node, CScope * _lc);
	bool gacWhile(PASTNode _node, CScope * _lc);
	bool gacIfElse(PASTNode _node, CScope * _lc);
	bool gacFunctionOrOperator(PASTNode _node, CScope * _lc, CScriptFunctionObject *irfs);
	bool gacReturn(PASTNode _node, CScope * _lc);
	bool gacIf(PASTNode _node, CScope * _lc);
	int gacInlineIf(PASTNode _node, CScope * _lc, int & instruction);
	bool gacSwitch(PASTNode _node, CScope * _lc);
	bool gacBody(PASTNode _node, CScope * _lc);


	void pushFunction(PASTNode _node,CScriptFunctionObject *sf);
	void popFunction();

	bool generateAsmCode_Recursive(PASTNode _node);

	bool ast2asm_Recursive(PASTNode _node, CScope * _lc);

	//vector<tInfoStatementOpCompiler > 			*m_currentListStatements;
	//vector<vector<tInfoStatementOpCompiler > *>	m_functionAsmStatements;
	CScope										*m_treescope;
	tInfoFunctionCompile						*m_currentFunctionInfo;
	vector <tInfoFunctionCompile *>  			stk_scriptFunction;

	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
