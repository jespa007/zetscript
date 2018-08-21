/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define CLEAR_COMPILE_INFORMATION 	CState::clearCurrentCompileInformation()
#define GET_SYMBOL_NAME(s)			(CCompiler::getSymbolNameFromSymbolRef(s))

#define LNK_SYM				"@lnk"
#define VAR_TYPE			"var"
#define FUN_TYPE			"fun"

namespace zetscript{


	class CScope;

	//class CScriptFunctionObject;

	/**
	 * The compiler compiles function structs and generates its asm instruction. The compile must know
	 * the class structure in order to link symbols of its scope.
	  */
	class  CCompiler{

	public:

		/*typedef struct{
			void *ptr;
			unsigned int type;
		}tInfoConstantValue;*/

		typedef tStackElement tInfoConstantValue;

		static tDefOperator def_operator[MAX_OPERATORS];

		static string 					makeSymbolRef(const string & symbol_name, int idxScope);
		static int 						getIdxScopeFromSymbolRef(const string & symbol_ref);
		static string 					getSymbolNameFromSymbolRef(const string & ref_symbol);


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

		bool compile();


	private:
		static CCompiler *m_compiler;
		static map<string,tInfoConstantValue *> *constant_pool;


		struct tInfoAsmOpCompiler{
			ASM_OPERATOR operator_type;
			char index_op1; // index/type/etc
			int  index_op2; // usually a pointer or index
			short idxAstNode;
			unsigned short var_type;
			unsigned int pre_post_op_type;
			unsigned int scope_type;
			unsigned int runtime_prop;

			tInfoAsmOpCompiler(){
				operator_type=ASM_OPERATOR::END_FUNCTION;
				index_op1=ZS_UNDEFINED_IDX;
				index_op2=ZS_UNDEFINED_IDX;
				idxAstNode=ZS_UNDEFINED_IDX;

				var_type=0;
				pre_post_op_type=0;
				scope_type = 0;
				runtime_prop = 0;
			}

		};

		typedef struct {
			vector<tInfoAsmOpCompiler *> asm_op;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tInfoFunctionCompile{
			vector<tInfoAsmOpCompiler *> 			asm_op;
			CScriptFunctionObject 				*  	function_info_object;

			tInfoFunctionCompile(CScriptFunctionObject	* _function_info_object){
				function_info_object = _function_info_object;
			}

			~tInfoFunctionCompile(){

				for(unsigned j = 0;  j< asm_op.size();j++){
					delete asm_op[j];
				}

			}
		};


		//---------------------------------------------------------------------------------------------------------------------------------------
		// CONSTANT TOOLS

		static tInfoConstantValue * getConstant(const string & const_name);
		static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned short properties);

		//---------------------------------------------------------------------------------------------------------------------------------------
		// LINK UTILS

		bool isFunctionNode(short idxAstNode);

		/*
		 * gets idx argument by its name.
		 */
		int getIdxArgument(const string & var);

		int  addLocalVarSymbol(const string & name,short idxAstNode);
		bool localVarSymbolExists(const string & name,short idxAstNode);
		int  getIdxLocalVarSymbol(const string & name,short idxAstNode, bool print_msg=true);

		CScriptFunctionObject *  addLocalFunctionSymbol(const string & name,short idxAstNode);
		bool functionSymbolExists(const string & name,short idxAstNode);
		int  getIdxFunctionObject(const string & name,short idxAstNode,unsigned int & scope_type, bool print_msg=true);

		//---------------------------------------------------------------------------------------------------------------------------------------
		//
		// INSTRUCTION UTILS
		//

		/**
		 * Compile class struct main ast node with class base object info to store instruction related with function information.
		 */

		bool compile_body(PASTNode _node, CScriptFunctionObject *sf);


		//bool parseExpression(const char *expression_str, int & m_line,CScriptFunctionObject * sf, CScope *currentEvaluatingScope);
		/**
		 * Load value or symbol and insert asm operation at current statment.
		 */
		//tInfoStatementOpCompiler  *newStatment();
		void insertStringConstantValueInstruction(short idxAstNode, const string & v);
		bool insertLoadValueInstruction(PASTNode _node, CScope * _lc, tInfoAsmOpCompiler **iao_result=NULL);
		bool insertMovVarInstruction(short idxAstNode, int left_index, int right_index);


		void insertNot(short idxAstNode);
		void insertNeg(short idxAstNode);
		/**
		 * Unconditional Jump instruction
		 */
		tInfoAsmOpCompiler * insert_JMP_Instruction(short idxAstNode,int instruction_index=ZS_UNDEFINED_IDX);

		/**
		 * Jump Not True (JNT) instruction
		 */
		tInfoAsmOpCompiler * insert_JNT_Instruction(short idxAstNode, int instruction_index=ZS_UNDEFINED_IDX);

		/**
		 * Jump if True (JT) instruction
		 */
		tInfoAsmOpCompiler * insert_JT_Instruction(short idxAstNode, int instruction_index=ZS_UNDEFINED_IDX);
		void insert_NOP_Instruction();

		tInfoAsmOpCompiler * insert_Save_CurrentInstruction();
		tInfoAsmOpCompiler * insert_Load_CurrentInstruction();

		/**
		 * IndexAccess
		 */

		void insert_CreateArrayObject_Instruction(short idxAstNode);
		void insert_ArrayObject_PushValueInstruction(short idxAstNode);

		void insert_ArrayAccess_Instruction(int instruction_vec_variable, short idxAstNode);


		/**
		 * Function instructions
		 */
		void insert_LoadFunctionObject_Instruction(short idxAstNode,CScriptVariable *obj);
		//void insert_StartArgumentStack_Instruction(short idxAstNode);

		/**
		 * insert call function
		 * index_object: index to take asm reference from. (-1 refers to main object)
		 * index_call: index where take function ref from.
		 */
		void insert_CallFunction_Instruction(short idxAstNode, int call_index);
		void insertRet(short idxAstNode);

		/**
		 * Class instructions.
		 */

		bool insert_NewObject_Instruction(short idxAstNode, const string & class_name);
		bool insert_DeleteObject_Instruction(short idxAstNode);
		bool insertObjectMemberAccessFrom(short idxAstNode, int ref_node_index);

		tInfoAsmOpCompiler * getLastInsertedInfoAsmOpCompiler();
		bool insertOperatorInstruction(PUNCTUATOR_TYPE op, short idxAstNode, string & error_str);


		string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

		bool *getObjectResultCurrentStatmentAsBoolean();
		int getCurrentInstructionIndex();

		bool insertPushScopeInstruction(short idxAstNode,int scope_idx, unsigned char properties=0);
		void insertPopScopeInstruction(short idxAstNode, unsigned char properties=0);

		void insert_DeclStruct_Instruction(short idxAstNode);
		void insert_PushAttribute_Instruction(short idxAstNode);
		//---------------------------------------------------------------------------------------------------------------------------------------
		// COMPILE ASSEMBLE CODE (GAC)

		ASM_OPERATOR puntuator2instruction(PUNCTUATOR_TYPE  op);
		unsigned int post_operator2instruction_property(PUNCTUATOR_TYPE op);

		bool gacExpression_FunctionOrArrayAccess(PASTNode _node, CScope *_lc);

		bool gacExpression_ArrayObject(PASTNode _node, CScope *_lc);
		bool gacExpression_FunctionObject(PASTNode _node, CScope *_lc);
		bool gacExpression_FunctionAccess(PASTNode _node, CScope *_lc);

		bool gacExpression_Struct(PASTNode _node, CScope *_lc);
		bool gacExpression_StructAttribute(PASTNode _node, CScope *_lc);

		bool  gacExpression_ArrayAccess(PASTNode _node, CScope *_lc);
		bool  gacExpression_Recursive(PASTNode _node, CScope * _lc);
		bool  gacExpression(PASTNode _node, CScope * _lc);

		bool gacKeyword(PASTNode _node, CScope * _lc);

		/**
		 * Adds class info into factory
		 */
		bool registerFunctionClassSymbol(short idx_var_node, const string & class_name,CScriptClass * current_class);
		bool registerVariableClassSymbol(short idx_node_fun, const string & class_name,CScriptClass * current_class);
		bool doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class);
		bool gacClass(PASTNode _node, CScope * _lc);

		bool gacNew(PASTNode _node, CScope * _lc);
		bool gacDelete(PASTNode _node, CScope * _lc);

		bool gacBreak(PASTNode _node, CScope * _lc);
		bool gacContinue(PASTNode _node, CScope * _lc);

		bool gacFor(PASTNode _node, CScope * _lc);
		bool gacForEach(PASTNode _node, CScope * _lc);
		bool gacVar(PASTNode _node, CScope * _lc);
		bool gacWhile(PASTNode _node, CScope * _lc);
		bool gacDoWhile(PASTNode _node, CScope * _lc);

		bool gacFunctionOrOperator(PASTNode _node, CScope * _lc, CScriptFunctionObject *irfs);
		bool gacReturn(PASTNode _node, CScope * _lc);
		bool gacIf(PASTNode _node, CScope * _lc);
		bool  gacInlineIf(PASTNode _node, CScope * _lc);
		bool gacSwitch(PASTNode _node, CScope * _lc);
		bool gacBody(PASTNode _node, CScope * _lc);


		void pushFunction(short idxAstNode,CScriptFunctionObject *sf);

		/// popFunction pop current function scope and optionally save its compiled statment with save_statment_op=true
		void popFunction(bool save_asm_op = true);

		bool generateAsmCode_Recursive(short idxAstNode);

		bool ast2asm_Recursive(short idxAstNode, CScope * _lc);

		CScope										*m_treescope;
		tInfoFunctionCompile						*m_currentFunctionInfo;
		vector <tInfoFunctionCompile *>  			stk_scriptFunction;

		// in order to find break continues within switch/for/while scopes
		vector<tBreakInstructionScope>				stk_breakInstructionsScope;
		tBreakInstructionScope						*ptr_breakInstructionsScope;

		vector<tContinueInstructionScope>			stk_continueInstructionsScope;
		tContinueInstructionScope					*ptr_continueInstructionsScope;

		void pushBreakInstructionScope();
		void popBreakInstructionScope();

		void pushContinueInstructionScope();
		void popContinueInstructionScope();


		//---------------------------------------------------------------

		CCompiler();
		~CCompiler();
	};
}
