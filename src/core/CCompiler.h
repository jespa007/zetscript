#pragma once


class CContext;
class CScopeInfo;

//class tInfoRegisteredFunctionSymbol;

/**
 * The compiler compiles function structs and generates its asm instruction. The compile must know
 * the class structure in order to link symbols of its scope.
  */
class CCompiler{

public:

	typedef struct{
		void *ptr;
		VALUE_INSTRUCTION_TYPE type;
	}tInfoConstantValue;

	static tDefOperator def_operator[MAX_OPERATORS];


	static CCompiler * getInstance();

	static void destroySingletons();


	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMMON COMPILE FUNCTIONS


	//bool compile(const string & s, tInfoRegisteredFunctionSymbol * pr);


	/**
	 * Clears all compilation information...
	 */
	void clear();

	/**
	 * Compiles main ast node with base object info to store instruction related with function information.
	 */

	bool compile(PASTNode _ast_main_node, tInfoRegisteredFunctionSymbol *sf);


private:
	static CCompiler *m_compiler;



	static map<string,tInfoConstantValue *> *constant_pool;


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
	static tInfoConstantValue * addConstant(const string & const_name, void *obj, VALUE_INSTRUCTION_TYPE type);

	//---------------------------------------------------------------------------------------------------------------------------------------
	// LINK UTILS

	bool isFunctionNode(PASTNode node);

	/*
	 * gets idx argument by its name.
	 */
	int getIdxArgument(const string & var);

	int  addLocalVarSymbol(const string & name,tASTNode *ast);
	bool localVarSymbolExists(const string & name,tASTNode *ast);
	int  getIdxLocalVarSymbol(const string & name,tASTNode *ast, bool print_msg=true);

	int  addLocalFunctionSymbol(const string & name,tASTNode *ast);
	bool functionSymbolExists(const string & name,tASTNode *ast);
	int  getIdxFunctionSymbol(const string & name,tASTNode *ast,SCOPE_TYPE & scope_type, bool print_msg=true);

	//---------------------------------------------------------------------------------------------------------------------------------------
	//
	// INSTRUCTION UTILS
	//

	/**
	 * Compile class struct main ast node with class base object info to store instruction related with function information.
	 */
	bool compile_class(PASTNode _ast_class_node, tScriptFunctionInfo *sf);


	bool parseExpression(const char *expression_str, int & m_line,tInfoRegisteredFunctionSymbol * sf, CScopeInfo *currentEvaluatingScope);
	/**
	 * Load value or symbol and insert asm operation at current statment.
	 */
	tInfoStatementOp  *newStatment();
	bool insertLoadValueInstruction(PASTNode _node, CScopeInfo * _lc);
	bool insertMovVarInstruction(PASTNode _node, int left_index, int right_index);


	/**
	 * Unconditional Jump instruction
	 */
	tInfoAsmOp * insert_JMP_Instruction(int jmp_statement =-1, int instruction_index = 0);

	/**
	 * Jump Not True (JNT) instruction
	 */
	tInfoAsmOp * insert_JNT_Instruction(int jmp_statement =-1, int instruction_index = 0);

	/**
	 * Jump if True (JT) instruction
	 */
	tInfoAsmOp * insert_JT_Instruction(int jmp_statement =-1, int instruction_index = 0);
	void insert_NOP_Instruction();

	tInfoAsmOp * insert_Push_CurrentInstruction();

	/**
	 * IndexAccess
	 */

	void insert_CreateArrayObject_Instruction(PASTNode _node);
	//void insert_LoadArrayObject_Instruction(CVariable *obj);
	void insert_ArrayObject_PushValueInstruction(PASTNode _node,int ref_vec_object_index, int index_instruction_to_push=-1);

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
	void insert_CallFunction_Instruction(PASTNode _node,int index_call,int index_object=-1);
	void insertRet(PASTNode _node,int index);

	/**
	 * Class instructions.
	 */

	bool insert_NewObject_Instruction(PASTNode _node, const string & class_name);
	bool insertObjectMemberAccessFrom(PASTNode _node, int ref_node_index);




	bool insertOperatorInstruction(tInfoPunctuator *  op, PASTNode _node, string & error_str, int left, int right=-1);


	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

	bool *getObjectResultCurrentStatmentAsBoolean();
	int getCurrentInstructionIndex();
	int getCurrentStatmentIndex();



	//CVariable::VAR_TYPE getTypeAsmResult(int index);

	void insertPushScopeInstruction(CScopeInfo * _goto_scope);
	void insertPopScopeInstruction();
	//---------------------------------------------------------------------------------------------------------------------------------------
	// COMPILE ASSEMBLE CODE (GAC)

	ASM_OPERATOR puntuator2instruction(tInfoPunctuator * op);
	ASM_PRE_POST_OPERATORS preoperator2instruction(PUNCTUATOR_TYPE op);

	int gacExpression_ArrayObject_Recursive(PASTNode _node, CScopeInfo *_lc);
	int gacExpression_ArrayObject(PASTNode op, CScopeInfo *_lc);
	int gacExpression_FunctionObject(PASTNode op, CScopeInfo *_lc);
	int gacExpression_FunctionAccess(PASTNode op, CScopeInfo *_lc);

	int gacExpression_ArrayAccess(PASTNode op, CScopeInfo *_lc);
	int  gacExpression_Recursive(PASTNode op, CScopeInfo * _lc, int & numreg);
	bool  gacExpression(PASTNode op, CScopeInfo * _lc,int index_instruction=-1);

	bool gacKeyword(PASTNode _node, CScopeInfo * _lc);

	/**
	 * Adds class info into factory
	 */
	bool doRegisterVariableSymbolsClass(const string & class_name, tInfoRegisteredClass *current_class);
	bool gacClass(PASTNode _node, CScopeInfo * _lc);

	bool gacNew(PASTNode _node, CScopeInfo * _lc);
	bool gacFor(PASTNode _node, CScopeInfo * _lc);
	bool gacVar(PASTNode _node, CScopeInfo * _lc);
	bool gacWhile(PASTNode _node, CScopeInfo * _lc);
	bool gacIfElse(PASTNode _node, CScopeInfo * _lc);
	bool gacFunction(PASTNode _node, CScopeInfo * _lc, tInfoRegisteredFunctionSymbol *irfs);
	bool gacReturn(PASTNode _node, CScopeInfo * _lc);
	bool gacIf(PASTNode _node, CScopeInfo * _lc);
	int gacInlineIf(PASTNode _node, CScopeInfo * _lc, int instruction);
	bool gacSwitch(PASTNode _node, CScopeInfo * _lc);
	bool gacBody(PASTNode _node, CScopeInfo * _lc);


	void pushFunction(PASTNode _node,tInfoRegisteredFunctionSymbol *sf);
	void popFunction();

	bool generateAsmCode_Recursive(PASTNode _node);

	bool ast2asm_Recursive(PASTNode _node, CScopeInfo * _lc);

	vector<tInfoStatementOp > 	*m_currentListStatements;
	CScopeInfo											*m_treescope;
	tInfoRegisteredFunctionSymbol						*m_currentFunctionInfo;
	vector <tInfoRegisteredFunctionSymbol *>  		 			stk_scriptFunction;


/*
	//tInfoScopeVar * existRegisteredSymbolRecursive(const string & var_name);
	tInfoScopeVar * existRegisteredSymbol(const string & var_name);
	CVariable *getRegisteredVariable(const string & v, bool print_msg=true);
	bool defineSymbol(const string & var_name, CVariable *obj);*/
	//---------------------------------------------------------------


	CCompiler();
	~CCompiler();
};
