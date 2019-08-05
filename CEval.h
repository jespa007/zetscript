/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once




namespace zetscript{


	class CScriptClassFactory;

	class  CEval{


	public:

		CEval(CZetScript *zs);
		bool eval(const char *s, const char *_filename=NULL, int _line=1);
		 //bool evalString(const std::string & expression);
		 //bool evalFile(const std::string & filename);

		 ~CEval();

	private:

		enum {
			LEFT_NODE = 0, RIGHT_NODE
		};


		typedef enum
			:unsigned char {
			UNKNOWN_NODE = 0,
			PUNCTUATOR_NODE,
			EXPRESSION_NODE,
			KEYWORD_NODE,
			ARGS_DECL_NODE,
			ARGS_PASS_NODE,
			ARRAY_ACCESS_NODE,
			ARRAY_INDEX_NODE,
			ARRAY_OBJECT_NODE,
			FUNCTION_OBJECT_NODE,
			SYMBOL_NODE,
			BODY_BLOCK_NODE,
			GROUP_CASES_NODE,
			GROUP_IF_NODES, // if, else-if
			IF_NODE, // if-node (each one has a conditional + body )
			CONDITIONAL_NODE,
			PRE_FOR_NODE,
			POST_FOR_NODE,
			CLASS_VAR_COLLECTION_NODE,
			CLASS_FUNCTION_COLLECTION_NODE,
			BASE_CLASS_NODE,
			CALLING_OBJECT_NODE,
			ARRAY_REF_NODE,
			FUNCTION_REF_NODE,
			NEW_OBJECT_NODE,
			DELETE_OBJECT_NODE,
			ACCESS_OBJECT_MEMBER_NODE,
			STRUCT_NODE,
			MAX_NODE_TYPE
		}NODE_TYPE;

		typedef enum
			:unsigned char {
			UNKNOWN_KEYWORD = 0,
			IF_KEYWORD,
			ELSE_KEYWORD,
			FOR_KEYWORD,
			//FOREACH_KEYWORD,
			IN_KEYWORD,
			WHILE_KEYWORD,
			DO_WHILE_KEYWORD,
			VAR_KEYWORD,
			SWITCH_KEYWORD,
			CASE_KEYWORD,
			DEFAULT_KEYWORD,
			BREAK_KEYWORD,
			CONTINUE_KEYWORD,
			RETURN_KEYWORD,
			FUNCTION_KEYWORD,
			CLASS_KEYWORD,
			THIS_KEYWORD,
			//	SUPER_KEYWORD,
			NEW_KEYWORD,
			DELETE_KEYWORD,
			MAX_KEYWORD
		}KEYWORD_TYPE;

		typedef enum
			:unsigned char {
			UNKNOWN_DIRECTIVE = 0,
			INCLUDE_DIRECTIVE,
			MAX_DIRECTIVES
		}DIRECTIVE_TYPE;

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

		struct OpCodeInstructionEval:OpCodeInstruction{

			LinkSymbolFirstAccess 					 	linkSymbolFirstAccess;
			OpCodeInstructionSourceInfo 							instructionInfo;

			OpCodeInstructionEval(OP_CODE _op_code
						 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
						 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
						 ,unsigned short _properties=0
						 ):OpCodeInstruction(_op_code,_index_op1,_index_op2,_properties){
			}
		};


		struct tTokenNode{


			TOKEN_TYPE	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
			PRE_OPERATOR_TYPE   pre_operator_type; // !,+,-
			OPERATOR_TYPE  		operator_type;


			IDENTITY_OPERATOR_TYPE  pre_inline_operator_identity_type; // ++i,--i
			IDENTITY_OPERATOR_TYPE  post_inline_operator_identity_type; // i++,i--

			std::string value; // first access
			int line;
			std::vector<OpCodeInstructionEval> instruction; // byte code load literal/identifier(can be anonymous function), std::vector/struct.
			//std::map<short,CScriptFunction::OpCodeInstructionSourceInfo> instruction_info;
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
			std::vector<OpCodeInstructionEval> instruction;
		}ContinueInstructionScope,BreakInstructionScope;

		struct FunctionInfo{

			std::vector<OpCodeInstructionEval>	 		instruction;
			CScriptFunction 				*  	function_info_object;

			FunctionInfo(CScriptFunction	* _function_info_object){
				function_info_object = _function_info_object;
			}
		};



		typedef struct {
			DIRECTIVE_TYPE id;
			const char *str;
			//char * (*parse_fun)(const char *, int &, CScope *, PASTNode *);
		} DirectiveInfo;

		typedef std::function< char * (const char *, int &, CScope *, bool &)> KeywordFunction;

		typedef struct {
			KEYWORD_TYPE id;
			const char *str;
			KeywordFunction * eval_fun;
		} KeywordInfo;

		typedef struct {
			OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} OperatorInfo;

		typedef struct {
			PRE_OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} PreOperatorInfo;

		typedef struct {
			IDENTITY_OPERATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} IdentityOperatorInfo;


		typedef struct {
			SEPARATOR_TYPE id;
			const char *str;
			bool (*eval_fun)(const char *);
		} SeparatorInfo;




		 CZetScript * zs;
		 CScopeFactory *scope_factory;
		 CScriptFunctionFactory *script_function_factory;
		 CScriptClassFactory *script_class_factory;



		 std::map<std::string,std::string *>			 	 m_compiledSymbolName;


		 const char * 	current_parsing_file;
		 //int 			CURRENT_PARSING_FILE_IDX;

		 OperatorInfo defined_operator[MAX_OPERATOR_TYPES];
		 PreOperatorInfo defined_pre_operator[MAX_PRE_OPERATOR_TYPES];
		 IdentityOperatorInfo defined_identity_operator[MAX_IDENTITY_OPERATOR_TYPES];
		 SeparatorInfo defined_separator[MAX_SEPARATOR_TYPES];
		 KeywordInfo defined_keyword[MAX_KEYWORD];

		 DirectiveInfo defined_directive[MAX_DIRECTIVES];

		 FunctionInfo								*pCurrentFunctionInfo;
		 std::vector<FunctionInfo *> 				vFunctionInfo;
		 //std::vector<LinkSymbolFirstAccess>			vLinkSymbolFirstAccess;


		 void iniVars();
		 CEval();

		 //int					getIdxScopeFromSymbolRef(const std::string & symbol_ref);
		 void pushFunction(CScriptFunction *sf);
		 void popFunction();

		 KEYWORD_TYPE 	isKeyword(const char *c);
		 DIRECTIVE_TYPE 	isDirective(const char *c);

		 char * evalLiteralNumber(const char *c, int & line, std::string & value, bool & error);

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
		 bool  makeOperatorPrecedence(std::vector<tTokenNode> * vExpressionTokens,std::vector<OpCodeInstructionEval> *instruction,int idx_start,int idx_end,bool & error);

		/// Turn expression in Tokens (with some byte code instructions inside)...
		 char * evalExpression(const char *s, int & line, CScope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);


		/// NEW EXPRESSION...
		 char * evalNewObject(const char *s,int & line,  CScope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// FUNCTION OBJECT...
		 char * evalFunctionObject(const char *s,int & line,  CScope *scope_info, std::vector<OpCodeInstructionEval> 	*	instruction);

		/// STRUCT OBJECT...
		 char * evalStructObject(const char *s,int & line,  CScope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// VECTOR OBJECT...
		 char * evalVectorObject(const char *s,int & line,  CScope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);

		/// GENERIC VECTOR/FUNCTION ARGS
		 char * evalExpressionArgs(char c1, char c2,const char *s,int & line,  CScope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction);


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



		 char * eval_Recursive(const char *s, int & line, CScope *scope_info, bool & error);

	};
}
