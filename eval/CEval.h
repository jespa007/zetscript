/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




#define DEFAULT_NO_FILENAME					"no_file"


namespace zetscript{

	class  CEval{

	public:


		bool eval(const char *s);

		static void	init();

		static void destroySingletons();

	private:

    	// In a expression we can find three types of tokens: an identifier, a separator, an operator, a literal or object token.
		enum TOKEN_TYPE:char{
			UNKNOWN_TOKEN=0,
			IDENTIFIER_TOKEN,  // a,b,c,d
			LITERAL_TOKEN, // true, false, 0.1, -12e-12
			OPERATOR_TOKEN, // +,-,%, ...
			SEPARATOR_TOKEN, // ',',')',...
			OBJECT_TOKEN, // [0,1,2], function(i){ return i+1;}
			MAX_TOKEN_TYPES
		};

		enum OPERATION_TYPE:char{
			UNKNOWN_OPERATION=0,
			CALL_FUNCTION_OPERATION, // v(1)
			VECTOR_ACCESS_OPERATION, // v[0]
			NEW_OBJECT_OPERATION, // new ob()

		};

		enum OPERATOR_TYPE
			:unsigned char {

			UNKNOWN_OPERATOR = 0,
			// assign group...
			ASSIGN_OPERATOR, 				// =
			ADD_ASSIGN_OPERATOR, 			// +=
			SUB_ASSIGN_OPERATOR, 			// -=
			MUL_ASSIGN_OPERATOR, 			// *=
			DIV_ASSIGN_OPERATOR, 			// /=
			MOD_ASSIGN_OPERATOR, 			// %=
			BINARY_XOR_ASSIGN_OPERATOR,		// ^=
			BINARY_AND__ASSIGN_OPERATOR,	// &=
			BINARY_OR_ASSIGN_OPERATOR,  	// |=

			// logic
			LOGIC_AND_OPERATOR, 			// &&
			LOGIC_OR_OPERATOR, 				// ||

			LOGIC_EQUAL_OPERATOR, 			// ==
			LOGIC_NOT_EQUAL_OPERATOR, 		// !=
			LOGIC_GTE_OPERATOR, 			// >=
			LOGIC_LTE_OPERATOR, 			// <=


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

			FIELD_OPERATOR, 				// .
			LOGIC_GT_OPERATOR, 				// >
			LOGIC_LT_OPERATOR, 				// <

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

		enum PRE_POST_OPERATOR_IDENTITY_TYPE:unsigned char {
			UNKNOWN_PRE_POST_IDENTITY_OPERATOR=0,
			PRE_POST_IDENTITY_INC_OPERATOR,	// ++
			PRE_POST_IDENTITY_DEC_OPERATOR,	// --
			MAX_PRE_POST_IDENTITY_OPERATOR_TYPES
		};



	enum SEPARATOR_TYPE
	:unsigned char {
			UNKNOWN_SEPARATOR=0,
			COMA_SEPARATOR,					// ,
			SEMICOLON_SEPARATOR,    		// ;
			OPEN_PARENTHESIS_SEPARATOR, 	// (
			CLOSE_PARENTHESIS_SEPARATOR, 	// )
			OPEN_BRAKET_SEPARATOR, 			// {
			CLOSE_BRAKET_SEPARATOR, 		// }
			OPEN_SQUARE_BRAKET_SEPARATOR, 	// [
			CLOSE_SQUARE_BRAKET_SEPARATOR, 	// ]
			MAX_SEPARATOR_TYPES
		};

	struct tInfoByteCodeCompiler{
		ASM_OPERATOR operator_type;
		unsigned char index_op1; 	// index/type/etc
		intptr_t  index_op2; 		// usually a pointer or index
		unsigned short var_type;
		unsigned int pre_post_op_type;
		unsigned int scope_type;
		unsigned int runtime_prop;

		tInfoByteCodeCompiler(){
			operator_type=ASM_OPERATOR::END_FUNCTION;
			index_op1=ZS_UNDEFINED_IDX;
			index_op2=ZS_UNDEFINED_IDX;

			var_type=0;
			pre_post_op_type=0;
			scope_type = 0;
			runtime_prop = 0;
		}

	};

	struct tTokenNode{


		TOKEN_TYPE	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
		PRE_OPERATOR_TYPE   pre_operator_type; // !,+,-
		OPERATOR_TYPE  		operator_type;

		PRE_POST_OPERATOR_IDENTITY_TYPE  pre_operator_identity_type; // ++i,--i
		PRE_POST_OPERATOR_IDENTITY_TYPE  post_operator_identity_type; // i++,i--

		string value;
		vector<tInfoByteCodeCompiler> byte_code; // byte code load literal/identifier(can be anonymous function), vector/struct.
		tTokenNode *left;
		tTokenNode *right;

		tTokenNode(){
			token_type=TOKEN_TYPE::UNKNOWN_TOKEN;
			operator_type=OPERATOR_TYPE::UNKNOWN_OPERATOR;
			left=right=NULL;
		}

	};



		typedef struct {
			vector<tInfoByteCodeCompiler> byte_code;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tInfoFunctionCompile{

			vector<tInfoByteCodeCompiler> 				byte_code;
			CScriptFunctionObject 				*  	function_info_object;

			tInfoFunctionCompile(CScriptFunctionObject	* _function_info_object){
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
			char * (* eval_fun)(const char *, int &, CScope *,tInfoFunctionCompile *, bool &);
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
			PRE_POST_OPERATOR_IDENTITY_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tPrePostIdentityOperatorInfo;


		typedef struct {
			SEPARATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tSeparatorInfo;


		// singleton
		static map<string,tInfoConstantValue *> *constant_pool;

		static const char * current_parsing_filename;
		static int current_idx_parsing_filename;

		static tOperatorInfo defined_operator[MAX_OPERATOR_TYPES];
		static tPreOperatorInfo defined_pre_operator[MAX_PRE_OPERATOR_TYPES];
		static tPrePostIdentityOperatorInfo defined_pre_post_identity_operator[MAX_PRE_POST_IDENTITY_OPERATOR_TYPES];
		static tSeparatorInfo defined_separator[MAX_SEPARATOR_TYPES];
		static tKeywordInfo defined_keyword[MAX_KEYWORD];
		static tDirectiveInfo defined_directive[MAX_DIRECTIVES];





		//---------------------------------------------------------------------------------------------------------------------------------------
		// CONSTANT TOOLS

		static tInfoConstantValue * getConstant(const string & const_name);
		static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned short properties);


		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

		static bool evalLiteralNumber(const char *c, int & line, tTokenNode * token_node, bool & error);


		//-----------------------------------------------
		//
		//static char * 	 isClassMember(const char *s,int & line, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...

		static bool   isIdentifierNameExpressionOk(const string & symbol, int line);
		static char *  getIdentifierToken(const char *s, const string & symbol);
		static bool	  isEndSymbolToken(char *s,char pre=0);
		static char * evalSymbol(const char *s, int line, tTokenNode * token_node);

		static SEPARATOR_TYPE  		evalSeparator(const char *s);
		static OPERATOR_TYPE   		evalOperator(const char *s);
		static PRE_OPERATOR_TYPE   	evalPreOperator(const char *s);
		static PRE_POST_OPERATOR_IDENTITY_TYPE   evalPrePostIdentityOperator(const char *s);

		static bool   isEndSeparator(char c);


		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS



		// AST core functions ...
		static bool CEval::generateByteCodeExpression(tTokenNode *_node);
		static bool  buildAstExpression(tTokenNode **node,vector<tTokenNode> * vExpressionTokens,int idx_start,int idx_end,bool & error);
		static char * evalExpression(const char *s, int & line, CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);



		// NEW EXPRESSION...
		static char * evalExpressionNew(const char *s,int & line,  CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);

		// FUNCTION OBJECT...
		static char * evalExpressionFunctionObject(const char *s,int & line,  CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);

		//STRUCT OBJECT...
		static char * evalExpressionStructObject(const char *s,int & line,  CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);

		//VECTOR OBJECT...
		static char * evalExpressionVectorObject(const char *s,int & line,  CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);

		// GENERIC VECTOR/FUNCTION ARGS
		static char * evalExpressionArgs(char c1, char c2,const char *s,int & line,  CScope *scope_info, vector<tInfoByteCodeCompiler> 		*	byte_code);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		static char * isClassMember(const char *s,int & line, short & idxScopeClass, bool & error);
		static char * evalKeywordClass(const char *s,int & line,  CScope *scope_info, bool & error);

		// eval block { }
		static char * evalBlock(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		static char * evalKeyword		(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordIf		(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordFor	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordWhile	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDoWhile(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalBreak			(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalContinue		(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalDefaultCase	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordSwitch	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordVar	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordReturn	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDelete	(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


		static char * evalKeywordFunction(const char *s,int & line,  CScope *scope_info, tInfoFunctionCompile ** ifc);


		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------


		static char * eval_Recursive(const char *s, int & line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


	};
}
