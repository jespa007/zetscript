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
				operator_type=ASM_OPERATOR::END_STATMENT;
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

			~tInfoFunctionCompile(){
				for(unsigned i = 0; i < stament.size(); i++){
					for(unsigned j = 0;  j< stament[i].asm_op.size();j++){
						delete stament[i].asm_op[j];
					}
				}
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


		// DEBUG TOOLS
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
		bool compile_class(short idxAstNode, tFunctionInfo *sf);
		bool compile_body(short idxAstNode, CScriptFunctionObject *sf);


		//bool parseExpression(const char *expression_str, int & m_line,CScriptFunctionObject * sf, CScope *currentEvaluatingScope);
		/**
		 * Load value or symbol and insert asm operation at current statment.
		 */
		tInfoStatementOpCompiler  *newStatment();
		void insertStringConstantValueInstruction(short idxAstNode, const string & v);
		bool insertLoadValueInstruction(short idxAstNode, CScope * _lc, tInfoAsmOpCompiler **iao_result=NULL);
		bool insertMovVarInstruction(short idxAstNode, int left_index, int right_index);


		void insertNot(short idxAstNode);
		void insertNeg(short idxAstNode);
		/**
		 * Unconditional Jump instruction
		 */
		tInfoAsmOpCompiler * insert_JMP_Instruction(short idxAstNode,int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);

		/**
		 * Jump Not True (JNT) instruction
		 */
		tInfoAsmOpCompiler * insert_JNT_Instruction(short idxAstNode,int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);

		/**
		 * Jump if True (JT) instruction
		 */
		tInfoAsmOpCompiler * insert_JT_Instruction(short idxAstNode,int jmp_statement =ZS_UNDEFINED_IDX, int instruction_index = ZS_UNDEFINED_IDX);
		void insert_NOP_Instruction();

		tInfoAsmOpCompiler * insert_Save_CurrentInstruction();
		tInfoAsmOpCompiler * insert_Load_CurrentInstruction();

		/**
		 * IndexAccess
		 */

		void insert_CreateArrayObject_Instruction(short idxAstNode);
		void insert_ArrayObject_PushValueInstruction(short idxAstNode,int ref_vec_object_index, int index_instruction_to_push=ZS_UNDEFINED_IDX);

		void insert_ArrayAccess_Instruction(int vect_object, int index_instrucction, short idxAstNode);


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
		void insert_CallFunction_Instruction(short idxAstNode,int index_call,int index_object=ZS_UNDEFINED_IDX);
		void insertRet(short idxAstNode,int index);

		/**
		 * Class instructions.
		 */

		bool insert_NewObject_Instruction(short idxAstNode, const string & class_name);
		bool insert_DeleteObject_Instruction(short idxAstNode);
		bool insertObjectMemberAccessFrom(short idxAstNode, int ref_node_index);

		//bool insert_DeleteObject_Instruction(short idxAstNode, const string & class_name);


		tInfoAsmOpCompiler * getLastInsertedInfoAsmOpCompiler();
		bool insertOperatorInstruction(PUNCTUATOR_TYPE op, short idxAstNode, string & error_str, int left, int right=ZS_UNDEFINED_IDX);


		string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);

		bool *getObjectResultCurrentStatmentAsBoolean();
		int getCurrentInstructionIndex();
		int getCurrentStatmentIndex();


		bool insertPushScopeInstruction(short idxAstNode,int scope_idx, bool save_break=false);
		void insertPopScopeInstruction(short idxAstNode);


		void insert_DeclStruct_Instruction(short idxAstNode);
		void insert_PushAttribute_Instruction(short idxAstNode,int ref_object,int ref_result_expression);
		//---------------------------------------------------------------------------------------------------------------------------------------
		// COMPILE ASSEMBLE CODE (GAC)

		ASM_OPERATOR puntuator2instruction(PUNCTUATOR_TYPE  op);
		unsigned int post_operator2instruction_property(PUNCTUATOR_TYPE op);

		int gacExpression_FunctionOrArrayAccess(short idxAstNode, CScope *_lc);
		int gacExpression_ArrayObject_Recursive(short idxAstNode, CScope *_lc);
		bool gacExpression_ArrayObject(short idxAstNode, CScope *_lc);
		bool gacExpression_FunctionObject(short idxAstNode, CScope *_lc);
		int gacExpression_FunctionAccess(short idxAstNode, CScope *_lc);

		int gacExpression_Struct(short idxAstNode, CScope *_lc);
		int gacExpression_StructAttribute(short idxAstNode, CScope *_lc, int index_calling_object);


		int gacExpression_ArrayAccess(short idxAstNode, CScope *_lc);
		int  gacExpression_Recursive(short idxAstNode, CScope * _lc, int & numreg);
		bool  gacExpression(short idxAstNode, CScope * _lc,int index_instruction=ZS_UNDEFINED_IDX);

		bool gacKeyword(short idxAstNode, CScope * _lc);

		/**
		 * Adds class info into factory
		 */
		bool registerFunctionClassSymbol(short idx_var_node, const string & class_name,CScriptClass * current_class);
		bool registerVariableClassSymbol(short idx_node_fun, const string & class_name,CScriptClass * current_class);
		bool doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class);
		bool gacClass(short idxAstNode, CScope * _lc);

		int gacNew(short idxAstNode, CScope * _lc);
		int gacDelete(short idxAstNode, CScope * _lc);

		bool gacFor(short idxAstNode, CScope * _lc);
		bool gacVar(short idxAstNode, CScope * _lc);
		bool gacWhile(short idxAstNode, CScope * _lc);
		bool gacDoWhile(short idxAstNode, CScope * _lc);

		bool gacFunctionOrOperator(short idxAstNode, CScope * _lc, CScriptFunctionObject *irfs);
		bool gacReturn(short idxAstNode, CScope * _lc);
		bool gacIf(short idxAstNode, CScope * _lc);
		int  gacInlineIf(short idxAstNode, CScope * _lc, int & instruction);
		bool gacSwitch(short idxAstNode, CScope * _lc);
		bool gacBody(short idxAstNode, CScope * _lc);


		void pushFunction(short idxAstNode,CScriptFunctionObject *sf);

		/// popFunction pop current function scope and optionally save its compiled statment with save_statment_op=true
		void popFunction(bool save_statment_op = true);

		bool generateAsmCode_Recursive(short idxAstNode);

		bool ast2asm_Recursive(short idxAstNode, CScope * _lc);

		CScope										*m_treescope;
		tInfoFunctionCompile						*m_currentFunctionInfo;
		vector <tInfoFunctionCompile *>  			stk_scriptFunction;

		//---------------------------------------------------------------

		CCompiler();
		~CCompiler();
	};
}
