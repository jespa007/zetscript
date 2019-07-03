/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




#define DEFAULT_NO_FILENAME					"no_file"
#define register_C_Constant(s,v)			(zs::CEval::getInstance()->addConstant(s,(v)))


namespace zs{




	class  CEval{


	public:

		//===================================================================================================
		//
		// PRINT ASM INFO
			static  void printGeneratedCode();

		// PRINT ASM INFO
		//---------------------------------------------------------------------------------------------------------------------------------------
//		 static std::string 				getSymbolNameFromSymbolRef(const std::string & ref_symbol);
//		 static std::string 				makeSymbolRef(const std::string & symbol_var, short idxScope, char n_params=NO_PARAMS_IS_VARIABLE);
		 tInfoConstantValue 	* 	addConstant(const std::string & const_name, int value);

		 static const char * 		getOpCodeStr(OP_CODE  op);


		 static bool evalString(const std::string & expression);
		 static bool evalFile(const std::string & filename);


		 static CEval * getInstance();

		 static void destroySingleton();

	private:

    	// In a expression we can find three types of tokens: an identifier, a separator, an operator, a literal or object token.
		enum TOKEN_TYPE:char{
			UNKNOWN_TOKEN=0,
			IDENTIFIER_TOKEN,  // a,b,c,d
			LITERAL_TOKEN, // true, false, 0.1, -12e-12
			OPERATOR_TOKEN, // +,-,%, ...
			SUB_EXPRESSION_TOKEN,
			VECTOR_OBJECT_TOKEN,
			STRUCT_OBJECT_TOKEN,
			FUNCTION_OBJECT_TOKEN,
			NEW_OBJECT_TOKEN, // =new ob(); | op (new obj()) op
			MAX_TOKEN_TYPES
		};


		enum ACCESSOR_TYPE:char{
			UNKNOWN_ACCESSOR_TYPE=0,
			CALL_ACCESSOR, // v(1)
			VECTOR_ACCESSOR, // v[0]
			MEMBER_ACCESSOR, // a.b.c
			MAX_ACCESSOR_TYPES
		};

		enum OPERATOR_TYPE:unsigned char {

			UNKNOWN_OPERATOR = 0,

			// ASSIGN
			ASSIGN_OPERATOR, 				// =
			ASSIGN_ADD_OPERATOR, 			// +=
			ASSIGN_SUB_OPERATOR, 			// -=
			ASSIGN_MUL_OPERATOR, 			// *=
			ASSIGN_DIV_OPERATOR, 			// /=
			ASSIGN_MOD_OPERATOR, 			// %=
			ASSIGN_BINARY_XOR_OPERATOR,		// ^=
			ASSIGN_BINARY_AND_OPERATOR,		// &=
			ASSIGN_BINARY_OR_OPERATOR,  	// |=
			ASSIGN_SHIFT_LEFT_OPERATOR, 	// <<=
			ASSIGN_SHIFT_RIGHT_OPERATOR, 	// >>=

			// LOGIC
			LOGIC_AND_OPERATOR, 			// &&
			LOGIC_OR_OPERATOR, 				// ||

			// RELATIONAL
			LOGIC_EQUAL_OPERATOR, 			// ==
			LOGIC_NOT_EQUAL_OPERATOR, 		// !=
			LOGIC_GTE_OPERATOR, 			// >=
			LOGIC_LTE_OPERATOR, 			// <=
			LOGIC_GT_OPERATOR, 				// >
			LOGIC_LT_OPERATOR, 				// <

			// ARITHMETIC
			ADD_OPERATOR, 					// +
			BINARY_OR_OPERATOR, 			// |
			BINARY_XOR_OPERATOR, 			// ^
			SUB_OPERATOR, 					// -
			MUL_OPERATOR, 					// *
			BINARY_AND_OPERATOR, 			// &
			DIV_OPERATOR, 					// /
			MOD_OPERATOR, 					// %
			SHIFT_LEFT_OPERATOR, 			// <<
			SHIFT_RIGHT_OPERATOR, 			// >>

			INSTANCEOF_OPERATOR, 			// instanceof

			TERNARY_IF_OPERATOR, 			// ?
			TERNARY_ELSE_OPERATOR, 			// :
			MAX_OPERATOR_TYPES
		};


		enum PRE_OPERATOR_TYPE:unsigned char {
			UNKNOWN_PRE_OPERATOR=0,
			PRE_LOGIC_NOT_OPERATOR, 		// !
			PRE_ADD_OPERATOR, 				// -
			PRE_SUB_OPERATOR	, 			// +
			MAX_PRE_OPERATOR_TYPES
		};



		enum IDENTITY_OPERATOR_TYPE:unsigned char {
			UNKNOWN_IDENTITY_OPERATOR=0,
			IDENTITY_INC_OPERATOR,	// ++
			IDENTITY_DEC_OPERATOR,	// --
			MAX_IDENTITY_OPERATOR_TYPES
		};



		enum SEPARATOR_TYPE:unsigned char {
			UNKNOWN_SEPARATOR=0,
			COMA_SEPARATOR,					// ,
			SEMICOLON_SEPARATOR,    		// ;
			OPEN_PARENTHESIS_SEPARATOR, 	// (
			CLOSE_PARENTHESIS_SEPARATOR, 	// )
			OPEN_SQUARE_BRAKET_SEPARATOR, 	// [
			CLOSE_SQUARE_BRAKET_SEPARATOR, 	// ]
			MAX_SEPARATOR_TYPES
		};


		struct tTokenNode{


			TOKEN_TYPE	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
			PRE_OPERATOR_TYPE   pre_operator_type; // !,+,-
			OPERATOR_TYPE  		operator_type;


			IDENTITY_OPERATOR_TYPE  pre_inline_operator_identity_type; // ++i,--i
			IDENTITY_OPERATOR_TYPE  post_inline_operator_identity_type; // i++,i--

			std::string value; // first access
			int line;
			std::vector<tInstructionEval> instruction; // byte code load literal/identifier(can be anonymous function), std::vector/struct.
			//std::map<short,CScriptFunction::tInstructionInfo> instruction_info;
			// access info like function call, std::vector access and variable memeber
			//std::vector<tTokenNodeAccessor> accessor;

			// AST info operator.
			tTokenNode *left;
			tTokenNode *right;

			tTokenNode(){
				line=-1;

				token_type=TOKEN_TYPE::UNKNOWN_TOKEN;
				operator_type=OPERATOR_TYPE::UNKNOWN_OPERATOR;
				left=right=NULL;
				pre_operator_type=PRE_OPERATOR_TYPE::UNKNOWN_PRE_OPERATOR;
				pre_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE::UNKNOWN_IDENTITY_OPERATOR;
				post_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE::UNKNOWN_IDENTITY_OPERATOR;

			}

		};


		typedef struct {
			std::vector<tInstructionEval> instruction;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tFunctionInfo{

			std::vector<tInstructionEval>	 		instruction;
			CScriptFunction 				*  	function_info_object;

			tFunctionInfo(CScriptFunction	* _function_info_object){
				function_info_object = _function_info_object;
			}
		};



		typedef struct {
			DIRECTIVE_TYPE id;
			const char *str;
			//char * (*parse_fun)(const char *, int &, CScope *, PASTNode *);
		} tDirectiveInfo;

		typedef std::function< char * (const char *, int &, CScope *, bool &)> tKeywordFunction;

		typedef struct {
			KEYWORD_TYPE id;
			const char *str;
			tKeywordFunction * eval_fun;
		} tKeywordInfo;

		typedef struct {
			OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tOperatorInfo;

		typedef struct {
			PRE_OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tPreOperatorInfo;

		typedef struct {
			IDENTITY_OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tIdentityOperatorInfo;


		typedef struct {
			SEPARATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tSeparatorInfo;

		typedef struct{
			OP_CODE op_code;
			const char *str;
		}tOpCodeInfo;

		typedef struct {
			std::string filename;
			//unsigned char *data;
		} tInfoParsedSource;

		static CEval *eval_singleton;


		// singleton
		 std::map<std::string,tInfoConstantValue *> 	 m_contantPool;
		 std::map<std::string,std::string *>			 	 m_compiledSymbolName;
		 std::vector<tInfoParsedSource> 			 m_parsedSource;

		 const char * 	CURRENT_PARSING_FILE_STR;
		 int 			CURRENT_PARSING_FILE_IDX;

		 tOperatorInfo defined_operator[MAX_OPERATOR_TYPES];
		 tPreOperatorInfo defined_pre_operator[MAX_PRE_OPERATOR_TYPES];
		 tIdentityOperatorInfo defined_identity_operator[MAX_IDENTITY_OPERATOR_TYPES];
		 tSeparatorInfo defined_separator[MAX_SEPARATOR_TYPES];
		 tKeywordInfo defined_keyword[MAX_KEYWORD];
		 tOpCodeInfo  defined_opcode[MAX_OP_CODES];
		 tDirectiveInfo defined_directive[MAX_DIRECTIVES];


		 tFunctionInfo							*pCurrentFunctionInfo;
		 std::vector<tFunctionInfo *> 				vFunctionInfo;
		 //std::vector<tLinkSymbolFirstAccess>			vLinkSymbolFirstAccess;


		 void iniVars();
		 CEval();



		 // CONSTANT TOOLS

		 tInfoConstantValue * getConstant(const std::string & const_name);
		 tInfoConstantValue * addConstant(const std::string & const_name, void *obj, unsigned short properties);

		 //int					getIdxScopeFromSymbolRef(const std::string & symbol_ref);
		 void pushFunction(CScriptFunction *sf);
		 void popFunction();

		 KEYWORD_TYPE 	isKeyword(const char *c);
		 DIRECTIVE_TYPE 	isDirective(const char *c);

		 char * evalLiteralNumber(const char *c, int & line, std::string & value, bool & error);


		//===================================================================================================
		//
		// PRINT ASM INFO
		static std::string getStrMovVar(tInstruction * iao);
		static std::string getStrTypeLoadValue(CScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction);
		static  void printGeneratedCode(CScriptFunction *sfo);
		//===================================================================================================



		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		 bool isFilenameAlreadyParsed(const std::string & filename);
		 const char * getParsedFilenameFromIdx(unsigned idx);

		//-----------------------------------------------
		// LINK
		 //void linkSymbols();
		 std::string * getCompiledSymbol(const std::string & s);


		//-----------------------------------------------
		//
		// char * 	 isClassMember(const char *s,int & line, std::string & _class_name, std::string & var_name, bool & error, KEYWORD_TYPE ikw);

		// std::string generic utils...
		 bool 	isLiteral(const std::string & s);
		 bool   	isIdentifierNameExpressionOk(const std::string & symbol, int line);
		 char *  	getIdentifierToken(const char *s, std::string & symbol);
		 bool	  	isEndSymbolToken(char *s,char pre=0);
		 bool 	isAccessPunctuator(char *s);
		 char * 	evalSymbol(const char *s, int line, CEval::tTokenNode *  token);
		 bool     endExpression(const char * s);

		 SEPARATOR_TYPE  		evalSeparator(const char *s);
		 OPERATOR_TYPE   		evalOperator(const char *s);
		 PRE_OPERATOR_TYPE   	evalPreOperator(const char *s);
		 IDENTITY_OPERATOR_TYPE   evalPrePostIdentityOperator(const char *s);

		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS

		 void linkAccessorSymbol(tTokenNode & node);

		// AST core functions ...

		/// Turn tokens into byte code throught semi AST algorithm ...
		 OP_CODE puntuator2instruction(OPERATOR_TYPE op);

		/// Make operator precedence from the AST of tokens built during evalExpression.
		 bool  makeOperatorPrecedence(std::vector<tTokenNode> * vExpressionTokens,std::vector<tInstructionEval> *instruction,int idx_start,int idx_end,bool & error);

		/// Turn expression in Tokens (with some byte code instructions inside)...
		 char * evalExpression(const char *s, int & line, CScope *scope_info, std::vector<tInstructionEval> 		*	instruction);


		/// NEW EXPRESSION...
		 char * evalNewObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction);

		/// FUNCTION OBJECT...
		 char * evalFunctionObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 	*	instruction);

		/// STRUCT OBJECT...
		 char * evalStructObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction);

		/// VECTOR OBJECT...
		 char * evalVectorObject(const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction);

		/// GENERIC VECTOR/FUNCTION ARGS
		 char * evalExpressionArgs(char c1, char c2,const char *s,int & line,  CScope *scope_info, std::vector<tInstructionEval> 		*	instruction);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		 char * isClassMember(const char *s,int & line, CScriptClass **sc,std::string & member_symbol, bool & error);
		 char * evalKeywordClass(const char *s,int & line,  CScope *scope_info, bool & error);

		// eval block { }
		 char * evalBlock(const char *s,int & line,  CScope *scope_info, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		 char * evalKeyword		(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordIf		(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordFor	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordWhile	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordDoWhile(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalBreak			(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalContinue		(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalDefaultCase	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordSwitch	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordVar	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordReturn	(const char *s, int & line, CScope *scope_info, bool & error);
		 char * evalKeywordDelete	(const char *s, int & line, CScope *scope_info, bool & error);

		 char * evalKeywordFunction(const char *s,int & line,  CScope *scope_info, bool & error);


		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------

		 bool evalStringInternal(const std::string & expression);
		 bool evalFileInternal(const std::string & filename);


		 char * eval(const char *s, bool & error);
		 char * eval_Recursive(const char *s, int & line, CScope *scope_info, bool & error);

		 ~CEval();


	};
}
