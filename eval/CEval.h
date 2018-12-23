/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




#define DEFAULT_NO_FILENAME					"no_file"
#define register_C_Constant(s,v)			(zetscript::CEval::addConstant(s,(v)))

namespace zetscript{




	class  CEval{


	public:
		static string 				getSymbolNameFromSymbolRef(const string & ref_symbol);
		static string 				makeSymbolRef(const string & symbol_var, int idxScope);
		static tInfoConstantValue * addConstant(const string & const_name, int value);



		static const char * 		getOpCodeStr(OP_CODE  op);


		static bool evalString(const string & expression);
		static bool evalFile(const string & filename);


		static void	initStaticVars();

		static void destroyStaticVars();

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


		struct tInstructionCompiler{
			OP_CODE op_code;
			unsigned char index_op1; 	// index/type/etc
			intptr_t  index_op2; 		// usually a pointer or index
			unsigned short var_type;
			unsigned int pre_op_type;
			unsigned int post_op_type;
			unsigned int scope_type;
			unsigned int runtime_prop;

			tInstructionCompiler(){
				op_code=OP_CODE::END_FUNCTION;
				index_op1=ZS_UNDEFINED_IDX;
				index_op2=ZS_UNDEFINED_IDX;

				var_type=0;
				pre_op_type=0;
				post_op_type=0;
				scope_type = 0;
				runtime_prop = 0;
			}

		};

		struct tTokenNodeAccessor {

			ACCESSOR_TYPE accessor_type;
			string value;
			vector<tInstructionCompiler> instruction;

			tTokenNodeAccessor(){
				accessor_type=ACCESSOR_TYPE::UNKNOWN_ACCESSOR_TYPE;
			}
		};

		struct tTokenNode{


			TOKEN_TYPE	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
			PRE_OPERATOR_TYPE   pre_operator_type; // !,+,-
			OPERATOR_TYPE  		operator_type;


			IDENTITY_OPERATOR_TYPE  pre_inline_operator_identity_type; // ++i,--i
			IDENTITY_OPERATOR_TYPE  post_inline_operator_identity_type; // i++,i--

			string value;
			int line;
			vector<tInstructionCompiler> instruction; // byte code load literal/identifier(can be anonymous function), vector/struct.

			// access info like function call, vector access and variable memeber
			vector<tTokenNodeAccessor> accessor;

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
			vector<tInstructionCompiler> instruction;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tFunctionInfo{

			vector<tInstructionCompiler> 		instruction;
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


		typedef struct {
			KEYWORD_TYPE id;
			const char *str;
			char * (* eval_fun)(const char *, int &, CScope *, bool &);
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
			string filename;
			//unsigned char *data;
		} tInfoParsedSource;


		// singleton
		static map<string,tInfoConstantValue *> *constant_pool;


		static vector<tInfoParsedSource> * m_parsedSource;
		//static int getIdxParsedFilenameSource(const char *file);


		static const char * current_parsing_filename;
		static int current_idx_parsing_filename;

		static tOperatorInfo defined_operator[MAX_OPERATOR_TYPES];
		static tPreOperatorInfo defined_pre_operator[MAX_PRE_OPERATOR_TYPES];
		static tIdentityOperatorInfo defined_identity_operator[MAX_IDENTITY_OPERATOR_TYPES];
		static tSeparatorInfo defined_separator[MAX_SEPARATOR_TYPES];
		static tKeywordInfo defined_keyword[MAX_KEYWORD];
		static tOpCodeInfo  defined_opcode[MAX_OP_CODES];
		static tDirectiveInfo defined_directive[MAX_DIRECTIVES];


		static tFunctionInfo				*pCurrentFunctionInfo;
		static vector<tFunctionInfo *> 		*vFunctionInfo;
		static bool is_initialized;

		// CONSTANT TOOLS

		static tInfoConstantValue * getConstant(const string & const_name);
		static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned short properties);

		static int					getIdxScopeFromSymbolRef(const string & symbol_ref);
		static void pushFunction(CScriptFunction *sf);
		static void popFunction();

		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

		static char * evalLiteralNumber(const char *c, int & line, string & value, bool & error);


		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		static bool isFilenameAlreadyParsed(const string & filename);
		static const char * getParsedFilenameFromIdx(unsigned idx);


		//-----------------------------------------------
		//
		//static char * 	 isClassMember(const char *s,int & line, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...
		static bool 	isLiteral(const string & s);
		static bool   	isIdentifierNameExpressionOk(const string & symbol, int line);
		static char *  	getIdentifierToken(const char *s, string & symbol);
		static bool	  	isEndSymbolToken(char *s,char pre=0);
		static bool 	isAccessPunctuator(char *s);
		static char * 	evalSymbol(const char *s, int line, CEval::tTokenNode *  token);
		static bool     endExpression(const char * s);

		static SEPARATOR_TYPE  		evalSeparator(const char *s);
		static OPERATOR_TYPE   		evalOperator(const char *s);
		static PRE_OPERATOR_TYPE   	evalPreOperator(const char *s);
		static IDENTITY_OPERATOR_TYPE   evalPrePostIdentityOperator(const char *s);

		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS


		// AST core functions ...

		// Turn tokens into byte code throught semi AST algorithm ...
		static OP_CODE puntuator2instruction(OPERATOR_TYPE op);
		static bool  compileTokens(vector<tTokenNode> * vExpressionTokens,int idx_start,int idx_end,bool & error);

		// Turn expression in Tokens (with some byte code instructions inside)...
		static char * evalExpression(const char *s, int & line, CScope *scope_info, vector<tInstructionCompiler> 		*	instruction);


		// NEW EXPRESSION...
		static char * evalNewObject(const char *s,int & line,  CScope *scope_info, vector<tInstructionCompiler> 		*	instruction);

		// FUNCTION OBJECT...
		static char * evalFunctionObject(const char *s,int & line,  CScope *scope_info, vector<tInstructionCompiler> 	*	instruction);

		//STRUCT OBJECT...
		static char * evalStructObject(const char *s,int & line,  CScope *scope_info, vector<tInstructionCompiler> 		*	instruction);

		//VECTOR OBJECT...
		static char * evalVectorObject(const char *s,int & line,  CScope *scope_info, vector<tInstructionCompiler> 		*	instruction);

		// GENERIC VECTOR/FUNCTION ARGS
		static char * evalExpressionArgs(char c1, char c2,const char *s,int & line,  CScope *scope_info, vector<tInstructionCompiler> 		*	instruction);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		static char * isClassMember(const char *s,int & line, CScriptClass **sc,string & member_symbol, bool & error);
		static char * evalKeywordClass(const char *s,int & line,  CScope *scope_info, bool & error);

		// eval block { }
		static char * evalBlock(const char *s,int & line,  CScope *scope_info, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		static char * evalKeyword		(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordIf		(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordFor	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordWhile	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordDoWhile(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalBreak			(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalContinue		(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalDefaultCase	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordSwitch	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordVar	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordReturn	(const char *s, int & line, CScope *scope_info, bool & error);
		static char * evalKeywordDelete	(const char *s, int & line, CScope *scope_info, bool & error);

		static char * evalKeywordFunction(const char *s,int & line,  CScope *scope_info, bool & error);


		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------


		static char * eval_Recursive(const char *s, int & line, CScope *scope_info, bool & error);


	};
}
