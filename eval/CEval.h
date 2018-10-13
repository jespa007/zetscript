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
			IDENTIFIER_TOKEN,
			LITERAL_TOKEN,
			OPERATOR_TOKEN,
			SEPARATOR_TOKEN,
			OBJECT_TOKEN,
			MAX_TOKEN_TYPES
		};

		enum OPERATOR_TYPE
			:unsigned char {

			UNKNOWN_OPERATOR = 0,
			SHIFT_LEFT_OPERATOR, 			// <<
			SHIFT_RIGHT_OPERATOR, 			// >>
			LOGIC_AND_OPERATOR, 			// &&
			LOGIC_OR_OPERATOR, 				// ||
			LOGIC_EQUAL_OPERATOR, 			// =
			LOGIC_NOT_EQUAL_OPERATOR, 		// !=
			LOGIC_GTE_OPERATOR, 			// >=
			LOGIC_LTE_OPERATOR, 			// <=
			INSTANCEOF_OPERATOR, 			// instanceof
			PRE_INC_OPERATOR, 				// ++
			PRE_DEC_OPERATOR, 				// --
			POST_INC_OPERATOR, 				// ++
			POST_DEC_OPERATOR, 				// --
			ADD_OPERATOR, 					// +
			SUB_OPERATOR, 					// -
			MUL_OPERATOR, 					// *
			DIV_OPERATOR, 					// /
			MOD_OPERATOR, 					// %
			BINARY_XOR_OPERATOR, 			// ^
			BINARY_AND_OPERATOR, 			// &
			BINARY_OR_OPERATOR, 			// |
			ASSIGN_OPERATOR, 				// =
			ADD_ASSIGN_OPERATOR, 			// +=
			SUB_ASSIGN_OPERATOR, 			// -=
			MUL_ASSIGN_OPERATOR, 			// *=
			DIV_ASSIGN_OPERATOR, 			// /=
			MOD_ASSIGN_OPERATOR, 			// %=
			BINARY_XOR_ASSIGN_OPERATOR,		// ^=
			BINARY_AND__ASSIGN_OPERATOR,	// &=
			BINARY_OR_ASSIGN_OPERATOR,  	// |=
			FIELD_OPERATOR, 				// .
			LOGIC_GT_OPERATOR, 				// >
			LOGIC_LT_OPERATOR, 				// <
			LOGIC_NOT_OPERATOR, 			// !
			TERNARY_IF_OPERATOR, 			// ?
			TERNARY_ELSE_OPERATOR, 			// :
			MAX_OPERATOR_TYPES
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

	struct tInfoAsmOpCompiler{
		ASM_OPERATOR operator_type;
		unsigned char index_op1; 	// index/type/etc
		intptr_t  index_op2; 		// usually a pointer or index
		unsigned short var_type;
		unsigned int pre_post_op_type;
		unsigned int scope_type;
		unsigned int runtime_prop;

		tInfoAsmOpCompiler(){
			operator_type=ASM_OPERATOR::END_FUNCTION;
			index_op1=ZS_UNDEFINED_IDX;
			index_op2=ZS_UNDEFINED_IDX;

			var_type=0;
			pre_post_op_type=0;
			scope_type = 0;
			runtime_prop = 0;
		}

	};

	typedef struct tTokenNode{

		TOKEN_TYPE	  token; // can be operator, literal, identifier, object. (separator are not take account)
		OPERATOR_TYPE punctuator_type;
		string value;
		vector<tInfoAsmOpCompiler> asm_op;

	}tExpressionNode;

	typedef struct tTokenNode *PASTExpressionNode;

		typedef struct {
			vector<tInfoAsmOpCompiler> asm_op;
		}tContinueInstructionScope,tBreakInstructionScope;

		struct tInfoFunctionCompile{

			vector<tInfoAsmOpCompiler> 				asm_op;
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
			SEPARATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} tSeparatorInfo;


		// singleton
		static map<string,tInfoConstantValue *> *constant_pool;

		static const char * current_parsing_filename;
		static int current_idx_parsing_filename;

		static tOperatorInfo defined_operator[MAX_OPERATOR_TYPES];
		static tSeparatorInfo defined_separator[MAX_SEPARATOR_TYPES];
		static tKeywordInfo defined_keyword[MAX_KEYWORD];
		static tDirectiveInfo defined_directive[MAX_DIRECTIVES];





		//---------------------------------------------------------------------------------------------------------------------------------------
		// CONSTANT TOOLS

		static tInfoConstantValue * getConstant(const string & const_name);
		static tInfoConstantValue * addConstant(const string & const_name, void *obj, unsigned short properties);




		static KEYWORD_TYPE 	isKeyword(const char *c);
		static DIRECTIVE_TYPE 	isDirective(const char *c);

		static bool evalLiteralNumber(const char *c, int & m_line, string & value, bool & error);


		//-----------------------------------------------
		//
		//static char * 	 isClassMember(const char *s,int & m_line, string & _class_name, string & var_name, bool & error, KEYWORD_TYPE ikw);

		// string generic utils...

		static bool   isIdentifierNameExpressionOk(const string & symbol, int m_line);
		static char *  getIdentifierToken(const char *s, const string & symbol);
		static bool	  isEndSymbolToken(char *s,char pre=0);
		static char * evalSymbol(const char *s, int m_line, tTokenNode * ast_node);

		static SEPARATOR_TYPE  evalSeparator(const char *s);
		static OPERATOR_TYPE   evalOperator(const char *s);

		static bool   isEndSeparator(char c);


		//------------------------------------------------------------------------------------------
		//
		// EXPRESSION FUNCTIONS



		// AST core functions ...
		static char * evalExpression(const char *s, int & m_line, CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);



		// NEW EXPRESSION...
		static char * evalExpressionNew(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);

		// FUNCTION OBJECT...
		static char * evalExpressionFunctionObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);

		//STRUCT OBJECT...
		static char * evalExpressionStructObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);

		//VECTOR OBJECT...
		static char * evalExpressionVectorObject(const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);

		// GENERIC VECTOR/FUNCTION ARGS
		static char * evalExpressionArgs(char c1, char c2,const char *s,int & m_line,  CScope *scope_info, vector<tInfoAsmOpCompiler> 		*	asm_op);


		// END EXPRESSION FUNCTION
		//
		//------------------------------------------------------------------------------------------
		// Class

		static char * isClassMember(const char *s,int & m_line, short & idxScopeClass, bool & error);
		static char * evalKeywordClass(const char *s,int & m_line,  CScope *scope_info, bool & error);

		// eval block { }
		static char * evalBlock(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);

		//------------------------------------------------------------------------------------------
		//
		// KEYWORDS FUNCTIONS
		//

		static char * evalKeyword		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordIf		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordFor	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordWhile	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDoWhile(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalBreak			(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalContinue		(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalDefaultCase	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordSwitch	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordVar	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordReturn	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);
		static char * evalKeywordDelete	(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


		static char * evalKeywordFunction(const char *s,int & m_line,  CScope *scope_info, tInfoFunctionCompile ** ifc);


		//
		// KEYWORDS FUNCTIONS
		//
		//------------------------------------------------------------------------------------------


		static char * eval_Recursive(const char *s, int & m_line, CScope *scope_info, tInfoFunctionCompile * ifc, bool & error);


	};
}
