/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

//#include "ZetScript.h"

/*

Constants (C)
-------------

C <= Integer,Number,Boolean,String


Pre operators (Pe)
-------------

Pe <= [ --, ++ ]

Post operators (Po)
-------------

Po <= [ --, ++ ]


Symbol (S)
-------------

S <=  [A-Za-Z ][A-Za-z0-9 ]*  --> token variable
S <=  [0-9]*.[0-9]* | [e|E][-|+][0-9]*  --> token integer or number
S <=  0x[0-9a-fA-F]* --> token hexadecimal
S <=  [0-1]*b --> token binary
S <= "[all characters]" --> token std::string


Punctuator (P)
-------------

Arithmetic

P <= [  /, *, %, -, + ]

Bit

P <= [  &, |, ^, <<, >> ]

Logical

P <= [  ==, >=, <=, >, <, !=, &&, || ]

Assignments

P <= [  =, +=, -=, /=, %=, *=, <<=, >>=, ^=, |=, &=]

Object

P <= [  . ]

Ternary

P <= [  ?,: ]


Anonymous Objects (O)
-------------

O <= [E1,E2,E3,...,En] 
O <= function(a1,a2,...,an){ B }
O <= {S1:E1,S2:E2,...,Sn:Em}

Expression (E)
-------------

E <= (E) // parentesis E
E <= E P E // punctuator
E <= !E // negate result of expression
E <= -E // Prerest (negate result of expression)
E <= +E // Presum (it does nothing)
E <= [Pe]S[Po] // variable with/out pre/post operator.
E <= (S|O)[E1][E2]...[En] // array access
E <= (S|O)(E1,E2,..,En) // function call
E(t)<= E; | E,


KeywordType (K)
-------------

- var S=E;
- for(var S1=Ei1,S2=Ei2,...,Sn=Eim;Ec;Ep1,Ep2,Ep3,...,Epk){ B } //
- for(var S1 in S2) { B } //
- while(Ec) { B }
- do{ B } while(Ec);
- switch(E){ // switch should have constants on case
	default:
	case C0:
		� break;
	case C1:
		� break;
		� ..
	case CN:
		� break;
	}

-if(Ec){ B1 } else if (Ec2) { B2 } else { B3 }


Body (B)
----------

Starts with '{' and ends with '}'

B <- [E;|K]* // A set of expressions ended with ; or KeywordType

*/


#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

#define print_eval_cr ZS_PRINT_DEBUG
#else
#define print_eval_cr(s,...)
#endif


namespace zetscript{

	namespace eval{

		typedef enum
				:unsigned char {
				KEYWORD_TYPE_UNKNOWN = 0,
				KEYWORD_TYPE_IF,
				KEYWORD_TYPE_ELSE,
				KEYWORD_TYPE_FOR,
				//FOREACH_KEYWORD,
				KEYWORD_TYPE_IN,
				KEYWORD_TYPE_WHILE,
				KEYWORD_TYPE_DO_WHILE,
				KEYWORD_TYPE_VAR,
				KEYWORD_TYPE_SWITCH,
				KEYWORD_TYPE_CASE,
				KEYWORD_TYPE_DEFAULT,
				KEYWORD_TYPE_BREAK,
				KEYWORD_TYPE_CONTINUE,
				KEYWORD_TYPE_RETURN,
				KEYWORD_TYPE_FUNCTION,
				KEYWORD_TYPE_CLASS,
				KEYWORD_TYPE_THIS,
				//	SUPER_KEYWORD,
				KEYWORD_TYPE_NEW,
				KEYWORD_TYPE_DELETE,
				KEYWORD_TYPE_MAX
			}KeywordType;

			typedef enum
				:unsigned char {
				DIRECTIVE_TYPE_UNKNOWN = 0,
				DIRECTIVE_TYPE_INCLUDE,
				DIRECTIVE_TYPE_MAX
			}DirectiveType;

			// In a expression we can find three types of tokens: an identifier, a separator, an operator, a literal or object token.
			typedef enum :char{
				TOKEN_TYPE_UNKNOWN=0,
				TOKEN_TYPE_IDENTIFIER,  // a,b,c,d
				TOKEN_TYPE_LITERAL, // true, false, 0.1, -12e-12
				TOKEN_TYPE_OPERATOR, // +,-,%, ...
				TOKEN_TYPE_SUBEXPRESSION,
				TOKEN_TYPE_VECTOR,
				TOKEN_TYPE_DICTIONARY,
				TOKEN_TYPE_FUNCTION_OBJECT,
				TOKEN_TYPE_NEW_OBJECT, // =new ob(); | op (new obj()) op
				TOKEN_TYPE_MAX
			}TokenType;


			typedef enum:char{
				ACCESSOR_TYPE_UNKNOWN=0,
				ACCESSOR_TYPE_CALL, // v(1)
				ACCESSOR_TYPE_VECTOR, // v[0]
				ACCESSOR_TYPE_MEMBER, // a.b.c
				ACCESSOR_TYPE_MAX
			}AccessorType;

			typedef enum :unsigned char {

				OPERATOR_TYPE_UNKNOWN = 0,

				// ASSIGN
				OPERATOR_TYPE_ASSIGN, 				// =
				OPERATOR_TYPE_ASSIGN_ADD, 			// +=
				OPERATOR_TYPE_ASSIGN_SUB, 			// -=
				OPERATOR_TYPE_ASSIGN_MUL, 			// *=
				OPERATOR_TYPE_ASSIGN_DIV, 			// /=
				OPERATOR_TYPE_ASSIGN_MOD, 			// %=
				OPERATOR_TYPE_ASSIGN_BINARY_XOR,	// ^=
				OPERATOR_TYPE_ASSIGN_BINARY_AND,	// &=
				OPERATOR_TYPE_ASSIGN_BINARY_OR,  	// |=
				OPERATOR_TYPE_ASSIGN_SHIFT_LEFT, 	// <<=
				OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT, 	// >>=

				// LOGIC
				OPERATOR_TYPE_LOGIC_AND, 			// &&
				OPERATOR_TYPE_LOGIC_OR, 			// ||

				// RELATIONAL
				OPERATOR_TYPE_LOGIC_EQUAL, 			// ==
				OPERATOR_TYPE_LOGIC_NOT_EQUAL, 		// !=
				OPERATOR_TYPE_LOGIC_GTE, 			// >=
				OPERATOR_TYPE_LOGIC_LTE, 			// <=
				OPERATOR_TYPE_LOGIC_GT, 			// >
				OPERATOR_TYPE_LOGIC_LT, 			// <

				// ARITHMETIC
				OPERATOR_TYPE_ADD, 					// +
				OPERATOR_TYPE_OR, 			// |
				OPERATOR_TYPE_XOR, 			// ^
				OPERATOR_TYPE_SUB, 					// -
				OPERATOR_TYPE_MUL, 					// *
				OPERATOR_TYPE_BINARY_AND, 			// &
				OPERATOR_TYPE_DIV, 					// /
				OPERATOR_TYPE_MOD, 					// %
				OPERATOR_TYPE_SHIFT_LEFT, 			// <<
				OPERATOR_TYPE_SHIFT_RIGHT, 			// >>

				OPERATOR_TYPE_INSTANCEOF, 			// instanceof

				OPERATOR_TYPE_TERNARY_IF, 			// ?
				OPERATOR_TYPE_TERNARY_ELSE, 		// :
				OPERATOR_TYPE_MAX
			}OperatorType;


			typedef enum :unsigned char {
				PRE_OPERATOR_TYPE_UNKNOWN=0,
				PRE_OPERATOR_TYPE_NOT, 		// !
				PRE_OPERATOR_TYPE_ADD, 				// -
				PRE_OPERATOR_TYPE_SUB	, 			// +
				PRE_OPERATOR_TYPE_MAX
			}PreOperatorType;

			typedef enum :unsigned char {
				IDENTITY_OPERATOR_TYPE_UNKNOWN=0,
				IDENTITY_OPERATOR_TYPE_INC,	// ++
				IDENTITY_OPERATOR_TYPE_DEC,	// --
				IDENTITY_OPERATOR_TYPE_MAX
			}IdentityOperatorType;

			typedef enum :unsigned char {
				SEPARATOR_TYPE_UNKNOWN=0,
				SEPARATOR_TYPE_COMA,					// ,
				SEPARATOR_TYPE_SEMICOLON,    		// ;
				SEPARATOR_TYPE_PARENTHESIS_OPEN, 	// (
				SEPARATOR_TYPE_PARENTHESIS_CLOSE, 	// )
				SEPARATOR_TYPE_SQUARE_BRAKET_OPEN, 	// [
				SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE, 	// ]
				SEPARATOR_TYPE_MAX
			}SeparatorType;

			struct OpCodeInstructionEval:Instruction{

				LinkSymbolFirstAccess			linkSymbolFirstAccess;
				OpCodeInstructionSourceInfo 	instructionInfo;

				OpCodeInstructionEval(ByteCode _op_code
							 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
							 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
							 ,unsigned short _properties=0
							 ):Instruction(_op_code,_index_op1,_index_op2,_properties){
				}
			};


			struct TokenNode{

				TokenType	  		token_type; // can be operator, literal, identifier, object. (separator are not take account)
				PreOperatorType   	pre_operator_type; // !,+,-
				OperatorType  		operator_type;
				IdentityOperatorType  pre_inline_operator_identity_type; // ++i,--i
				IdentityOperatorType  post_inline_operator_identity_type; // i++,i--

				std::string value; // first access
				int line;
				std::vector<OpCodeInstructionEval> instruction; // byte code load literal/identifier(can be anonymous function), std::vector/struct.
				//std::map<short,ScriptFunction::OpCodeInstructionSourceInfo> instruction_info;
				// access info like function call, std::vector access and variable memeber
				//std::vector<tTokenNodeAccessor> accessor;

				// AST info operator.
				TokenNode *left;
				TokenNode *right;

				TokenNode(){
					line=-1;

					token_type=TokenType::TOKEN_TYPE_UNKNOWN;
					operator_type=OperatorType::OPERATOR_TYPE_UNKNOWN;
					left=right=NULL;
					pre_operator_type=PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
					pre_inline_operator_identity_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
					post_inline_operator_identity_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
				}
			};

			typedef struct {
				std::vector<OpCodeInstructionEval> instruction;
			}ContinueInstructionScope,BreakInstructionScope;

			struct FunctionInfo{

				std::vector<OpCodeInstructionEval>	 		instruction;
				ScriptFunction 				*  	function_info_object;

				FunctionInfo(ScriptFunction	* _function_info_object){
					function_info_object = _function_info_object;
				}
			};


			typedef struct {
				DirectiveType id;
				const char *str;
				//char * (*parse_fun)(const char *, int &, Scope *, PASTNode *);
			} DirectiveInfo;

			typedef char (* KeywordFunction)(const char *, int &, Scope *, bool &);

			typedef struct {
				KeywordType id;
				const char *str;
				KeywordFunction * eval_fun;
			} KeywordInfo;

			typedef struct {
				OperatorType id;
				const char *str;
				bool (*eval_fun)(const char *);
			} OperatorInfo;

			typedef struct {
				PreOperatorType id;
				const char *str;
				bool (*eval_fun)(const char *);
			} PreOperatorInfo;

			typedef struct {
				IdentityOperatorType id;
				const char *str;
				bool (*eval_fun)(const char *);
			} IdentityOperatorInfo;


			typedef struct {
				SeparatorType id;
				const char *str;
				bool (*eval_fun)(const char *);
			} SeparatorInfo;

			//---------------------------------------------------------------------------------------------------
			// GLOBAL VARS ?

			 /*ZetScript * zs;
			 ScopeFactory *scope_factory;
			 ScriptFunctionFactory *script_function_factory;
			 ScriptClassFactory *script_class_factory;*/

			 std::map<std::string,std::string *>			 	 compiled_symbol_name;

			 const char * 	current_parsing_file;
			 //int 			CURRENT_PARSING_FILE_IDX;

			 OperatorInfo defined_operator[OPERATOR_TYPE_MAX];
			 PreOperatorInfo defined_pre_operator[PRE_OPERATOR_TYPE_MAX];
			 IdentityOperatorInfo defined_identity_operator[MAX_IDENTITY_OPERATOR_TYPES];
			 SeparatorInfo defined_separator[SEPARATOR_TYPE_MAX];
			 KeywordInfo defined_keyword[KEYWORD_TYPE_MAX];

			 DirectiveInfo defined_directive[DIRECTIVE_TYPE_MAX];

			 FunctionInfo								*current_function_info;
			 std::vector<FunctionInfo *> 				*function_infos;
			 //std::vector<LinkSymbolFirstAccess>			vLinkSymbolFirstAccess;
			void  	WriteError(const char *filename, int line, const  char  *string_text, ...);

			bool	IsOperatorTypeTernaryIf(const char *s)			{return ((*s=='?'));}
			bool 	IsOperatorTypeTernaryElse(const char *s)		{return ((*s==':'));}
			bool 	IsOperatorAdd(const char *s)					{return	((*s=='+') && (*(s+1)!='+') && (*(s+1)!='='));}
			bool 	IsOperatorTypeSub(const char *s)				{return	((*s=='-') && (*(s+1)!='-') && (*(s+1)!='='));}
			bool 	IsOperatorTypeMul(const char *s)				{return ((*s=='*') && (*(s+1)!='='));}
			bool 	IsOperatorTypeDiv(const char *s)				{return ((*s=='/') && (*(s+1)!='='));}
			bool 	IsOperatorTypeMod(const char *s)				{return ((*s=='%') && (*(s+1)!='='));}
			bool 	IsOperatorTypeAssign(const char *s)				{return	((*s=='=') && (*(s+1)!='='));}
			bool 	IsOperatorTypeAssignAdd(const char *s)			{return ((*s=='+') && (*(s+1)=='='));}
			bool 	IsOperatorTypeAssignSub(const char *s)			{return ((*s=='-') && (*(s+1)=='='));}
			bool 	IsOperatorTypeAssignMul(const char *s)			{return ((*s=='*') && (*(s+1)=='='));}
			bool 	IsOperatorTypeAssignDiv(const char *s)			{return ((*s=='/') && (*(s+1)=='='));}
			bool 	IsOperatorTypeAssignMod(const char *s)			{return ((*s=='%') && (*(s+1)=='='));}
			bool 	IsOperatorTypeAssignXor(const char *s)			{return ((*s=='^') && (*(s+1)=='='));}		// ^=
			bool 	IsOperatorTypeAssignBinaryAnd(const char *s)	{return ((*s=='&') && (*(s+1)=='='));}		// &=
			bool 	IsOperatorTypeAssignBinaryOr(const char *s)		{return ((*s=='|') && (*(s+1)=='='));}  	// |=
			bool 	IsOperatorTypeAssignShiftLeft(const char *s)	{return ((*s=='<') && (*(s+1)=='<')&& (*(s+2)=='='));} 	// <<=
			bool 	IsOperatorTypeAssignShiftRight(const char *s)	{return ((*s=='>') && (*(s+1)=='>')&& (*(s+2)=='='));} 	// >>=
			bool 	IsOperatorTypeXor(const char *s)				{return ((*s=='^') && (*(s+1)!='='));}
			bool 	IsOperatorTypeBinaryAnd(const char *s)			{return ((*s=='&') && (*(s+1)!='&') && (*(s+1)!='='));}
			bool 	IsOperatorTypeBinaryOr(const char *s)			{return ((*s=='|') && (*(s+1)!='|') && (*(s+1)!='='));}
			bool 	IsOperatorTypeShiftLeft(const char *s)			{return ((*s=='<') && (*(s+1)=='<') && (*(s+2)!='='));}
			bool 	IsOperatorTypeShiftRight(const char *s)			{return	((*s=='>') && (*(s+1)=='>') && (*(s+2)!='='));}
			bool 	IsOperatorTypeLogicAnd(const char *s)			{return ((*s=='&') && (*(s+1)=='&'));}
			bool 	IsOperatorTypeLogicOr(const char *s)			{return ((*s=='|') && (*(s+1)=='|'));}
			bool 	IsOperatorTypeLogicEqual(const char *s) 		{return ((*s=='=') && (*(s+1)=='='));}
			bool 	IsOperatorTypeLogicNotEqual(const char *s)		{return ((*s=='!') && (*(s+1)=='='));}
			bool 	IsOperatorTypeLogicGt(const char *s)			{return ((*s=='>') && (*(s+1)!='>'));}
			bool 	IsOperatorTypeLogicLt(const char *s)			{return ((*s=='<') && (*(s+1)!='<'));}
			bool 	IsOperatorTypeLogicGte(const char *s)			{return ((*s=='>') && (*(s+1)=='='));}
			bool 	IsOperatorTypeLogicLte(const char *s)			{return ((*s=='<') && (*(s+1)=='='));}
			bool 	IsOperatorTypeLogicNot(const char *s)			{return ((*s=='!') && (*(s+1)!='='));}
			bool 	IsOperatorTypeInc(const char *s)				{return ((*s=='+') && (*(s+1)=='+'));}
			bool 	IsOperatorTypeDec(const char *s)				{return ((*s=='-') && (*(s+1)=='-'));}
			bool 	IsCommentSingleLine(char *s)						{return	((*s=='/') && (*(s+1)=='/'));}
			bool 	IsCommentBlockStart(char *s)					{return ((*s=='/') && (*(s+1)=='*'));}
			bool 	IsCommentBlockEnd(char *s)						{return ((*s=='*') && (*(s+1)=='/'));}
			bool 	IsOperatorTypeInstanceOf(const char *s)			{return strncmp("instanceof",s,10) == 0;}


			char *ADVANCE_TO_END_COMMENT(char *aux_p, int &line){

				if(IsCommentBlockStart(aux_p)){
					aux_p+=2; //advance first
					bool end = false;
					while(*aux_p != 0  && !end){//!IsCommentBlockEnd(aux_p) && *aux_p != 0){

						if(*aux_p == '*' && *(aux_p+1) == '/') {
							end=true;
						} // not end comment ... advance ...
						else {
							if(*aux_p=='\n'){
								line++;
							}
							aux_p++;
						}
					}
				}
				return aux_p;
			}

			char *IgnoreBlanks(const char *str, int &line) {
				char *aux_p = (char *)str;
				bool end = false;
				while(!end){
					end = true;
					while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

					if(IsCommentSingleLine(aux_p)) // ignore line
						while(*aux_p!=0 && *aux_p!='\n') aux_p++;

					else if(IsCommentBlockStart(aux_p)){
						// ignore until get the end of the comment...
						aux_p = ADVANCE_TO_END_COMMENT(aux_p, line);

						if(IsCommentBlockEnd(aux_p))
							aux_p+=2;

						end=false;
					}
					// make compatible windows format...
					if(*aux_p == '\r')
						aux_p++;

					if(*aux_p == '\n') {
						line=line+1;
						end=false;
						aux_p++;
					}
				}
				return aux_p;
			}



			std::string * GetCompiledSymbol(const std::string & s){
				if(this->compiled_symbol_name.count(s)==0){
					this->compiled_symbol_name[s]=new std::string (s);
				}
				return this->compiled_symbol_name[s];
			}

			bool eval(const char * str, const char *  _filename, int _line){
				int line =_line;
				current_parsing_file=_filename;
				char *end_char = NULL;
				bool error=false;

				PushFunction(MAIN_FUNCTION);

				end_char=EvalRecursive(str,line,MAIN_SCOPE,error);

				PopFunction();

				if(*end_char != 0){
					WriteError(current_parsing_file,line,"Unexpected \'%c\' ",*end_char);
					THROW_SCRIPT_ERROR();
					return false;
				}

				return !error;
			}

			void IniVars(){

				using namespace std::placeholders;

				current_function_info=NULL;
				//CURRENT_PARSING_FILE_IDX=-1;
				current_parsing_file=NULL;

				// init operator punctuators...
				memset(defined_operator,0,sizeof(defined_operator));
				memset(defined_pre_operator,0,sizeof(defined_pre_operator));
				memset(defined_identity_operator,0,sizeof(defined_identity_operator));
				memset(defined_separator,0,sizeof(defined_separator));
				memset(defined_keyword,0,sizeof(defined_keyword));



				defined_operator[OPERATOR_TYPE_UNKNOWN]={OPERATOR_TYPE_UNKNOWN, "none",NULL};

				defined_operator[OPERATOR_TYPE_ADD]={OPERATOR_TYPE_ADD, "+",IsOperatorAdd};
				defined_operator[OPERATOR_TYPE_SUB]={OPERATOR_TYPE_SUB, "-",IsOperatorTypeSub};
				defined_operator[OPERATOR_TYPE_MUL]={OPERATOR_TYPE_MUL, "*",IsOperatorTypeMul};
				defined_operator[OPERATOR_TYPE_DIV]={OPERATOR_TYPE_DIV, "/",IsOperatorTypeDiv};
				defined_operator[OPERATOR_TYPE_MOD]={OPERATOR_TYPE_MOD, "%",IsOperatorTypeMod};

				defined_operator[OPERATOR_TYPE_TERNARY_IF]={OPERATOR_TYPE_TERNARY_IF, "?",IsOperatorTypeTernaryIf};
				defined_operator[OPERATOR_TYPE_TERNARY_ELSE]={OPERATOR_TYPE_TERNARY_ELSE, ":",IsOperatorTypeTernaryElse};
				defined_operator[OPERATOR_TYPE_ASSIGN]={OPERATOR_TYPE_ASSIGN, "=",IsOperatorTypeAssign};
				defined_operator[OPERATOR_TYPE_ASSIGN_ADD]={OPERATOR_TYPE_ASSIGN_ADD, "+=",IsOperatorTypeAssignAdd};
				defined_operator[OPERATOR_TYPE_ASSIGN_SUB]={OPERATOR_TYPE_ASSIGN_SUB, "-=",IsOperatorTypeAssignSub};
				defined_operator[OPERATOR_TYPE_ASSIGN_MUL]={OPERATOR_TYPE_ASSIGN_MUL, "*=",IsOperatorTypeAssignMul};
				defined_operator[OPERATOR_TYPE_ASSIGN_DIV]={OPERATOR_TYPE_ASSIGN_DIV, "/=",IsOperatorTypeAssignDiv};
				defined_operator[OPERATOR_TYPE_ASSIGN_MOD]={OPERATOR_TYPE_ASSIGN_MOD, "%=",IsOperatorTypeAssignMod};
				defined_operator[OPERATOR_TYPE_ASSIGN_BINARY_XOR]={OPERATOR_TYPE_ASSIGN_BINARY_XOR,"^=",IsOperatorTypeAssignXor};
				defined_operator[OPERATOR_TYPE_ASSIGN_BINARY_AND]={OPERATOR_TYPE_ASSIGN_BINARY_AND,"&=",IsOperatorTypeAssignBinaryAnd};
				defined_operator[OPERATOR_TYPE_ASSIGN_BINARY_OR]={OPERATOR_TYPE_ASSIGN_BINARY_OR,"|=",IsOperatorTypeAssignBinaryOr};
				defined_operator[OPERATOR_TYPE_ASSIGN_SHIFT_LEFT]={OPERATOR_TYPE_ASSIGN_SHIFT_LEFT,"<<=",IsOperatorTypeAssignShiftLeft};
				defined_operator[OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT]={OPERATOR_TYPE_ASSIGN_SHIFT_RIGHT,">>=",IsOperatorTypeAssignShiftRight};

				defined_operator[OPERATOR_TYPE_XOR]={OPERATOR_TYPE_XOR, "^",IsOperatorTypeXor};
				defined_operator[OPERATOR_TYPE_BINARY_AND]={OPERATOR_TYPE_BINARY_AND, "&",IsOperatorTypeBinaryAnd};
				defined_operator[OPERATOR_TYPE_OR]={OPERATOR_TYPE_OR, "|",IsOperatorTypeBinaryOr};
				defined_operator[OPERATOR_TYPE_SHIFT_LEFT]={OPERATOR_TYPE_SHIFT_LEFT, "<<",IsOperatorTypeShiftLeft};
				defined_operator[OPERATOR_TYPE_SHIFT_RIGHT]={OPERATOR_TYPE_SHIFT_RIGHT, ">>",IsOperatorTypeShiftRight};
				defined_operator[OPERATOR_TYPE_LOGIC_AND]={OPERATOR_TYPE_LOGIC_AND, "&&",IsOperatorTypeLogicAnd};
				defined_operator[OPERATOR_TYPE_LOGIC_OR]={OPERATOR_TYPE_LOGIC_OR, "||",IsOperatorTypeLogicOr};
				defined_operator[OPERATOR_TYPE_LOGIC_EQUAL]={OPERATOR_TYPE_LOGIC_EQUAL, "==",IsOperatorTypeLogicEqual};
				defined_operator[OPERATOR_TYPE_LOGIC_NOT_EQUAL]={OPERATOR_TYPE_LOGIC_NOT_EQUAL, "!=",IsOperatorTypeLogicNotEqual};
				defined_operator[OPERATOR_TYPE_LOGIC_GT]={OPERATOR_TYPE_LOGIC_GT, ">",IsOperatorTypeLogicGt};
				defined_operator[OPERATOR_TYPE_LOGIC_LT]={OPERATOR_TYPE_LOGIC_LT, "<",IsOperatorTypeLogicLt};
				defined_operator[OPERATOR_TYPE_LOGIC_GTE]={OPERATOR_TYPE_LOGIC_GTE, ">=",IsOperatorTypeLogicGte};
				defined_operator[OPERATOR_TYPE_LOGIC_LTE]={OPERATOR_TYPE_LOGIC_LTE, "<=",IsOperatorTypeLogicLte};
				defined_operator[OPERATOR_TYPE_INSTANCEOF]={OPERATOR_TYPE_INSTANCEOF, "instanceof",IsOperatorTypeInstanceOf};


				defined_pre_operator[PRE_OPERATOR_TYPE_NOT]={PRE_OPERATOR_TYPE_NOT, "!",IsOperatorTypeLogicNot};
				defined_pre_operator[PRE_OPERATOR_TYPE_ADD]={PRE_OPERATOR_TYPE_ADD, "+",IsOperatorAdd};
				defined_pre_operator[PRE_OPERATOR_TYPE_SUB]={PRE_OPERATOR_TYPE_SUB, "-",IsOperatorTypeSub};


				defined_identity_operator[IDENTITY_OPERATOR_TYPE_INC]={IDENTITY_OPERATOR_TYPE_INC, "++",IsOperatorTypeInc};
				defined_identity_operator[IDENTITY_OPERATOR_TYPE_DEC]={IDENTITY_OPERATOR_TYPE_DEC, "--",IsOperatorTypeDec};


				// special punctuators...
				defined_separator[SEPARATOR_TYPE_COMA]={SEPARATOR_TYPE_COMA, ",",NULL};
				defined_separator[SEPARATOR_TYPE_SEMICOLON]={SEPARATOR_TYPE_SEMICOLON, ";",NULL};
				defined_separator[SEPARATOR_TYPE_PARENTHESIS_OPEN]={SEPARATOR_TYPE_PARENTHESIS_OPEN, "(",NULL};
				defined_separator[SEPARATOR_TYPE_PARENTHESIS_CLOSE]={SEPARATOR_TYPE_PARENTHESIS_CLOSE, ")",NULL};
				defined_separator[SEPARATOR_TYPE_SQUARE_BRAKET_OPEN]={SEPARATOR_TYPE_SQUARE_BRAKET_OPEN, "[",NULL};
				defined_separator[SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE]={SEPARATOR_TYPE_SQUARE_BRAKET_CLOSE, "]",NULL};

				// init special punctuators...
				// init keywords...



				defined_keyword[KeywordType::KEYWORD_TYPE_UNKNOWN] = {KEYWORD_TYPE_UNKNOWN, NULL,NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_VAR] = {KEYWORD_TYPE_VAR,"var",new KeywordFunction(std::bind(&EvalKeywordTypeVar,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_IF] = {KEYWORD_TYPE_IF,"if",new KeywordFunction(std::bind(&EvalKeywordTypeIf,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_ELSE] = {KEYWORD_TYPE_ELSE,"else"};
				defined_keyword[KeywordType::KEYWORD_TYPE_FOR] = {KEYWORD_TYPE_FOR,"for",new KeywordFunction(std::bind(&EvalKeywordTypeFor,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_WHILE] = {KEYWORD_TYPE_WHILE,"while",new KeywordFunction(std::bind(&EvalKeywordTypeWhile,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_DO_WHILE] = {KEYWORD_TYPE_DO_WHILE,"do",new KeywordFunction(std::bind(&EvalKeywordTypeDoWhile,this,_1,_2,_3,_4))}; // while is expected in the end ...

				defined_keyword[KeywordType::KEYWORD_TYPE_SWITCH] = {KEYWORD_TYPE_SWITCH,"switch",new KeywordFunction(std::bind(&EvalKeywordTypeSwitch,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_CASE] = {KEYWORD_TYPE_CASE,"case",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_BREAK] = {KEYWORD_TYPE_BREAK,"break",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_CONTINUE] = {KEYWORD_TYPE_CONTINUE,"continue",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_DEFAULT] = {KEYWORD_TYPE_DEFAULT,"default",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_FUNCTION] = {KEYWORD_TYPE_FUNCTION,"function",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_RETURN] = {KEYWORD_TYPE_RETURN,"return",new KeywordFunction(std::bind(&EvalKeywordTypeReturn,this,_1,_2,_3,_4))};
				defined_keyword[KeywordType::KEYWORD_TYPE_THIS] = {KEYWORD_TYPE_THIS,"this", NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_CLASS] = {KEYWORD_TYPE_CLASS,"class",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_NEW] = {KEYWORD_TYPE_NEW,"new", NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_DELETE] = {KEYWORD_TYPE_DELETE,"delete",NULL};
				defined_keyword[KeywordType::KEYWORD_TYPE_IN] = {KEYWORD_TYPE_IN,"in",NULL};

				// DIRECTIVES
				defined_directive[DIRECTIVE_TYPE_UNKNOWN]={DIRECTIVE_TYPE_UNKNOWN, NULL};
				defined_directive[DIRECTIVE_TYPE_INCLUDE]={DIRECTIVE_TYPE_INCLUDE, "import"};



				// create main ast management
				// init variables...
			}

			/*ScriptEval(ZetScript *_zs){
				//CURRENT_PARSING_FILE_IDX=-1;
				current_parsing_file=NULL;
				current_function_info=NULL;
				this->zs=_zs;
				this->script_function_factory=zs->GetScriptFunctionFactory();
				this->scope_factory=zs->GetScopeFactory();
				this->script_class_factory=zs->GetScriptClassFactory();

			}*/

			bool   EndExpression(const char * s){
				return *s==')' || *s==','||  *s==']' ||  *s==']' ||  *s==';' || *s == 0;
			}

			SeparatorType   EvalSeparatorType(const char *s){
				for(unsigned char i = 1; i < SEPARATOR_TYPE_MAX; i++){
					if(*defined_separator[i].str == *s){
						return defined_separator[i].id;
					}
				}
				return SeparatorType::SEPARATOR_TYPE_UNKNOWN;
			}

			OperatorType   EvalOperatorType(const char *s){
				for(unsigned char i = 1; i < OPERATOR_TYPE_MAX; i++){
					if(defined_operator[i].eval_fun(s)){
						return defined_operator[i].id;
					}
				}
				return OperatorType::OPERATOR_TYPE_UNKNOWN;
			}

			PreOperatorType   	EvalPreOperatorType(const char *s){
				for(unsigned char i = 1; i < PRE_OPERATOR_TYPE_MAX; i++){
					if(*defined_pre_operator[i].str == *s){
						return defined_pre_operator[i].id;
					}
				}
				return PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
			}

			IdentityOperatorType   evalPrePostIdentityOperator(const char *s){
				for(unsigned char i = 1; i < IDENTITY_OPERATOR_TYPE_MAX; i++){
					if(*defined_identity_operator[i].str == *s){
						return defined_identity_operator[i].id;
					}
				}
				return IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
			}

			KeywordType IsKeywordType(const char *c){
				// PRE: The first char must be starting symbol.
				char *str=(char *)c;
				//char *aux=str;

				for(int i = 0; i < KEYWORD_TYPE_MAX; i++){
					if(defined_keyword[i].str!=NULL){
						int size = strlen(defined_keyword[i].str);
						char *aux = str+size;
						if((strncmp(str,defined_keyword[i].str,size)==0) && (
								*aux == 0  || // carry end
								*aux == ' '  || // space
								*aux == '\t'  || // tab
								*aux == '('  || // ( // mostly if,for,while,switch
								*aux == '{'  || // ( // mostly else,
								*aux == '\n' || // carry return

							   IsCommentBlockStart(aux)) //start block comment
							   ){
							return defined_keyword[i].id;
						}
					}
				}
				return KeywordType::KEYWORD_TYPE_UNKNOWN;
			}

			DirectiveType IsDirectiveType(const char *c){
				// PRE: The first char must be starting symbol.
				char *str=(char *)c;

				for(int i = 0; i < DIRECTIVE_TYPE_MAX; i++){
					if(defined_directive[i].str){
						int size = strlen(defined_directive[i].str);

						if(strncmp(str,defined_directive[i].str,size)==0)
						{
							return defined_directive[i].id;
						}
					}
				}
				return DirectiveType::DIRECTIVE_TYPE_UNKNOWN;
			}


			char * EvalLiteralNumber(const char *c, int & line, std::string & value, bool & error){
				// PRE: a std::string given...
				char *aux_p = IgnoreBlanks(c,line);
				bool end=false;
				int current_part=0;
				std::string number_part[3];
				value="";
				bool is01s=true;
				//bool isInt=true;
				bool isHexa=(*aux_p == 'x' || *aux_p == 'X') || ((*aux_p == '0' && *(aux_p+1) == 'X') || (*aux_p == '0' && *(aux_p+1) == 'x'));

				if(!(('0'<=*aux_p&&*aux_p<='9') || isHexa || (*aux_p == '.'))){ // is no number directly
					return NULL;
				}

				// else is candidate to number ...
				do{

					if('0'<=*aux_p&&*aux_p<='9'){
						number_part[current_part]+=*aux_p;
					}
					else if(('a'<=*aux_p&& *aux_p<='f') || ('A'<=*aux_p && *aux_p<='F')){ // hexa ?

						if(isHexa){
								number_part[current_part]+=*aux_p;
						}
						else if(*aux_p == 'e' || *aux_p == 'E'){ // exponencial part ?

							if(current_part==0 || current_part==1){
								current_part=2;
								number_part[current_part]+=*aux_p;
								aux_p=IgnoreBlanks(aux_p+1,line);
								if(*(aux_p)=='+' || *(aux_p)=='-'){
									number_part[current_part]+=*aux_p;
									aux_p=IgnoreBlanks(aux_p+1,line);
								}
							}
							else{ // error

								 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
								 error=true;
								 return NULL;
							}
						}
						else{ // error
							 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
							 error=true;
							 return NULL;
						}

					}
					else if(*aux_p == '.'){ // fraccional part ?

						if(isHexa){
							 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
								 error=true;
								 return NULL;
						}

						if(current_part==0){
							current_part++;
							number_part[current_part]+=".";
						}
						else{ // error
							 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
							 error=true;
							 return NULL;
						}
					}

					else if(*aux_p == 'b'){ // is end binary format?
						if(!is01s || (current_part != 0)){
							 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
							 error=true;
							 return NULL;
						}

						number_part[current_part]+=*aux_p;
						end=true;
					}
					else{
						 WriteError(current_parsing_file,line ,"Invalid number format \"%s\"",value.c_str());
						 error=true;
						 return NULL;
					}

					is01s&=(('0'==*aux_p) || ('1'==*aux_p));

					aux_p++;


					//if(evalSpecialPunctuator(aux)==OperatorType::FIELD_OPERATOR);
				}while(!end && !(IsEndSymbolToken(aux_p)&&!(*aux_p=='.')));

				// check and eval token ?
				value = number_part[0]+number_part[1]+number_part[2];

				if(IsEndSymbolToken(aux_p)){
					return aux_p;
				}

				return NULL;

				// POST: detects integer/binary/fractional/hexa
			}


			bool IsAccessPunctuator(char *s){
				return *s=='.' || *s=='[' || *s=='(';
			}

			bool IsEndSymbolToken(char *s, char pre){
				return EvalOperatorType(s)!=OperatorType::OPERATOR_TYPE_UNKNOWN
					   || EvalSeparatorType(s)!=SeparatorType::SEPARATOR_TYPE_UNKNOWN
					   || *s=='.' // to separate access identifiers.
					   || *s==' '
					   || *s==0
					   || (*s=='\"' && pre!='\\');
			}

			// to std::string utils ...
			char * EvalSymbol(const char *start_word, int line,TokenNode * token_node){
				// PRE:
				unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
				LoadType load_type=LOAD_TYPE_NOT_DEFINED;

				unsigned int scope_type=0;
				void *obj=NULL,*get_obj=NULL,*const_obj=NULL;
				//char *start_str=(char *)start_word;
				char *aux=(char *)start_word;
				std::string v="";
				 //bool is_possible_number=false;
				 //int i=0;
				 bool error=false;
				 //bool start_digit = false;
				 token_node->token_type = TokenType::TOKEN_TYPE_UNKNOWN;
				 //KeywordType kw=IsKeywordType(aux);
				 //int start_line = line;
				 bool is_constant_string=false;

				 if((aux=EvalLiteralNumber(start_word,line,v ,error))==NULL){ // if not number,integer, hex, bit then is a literal std::string, boolean or identifier...

					 if(error){
						 return NULL;
					 }

					 aux=(char *)start_word;
					 // try eval identifier, boolean, std::string ...
					 token_node->token_type = TokenType::TOKEN_TYPE_LITERAL;


					char pre=0;
					if(*aux=='\"'){
						is_constant_string = true;
						aux++;
					}
					while(!IsEndSymbolToken(aux,pre)){
						pre=*aux;
						v += (*aux++);
					}

					if(*aux==0){
						return NULL;
					}

					if(*aux=='\"'){
						 if(!is_constant_string){
							 WriteError(current_parsing_file,line ,"Unexpected '\"'");
							 return NULL;
						 }
						 aux++;


						 type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING;
						 load_type=LOAD_TYPE_CONSTANT;


						if((get_obj = this->zs->getRegisteredConstantValue(v))!=NULL){
							obj = get_obj;
						}else{

							ScriptVarString *s=new ScriptVarString(zs,v);
							obj=this->zs->registerConstantValue(v,NULL,type);
							((StackElement *)obj)->stk_value=((void *)(s->m_strValue.c_str()));
							((StackElement *)obj)->var_ref=s;

						 }
					}
					 // add load std::string constant
					// compile constant ...
				 }

				 if(!is_constant_string){
					 // try parse value...
					if(v=="null"){
						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;
						load_type=LOAD_TYPE_NULL;
						obj=NULL;//ScriptVar::NullSymbol;


					}else if(v=="undefined"){
							type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;
							load_type=LOAD_TYPE_UNDEFINED;
							obj=NULL;// ScriptVar::UndefinedSymbol;
					}else if((const_obj=zs_strutils::ParseInteger(v))!=NULL){
						int value = *((int *)const_obj);
						delete (int *)const_obj;
						load_type=LOAD_TYPE_CONSTANT;
						obj=this->zs->registerConstantIntValue(v,value);
					}
					else if((const_obj=zs_strutils::ParseFloat(v))!=NULL){
						float value = *((float *)const_obj);
						delete (float *)const_obj;
						void *value_ptr;
						memcpy(&value_ptr,&value,sizeof(float));

						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
						load_type=LOAD_TYPE_CONSTANT;

						if((get_obj = this->zs->getRegisteredConstantValue(v))!=NULL){
							obj = get_obj;
						}else{
							obj=this->zs->registerConstantValue(v,value_ptr,type);
						}
					}
					else if((const_obj=zs_strutils::ParseBoolean(v))!=NULL){

						bool value = *((bool *)const_obj);
						delete (bool *)const_obj;

						type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;
						load_type=LOAD_TYPE_CONSTANT;

						if((get_obj = this->zs->getRegisteredConstantValue(v))!=NULL){
							obj = get_obj;
						}else{
							obj=this->zs->registerConstantValue(v,(void *)value,type);
						}
					}else{ // it should be an identifier token  ...

						token_node->token_type = TokenType::TOKEN_TYPE_IDENTIFIER;
						intptr_t idx_local_var=ZS_UNDEFINED_IDX;

						for(unsigned i = 0; i < current_function_info->function_info_object->arg_info.size() && idx_local_var == ZS_UNDEFINED_IDX; i++){

							if(current_function_info->function_info_object->arg_info[i].arg_name == v){
								idx_local_var=i;
							}
						}

						if(idx_local_var!=ZS_UNDEFINED_IDX){ // is arg...
							load_type=LOAD_TYPE_ARGUMENT;
							obj=(void *)idx_local_var;
						}
						else if(v == "super"){
							scope_type=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER;
						}else if(v == "this"){
							scope_type=MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS;
						}else if((get_obj = this->zs->getRegisteredConstantValue(v)) != NULL){  // check if symbol is constant ...
							obj=get_obj;
							load_type=LOAD_TYPE_CONSTANT;
						}else{
							// should be an identifier...
							if(!isIdentifierNameExpressionOk(v,line)){
								return NULL;
							}

							// search local var into current function...


							// search global ...

							// else access scope  var ?
						}

					}
				 }
				token_node->value = v;
				token_node->instruction.push_back(OpCodeInstructionEval(ByteCode::BYTE_CODE_LOAD,load_type,(intptr_t)obj,scope_type));

				return aux;
				// POST: token as literal or identifier
			}

			char *  GetIdentifierToken(const char *s, std::string & symbol){

				char *aux_p = (char *)s;
				symbol="";

				if(*aux_p!=0 && (
						   ('a' <= *aux_p && *aux_p <='z') ||
						   ('A' <= *aux_p && *aux_p <='Z') ||
						   *aux_p == '_'
						 )
					){ // let's see it has right chars...

						do{
							symbol+=*aux_p;
							aux_p++;
						}while(  (*aux_p!=0) && (
									('a' <= *aux_p && *aux_p <='z')
								||  ('0' <= *aux_p && *aux_p <='9')
								||  (*aux_p=='_')
								||  ('A' <= *aux_p && *aux_p <='Z')));

						return aux_p;
				}else{
					WriteError(current_parsing_file,-1," identifier cannot begin with %c", *aux_p);
				}
				return NULL;
			}

			bool IsLiteral(const std::string & symbol){
				return symbol=="true" || symbol == "false" || symbol=="undefined" || symbol == "null";
			}

			bool isIdentifierNameExpressionOk(const std::string & symbol, int line){

				char *aux_p = (char *)symbol.c_str();
				KeywordType kw;

				if((kw=IsKeywordType(aux_p))!=KeywordType::KEYWORD_TYPE_UNKNOWN){
					WriteError(current_parsing_file,line," Unexpected \"%s\" keyword", aux_p);
					return false;
				}

				if(IsLiteral(symbol)){
					WriteError(current_parsing_file,line," Unexpected \"%s\"", aux_p);
					return false;
				}

				if(*aux_p!=0 && (
				   ('a' <= *aux_p && *aux_p <='z') ||
				   ('A' <= *aux_p && *aux_p <='Z') ||
				   *aux_p == '_'
				 )
				){ // let's see it has right chars...

					aux_p++;
					while((*aux_p!=0) && (('a' <= *aux_p && *aux_p <='z') ||
						  ('0' <= *aux_p && *aux_p <='9') ||
						  (*aux_p=='_') ||
						  ('A' <= *aux_p && *aux_p <='Z'))){
						aux_p++;
					}
				}else{
					WriteError(current_parsing_file,line," identifier cannot begin with %c", *aux_p);
					return false;
				}

				return true;
			}
			//------------------------------------------------------------------------------------------------------------
			char * EvalFunctionObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction){

				// this function is not like keyword function, it ensures that is a function object (anonymouse function)...

				return NULL;
			}

			char * EvalDictionaryObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
					char *aux_p = (char *)s;
					std::string symbol_value;
					int lineSymbol;

					if(*aux_p == '{'){ // go for final ...

						// this solve problem void structs...
						aux_p=IgnoreBlanks(aux_p+1,line);

						while (*aux_p != '}' && *aux_p != 0){

							lineSymbol = line;

							if((aux_p=GetIdentifierToken(aux_p,symbol_value))==NULL){
								 WriteError(current_parsing_file,lineSymbol ,"Expected symbol");
								 return NULL;

							}

							 aux_p=IgnoreBlanks(aux_p,line);

							 if(*aux_p != ':'){ // expected : ...
								 WriteError(current_parsing_file,line,"Expected ':'");
								 return NULL;
							 }

							 aux_p++;

							 // go to variable...
							 if((aux_p=EvalExpression(aux_p,line,scope_info,NULL)) == NULL){  //IgnoreBlanks(aux_p+1,line);
								 return NULL;
							 }

							 // for each attribute we stack to items SYMBOL_NODE and EXPRESSION_NODE ...
							 aux_p=IgnoreBlanks(aux_p,line);

							 if(*aux_p == ','){
								 aux_p=IgnoreBlanks(aux_p+1,line);
							 }
							 else if(*aux_p != '}' ){
								 WriteError(current_parsing_file,line,"expected '}' or ','");
								 return NULL;
							 }
						}
					}
					else{
						WriteError(current_parsing_file,line,"Expected '{'");
					}
					return aux_p;
			}


			char * EvalVectorObject(const char *s,int & line,  Scope *scope_info,  std::vector<OpCodeInstructionEval> *	instruction){

				char * aux_p=IgnoreBlanks(s,line);

				if(*aux_p != '['){
					WriteError(current_parsing_file,line,"Expected '['");
					return NULL;
				}

				aux_p=IgnoreBlanks(aux_p+1,line);
				unsigned v_elements=0;

				while(*aux_p!=0 && *aux_p != ']'){

					aux_p=IgnoreBlanks(aux_p,line);

					// expression expected ...
					if(v_elements > 0){
						if(*aux_p != ','){
							WriteError(current_parsing_file,line,",");
							return NULL;
						}

						aux_p=IgnoreBlanks(aux_p+1,line);
					}

					aux_p=EvalExpression(aux_p,line,scope_info,NULL);

					v_elements++;
				}

				return aux_p;
			}

			char * EvalNewObject(const char *s,int & line,  Scope *scope_info, std::vector<OpCodeInstructionEval> 		*	instruction){
				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				std::string symbol_value;

				KeywordType key_w;

				aux_p=IgnoreBlanks(aux_p,line);

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

					if(key_w == KeywordType::KEYWORD_TYPE_NEW){
						aux_p=IgnoreBlanks(aux_p+strlen(defined_keyword[key_w].str),line);
						// try get symbol ...

						if((aux_p=GetIdentifierToken(aux_p,symbol_value))==NULL){
							 WriteError(current_parsing_file,line ,"Expected symbol");
							 return NULL;
						}

						 aux_p=IgnoreBlanks(aux_p,line);

						 if(*aux_p != '('){
							 WriteError(current_parsing_file,line,"Expected '(' after \'%s\'",defined_keyword[key_w].str);
							 return NULL;
						 }

						 do{
							if((aux_p = EvalExpression(IgnoreBlanks(aux_p+1,line),line,scope_info,instruction))==NULL){
								return NULL;
							}

							aux_p=IgnoreBlanks(aux_p,line);

							if(*aux_p != ',' && *aux_p != ')'){
								WriteError(current_parsing_file,line ,"Expected ',' or ')'");
								return NULL;
							}

						}while(*aux_p != ')');

						 //aux_p = EvalExpressionArgs('(', ')',aux_p,line,scope_info,instruction);
						 if(aux_p == NULL){
							 return NULL;
						 }

						return aux_p;
					}
				}
				return NULL;
			}

			//-----------------------------------------------------------------------------------------------------------

			ByteCode PunctuatorToInstruction(OperatorType op){

				switch(op){
				default:
					break;
				case OperatorType::OPERATOR_TYPE_SUB:
					return ByteCode::BYTE_CODE_NEG;
				case OperatorType::OPERATOR_TYPE_ADD:
					return ByteCode::BYTE_CODE_ADD;
				case OperatorType::OPERATOR_TYPE_MUL:
					return ByteCode::BYTE_CODE_MUL;
				case OperatorType::OPERATOR_TYPE_DIV:
					return ByteCode::BYTE_CODE_DIV;
				case OperatorType::OPERATOR_TYPE_MOD:
					return ByteCode::BYTE_CODE_MOD;
				case OperatorType::OPERATOR_TYPE_ASSIGN:
					return ByteCode::BYTE_CODE_STORE;
				case OperatorType::OPERATOR_TYPE_XOR:
					return ByteCode::BYTE_CODE_XOR;
				case OperatorType::OPERATOR_TYPE_BINARY_AND:
					return ByteCode::BYTE_CODE_AND;
				case OperatorType::OPERATOR_TYPE_OR:
					return ByteCode::BYTE_CODE_OR;
				case OperatorType::OPERATOR_TYPE_SHIFT_LEFT:
					return ByteCode::BYTE_CODE_SHL;
				case OperatorType::OPERATOR_TYPE_SHIFT_RIGHT:
					return ByteCode::BYTE_CODE_SHR;
				case OperatorType::OPERATOR_TYPE_LOGIC_AND:
					return ByteCode::BYTE_CODE_LOGIC_AND;
				case OperatorType::OPERATOR_TYPE_LOGIC_OR:
					return ByteCode::BYTE_CODE_LOGIC_OR;
				case OperatorType::OPERATOR_TYPE_LOGIC_EQUAL:
					return ByteCode::BYTE_CODE_EQU;
				case OperatorType::OPERATOR_TYPE_LOGIC_NOT_EQUAL:
					return ByteCode::BYTE_CODE_NOT_EQU;
				case OperatorType::OPERATOR_TYPE_LOGIC_GT:
					return ByteCode::BYTE_CODE_GT;
				case OperatorType::OPERATOR_TYPE_LOGIC_LT:
					return ByteCode::BYTE_CODE_LT;
				case OperatorType::OPERATOR_TYPE_LOGIC_GTE:
					return ByteCode::BYTE_CODE_GTE;
				case OperatorType::OPERATOR_TYPE_LOGIC_LTE:
					return ByteCode::BYTE_CODE_LTE;
				case OperatorType::OPERATOR_TYPE_INSTANCEOF:
					return ByteCode::BYTE_CODE_INSTANCEOF;
				}


				THROW_RUNTIME_ERROR(zs_strutils::Format("operator %i not implemented",op));
				return ByteCode::BYTE_CODE_END_FUNCTION;
			}


			bool MakeOperatorPrecedence(std::vector<TokenNode> * expression_tokens,std::vector<OpCodeInstructionEval> *instruction, int idx_start,int idx_end, bool & error){

				OperatorType 	op_split=OperatorType::OPERATOR_TYPE_MAX;
				int 			idx_split=-1;
				TokenNode      *split_node;

				// trivial case (no operator left, append byte code )...
				if(idx_start>=idx_end){

					// concatenate instructions ...
					instruction->insert(
							  instruction->end()
							, expression_tokens->at(idx_start).instruction.begin()
							, expression_tokens->at(idx_start).instruction.end() );

					return true;
				}

				// search for the most priority operator...
				for(int i=idx_start; i < idx_end; i++){

					if( (expression_tokens->at(i).token_type == TokenType::TOKEN_TYPE_OPERATOR)
						&& (expression_tokens->at(i).operator_type < op_split))
					{
						idx_split=i;
						op_split=expression_tokens->at(i).operator_type;
					}
				}

				if(idx_split == -1){
					WriteError(current_parsing_file,-1,"Unexpected error");
					error =true;
					return false;
				}


				// Split left/right by operator precedence...
				split_node=&expression_tokens->at(idx_split);

				bool result= MakeOperatorPrecedence(expression_tokens,instruction,idx_start,idx_split-1, error) // left branches...
											&&
							MakeOperatorPrecedence(expression_tokens,instruction,idx_split+1,idx_end,error); // right branches...

				if(result){ // if result was ok, push byte_code_op...
					instruction->push_back(OpCodeInstructionEval(PunctuatorToInstruction(split_node->operator_type)));
				}

				return result;
			}

			char * EvalExpression(const char *s, int & line, Scope *scope_info, std::vector<OpCodeInstructionEval> 	* instruction){
				// PRE: s is current std::string to eval. This function tries to eval an expression like i+1; and generates binary ast.
				// If this functions finds ';' then the function will generate ast.

				std::vector<TokenNode> expression_tokens;
				PreOperatorType pre_op = PreOperatorType::PRE_OPERATOR_TYPE_UNKNOWN;
				OperatorType op = OperatorType::OPERATOR_TYPE_UNKNOWN;
				KeywordType kw;
				bool error=false;

				bool is_first_access=false;
				//int instruction_first_access=-1;
				int instruction_identifier=-1;
				unsigned char params=0;

				//PASTNode ast_node_to_be_evaluated=NULL;
				char *aux_p=IgnoreBlanks(s,line);

				while(!EndExpression(aux_p)){

					TokenNode 	symbol_token_node
								,operator_token_node;
					kw=KeywordType::KEYWORD_TYPE_UNKNOWN;

					// check pre operator (-,+,!)...
					switch(pre_op=EvalPreOperatorType(aux_p)){

						default:
							break;
						case PreOperatorType::PRE_OPERATOR_TYPE_SUB:
						case PreOperatorType::PRE_OPERATOR_TYPE_ADD:
						case PreOperatorType::PRE_OPERATOR_TYPE_NOT:
							aux_p+=strlen(defined_operator[pre_op].str);
							break;
					}

					aux_p=IgnoreBlanks(aux_p,line);
					kw=IsKeywordType(aux_p);

					// parenthesis (evals another expression)
					if(*aux_p=='('){ // inner expression (priority)
						std::vector<OpCodeInstructionEval> 	instruction_inner;
						if((aux_p=EvalExpression(aux_p+1, line, scope_info, &instruction_inner))==NULL){
							return NULL;
						}

						// concatenate instruction ...
						instruction->insert(
											  instruction->end()
											, instruction_inner.begin()
											, instruction_inner.end() );

						if(*aux_p != ')'){
							WriteError(current_parsing_file,line ,"Expected ')'");
							return NULL;
						}

						aux_p=IgnoreBlanks(aux_p+1,line);

						operator_token_node.token_type=TokenType::TOKEN_TYPE_SUBEXPRESSION;

					}
					else{

						IdentityOperatorType pre_inline_operator_identity_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;
						IdentityOperatorType post_inline_operator_identity_type=IdentityOperatorType::IDENTITY_OPERATOR_TYPE_UNKNOWN;

						if(IsOperatorTypeInc(aux_p)){
							pre_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE_INC;
							aux_p=IgnoreBlanks(aux_p+2,line);
						}
						else if(IsOperatorTypeDec(aux_p)){
							pre_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE_DEC;
							aux_p=IgnoreBlanks(aux_p+2,line);
						}

						// pre operator identifier ...

						// first call..
						if(*aux_p=='['){ // std::vector object...

							if((aux_p=EvalVectorObject(aux_p,line,scope_info,instruction)) == NULL){
								return NULL;
							}
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_VECTOR;

						}else if(*aux_p=='{'){ // struct object ...

							if((aux_p=EvalDictionaryObject(aux_p,line,scope_info,instruction)) == NULL){
								return NULL;
							}
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_DICTIONARY;
						}else if(kw == KeywordType::KEYWORD_TYPE_NEW){

							if((aux_p=EvalNewObject(aux_p,line,scope_info,instruction)) == NULL){
								return NULL;
							}
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_NEW_OBJECT;

						}else if(kw == KeywordType::KEYWORD_TYPE_FUNCTION){ // can be after instanceof or a function object..
							if((aux_p=EvalFunctionObject(aux_p,line,scope_info,instruction)) == NULL){
								return NULL;
							}
							symbol_token_node.token_type = TokenType::TOKEN_TYPE_FUNCTION_OBJECT;
						}
						else if((aux_p = EvalSymbol(aux_p,line,&symbol_token_node))==NULL){ // finally try EvalSymbol (it should be an identifier or literal)
								return NULL;
						}

						aux_p=IgnoreBlanks(aux_p,line);
						is_first_access=false;
						params=NO_PARAMS_IS_VARIABLE;

						// check valid access punctuator...
						if(IsAccessPunctuator(aux_p) || symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER){

							if(!(      symbol_token_node.token_type == TokenType::TOKEN_TYPE_IDENTIFIER
								  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_FUNCTION_OBJECT)&& *aux_p == '(')// cannot be a number/boolean or std::string and then accessor like . / ( / [
								  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_VECTOR  )&& *aux_p == '[')// inline function object like this: 1+function(a,b){ return a+b;} + 0
								  || ((symbol_token_node.token_type == TokenType::TOKEN_TYPE_DICTIONARY  )&& *aux_p == '.')// inline struct object like this: 1+{"a":0,"b":1}.a + 0

								))
							{
									WriteError(current_parsing_file,line ,"Unexpected '%c' after literal",*aux_p);
									return NULL;
							}

							is_first_access=true;

							//instruction_first_access=0;//symbol_token_node.instruction.size()-1;
							//instruction_identifier=instruction_first_access;

							// eval accessor element (supose that was a preinsert a load instruction for identifier )...
							while(IsAccessPunctuator(aux_p)){
								ByteCode byte_code_op=ByteCode::BYTE_CODE_INVALID;
								std::string accessor_value="";

								switch(*aux_p){
								case '(': // function call
									if(is_first_access){
										params=0;
									}
									aux_p = IgnoreBlanks(aux_p+1,line);
									if(*aux_p != ')'){
										do{


											if((aux_p = EvalExpression(aux_p,line,scope_info,&symbol_token_node.instruction))==NULL){
												return NULL;
											}

											if(*aux_p != ',' && *aux_p != ')'){
												WriteError(current_parsing_file,line ,"Expected ',' or ')'");
												return NULL;
											}

											if(*aux_p == ','){
												aux_p=IgnoreBlanks(aux_p+1,line);
											}

											if(is_first_access){
												params++;
											}

										}while(*aux_p != ')');
									}
									//accessor_token.params=params;
									byte_code_op=ByteCode::BYTE_CODE_CALL;
									is_first_access=false;

									aux_p++;
									break;
								case '[': // std::vector access
									if((aux_p = EvalExpression(IgnoreBlanks(aux_p+1,line),line,scope_info,&symbol_token_node.instruction))==NULL){
										return NULL;
									}
									if(*aux_p != ']'){
										WriteError(current_parsing_file,line ,"Expected ']'");
										return NULL;
									}
									aux_p=IgnoreBlanks(aux_p+1,line);

									//symbol_token_node.instruction.push_back(Instruction(ByteCode::BYTE_CODE_VGET));
									byte_code_op=ByteCode::BYTE_CODE_VGET;
									break;
								case '.': // member access
									aux_p=IgnoreBlanks(aux_p+1,line);
									accessor_value="";
									while(!IsEndSymbolToken(aux_p)){ // get name...
										accessor_value += (*aux_p++);
									}

									if(!isIdentifierNameExpressionOk(accessor_value,line)){
										return NULL;
									}

									//save_str_symbol_value=true;
									//current_function_info->function_info_object->instruction_info[symbol_token_node.instruction.size()]=ScriptFunction::OpCodeInstructionSourceInfo(accessor_value,current_parsing_file,line);
									//symbol_token_node.instruction.push_back(Instruction(ByteCode::BYTE_CODE_LOAD)); //--> must add symbol value instruction...
									byte_code_op=ByteCode::BYTE_CODE_LOAD;
									break;
								}



								if(byte_code_op==ByteCode::BYTE_CODE_LOAD){
									//instruction_identifier=symbol_token_node.instruction.size();
									symbol_token_node.instruction[instruction_identifier].instructionInfo= OpCodeInstructionSourceInfo(current_parsing_file,line,GetCompiledSymbol(symbol_token_node.value));
								}

								OpCodeInstructionEval instruction_token=OpCodeInstructionEval(byte_code_op);
								symbol_token_node.instruction.push_back(instruction_token);

								aux_p=IgnoreBlanks(aux_p,line);



							}


							// add info to solve symbols first access (we need to put here because we have to know n params if function related)
							symbol_token_node.instruction[0].linkSymbolFirstAccess=LinkSymbolFirstAccess(
									current_function_info->function_info_object->idx_script_function
									,scope_info->idx_scope
									,symbol_token_node.value
									,params // only if first access is a function...
							);

							// add info to add as symbol value ...
							symbol_token_node.instruction[0].instructionInfo = OpCodeInstructionSourceInfo(current_parsing_file,line,GetCompiledSymbol(symbol_token_node.value));


							if(IsOperatorTypeInc(aux_p)){
								post_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE_INC;
								aux_p=IgnoreBlanks(aux_p+2,line);
							}
							else if(IsOperatorTypeDec(aux_p)){
								post_inline_operator_identity_type=IDENTITY_OPERATOR_TYPE_DEC;
								aux_p=IgnoreBlanks(aux_p+2,line);
							}

							// pre/post operator...
							symbol_token_node.pre_inline_operator_identity_type = pre_inline_operator_identity_type;
							symbol_token_node.post_inline_operator_identity_type = post_inline_operator_identity_type;
						}


					}

					expression_tokens.push_back(symbol_token_node);

					aux_p=IgnoreBlanks(aux_p,line);

					if(!EndExpression(aux_p)){ // if not end expression then a operator is expected...
						op=EvalOperatorType(aux_p);

						if(op==OperatorType::OPERATOR_TYPE_UNKNOWN){
							WriteError(current_parsing_file,line ,"Expected operator");
							return NULL;
						}

						operator_token_node.operator_type=op;
						operator_token_node.token_type=TokenType::TOKEN_TYPE_OPERATOR;

						expression_tokens.push_back(operator_token_node);
						aux_p+=strlen(defined_operator[op].str);
					}

					aux_p=IgnoreBlanks(aux_p,line);
				}

				if(aux_p==0){
					WriteError(current_parsing_file,line ,"Unexpected end of file");
					return NULL;
				}


				// make operator precedence from the AST built before...
				if(!MakeOperatorPrecedence(&expression_tokens,instruction,0,expression_tokens.size()-1,error)){
					return NULL;
				}

				// last character is a separator so it return increments by 1
				return aux_p;
			}
			//---------------------------------------------------------------------------------------------------------------
			// PARSE KEYWORDS

			char * EvalKeywordDelete(const char *s,int & line,  Scope *scope_info, bool & error){
				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				std::string symbol_value;
				KeywordType key_w;

				aux_p=IgnoreBlanks(aux_p,line);

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_DELETE){

						aux_p=IgnoreBlanks(aux_p+strlen(defined_keyword[key_w].str),line);


						if((aux_p=GetIdentifierToken(aux_p,symbol_value))==NULL){
							 WriteError(current_parsing_file,line ,"Expected symbol");
							 return NULL;
						}

						 aux_p=IgnoreBlanks(aux_p,line);

						 if(*aux_p != ';'){
							 WriteError(current_parsing_file,line,"Expected ;");
							 return NULL;
						 }

						return aux_p;
					}
				}
				return NULL;
			}

			char * IsClassMember(const char *s,int & line,ScriptClass **sc,std::string & member_symbol, bool & error){

				char *aux_p = (char *)s;
				std::string class_name;
				*sc=NULL;

				error = false;

				aux_p=IgnoreBlanks(aux_p,line);

				// check whwther the function is anonymous or not.
				if((aux_p=GetIdentifierToken(aux_p,class_name))==NULL){
					 WriteError(current_parsing_file,line ,"Expected class symbol");
					 return NULL;
				}

				aux_p=IgnoreBlanks(aux_p,line);

				if(*aux_p == ':' && *(aux_p+1)==':'){ // extension class detected...

					if((*sc=GET_SCRIPT_CLASS(class_name)) != NULL){

						if((aux_p=GetIdentifierToken(aux_p+2,member_symbol))==NULL){
							 WriteError(current_parsing_file,line ,"Expected class member symbol");
							 return NULL;
						}
						//idxScopeClass=sc->idx_scope;

						return aux_p;

					}else{
						error=true;
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeClass(const char *s,int & line, Scope *scope_info, bool & error){
				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				Scope *class_scope_info=NULL;
				int class_line;
				std::string class_name;
				std::string base_class_name="";
				ScriptClass *sc;
				KeywordType key_w;
				aux_p=IgnoreBlanks(aux_p,line);

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

					if(key_w == KeywordType::KEYWORD_TYPE_CLASS){

						if(scope_info->getIdxParent()!=ZS_UNDEFINED_IDX){
							WriteError(current_parsing_file,line,"class keyword is not allowed");
							return NULL;
						}

						aux_p=IgnoreBlanks(aux_p+strlen(defined_keyword[key_w].str),line);

						// check for symbol's name
						if((aux_p=GetIdentifierToken(aux_p,class_name))==NULL){
							 WriteError(current_parsing_file,line ,"Expected symbol");
							 return NULL;
						}

						// try to register class...
						class_line = line;


						aux_p=IgnoreBlanks(aux_p,line);

						if(*aux_p == ':' ){

							aux_p=IgnoreBlanks(aux_p+1,line);

							if((aux_p=GetIdentifierToken(aux_p,base_class_name))==NULL){
								 WriteError(current_parsing_file,line ,"Expected symbol");
								 return NULL;
							}

							aux_p=IgnoreBlanks(aux_p, line);
						}


						// register class
						if((sc=this->script_class_factory->registerClass(__FILE__, __LINE__, class_name,base_class_name))==NULL){
							return NULL;
						}

						ZS_PRINT_DEBUG("registered class \"%s\" line %i ",class_name.c_str(), class_line);

						if(*aux_p == '{' ){

							aux_p=IgnoreBlanks(aux_p+1,line);

							// TODO: Register class and baseof

							// register info class ...
							// check for named functions or vars...
							while(*aux_p != '}' && *aux_p != 0){

								//FunctionInfo *local_ifc=NULL;

								// 1st. check whether eval a keyword...
								key_w = IsKeywordType(aux_p);
								if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
									switch(key_w){
									default:
										WriteError(current_parsing_file,line,"Expected \"var\" or \"function\" keyword");
										return NULL;
										break;
									case KeywordType::KEYWORD_TYPE_FUNCTION:

										if((aux_p = EvalKeywordTypeFunction(aux_p, line,class_scope_info,error)) == NULL){
											return NULL;
										}
										break;
									case KeywordType::KEYWORD_TYPE_VAR:
										if((aux_p = EvalKeywordTypeVar(aux_p, line,class_scope_info,error)) == NULL){
											return NULL;
										}
										break;
									}
								}else{
									WriteError(current_parsing_file,line,"Expected \"var\" or \"function\" keyword");
									return NULL;
								}
								aux_p=IgnoreBlanks(aux_p,line);
							}

							if(*aux_p != '}'){
								WriteError(current_parsing_file,class_line ,"class \"%s\" declared is not closed ",class_name.c_str());
								return NULL;
							}

							aux_p=IgnoreBlanks(aux_p+1,line);

							if(*aux_p != ';'){
								WriteError(current_parsing_file,class_line ,"class \"%s\" not end with ;",class_name.c_str());
								return NULL;
							}

							return aux_p+1;

						}else{
							WriteError(current_parsing_file,line,"Expected '{'");
							return NULL;
						}
					}
				}
				return NULL;
			}

			//
			//--------------------------------------------------------------------------------------------------------------------------------------------------------
			//
			//  KEYWORDS
			//

			char * EvalKeywordTypeFunction(const char *s,int & line,  Scope *scope_info, bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				char *symbol_value=NULL,*end_var=NULL;
				KeywordType key_w;
				std::vector<ParamArgInfo> arg;
				std::string conditional_str;
				ScriptClass *sc=NULL;

				Symbol * irv=NULL;
				std::string str_name,arg_value;
				static int n_anonymous_function=0;
				//std::string class_member,class_name,
				std::string function_name="";


				int idx_scope=ZS_UNDEFINED_IDX;
				Scope *body_scope=NULL;

				idx_scope=scope_info->idx_scope;

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

					if(key_w == KeywordType::KEYWORD_TYPE_FUNCTION){

						// advance keyword...
						aux_p += strlen(defined_keyword[key_w].str);
						aux_p=IgnoreBlanks(aux_p,line);


						bool named_function = *aux_p!='(';

						if(named_function){ // is named function..

							if((end_var=IsClassMember(aux_p,line,&sc, function_name, error))!=NULL){ // check if particular case extension attribute class

								idx_scope = sc->symbol_info.symbol->idx_scope;
								//symbol_value = (char *)class_member.c_str();

							}
							else{
								if(error){
									return NULL;
								}
								else{ // get normal name...

									// check whwther the function is anonymous with a previous arithmetic operation ....

									if((end_var=GetIdentifierToken(aux_p,function_name)) == NULL){
										 WriteError(current_parsing_file,line ,"Expected symbol");
										 return NULL;
									}
								}
							}
							aux_p=end_var;
							aux_p=IgnoreBlanks(aux_p,line);
						}
						else{ //function node
							// TODO manage function object
							printf("TODO manage function object\n");
						}

						// eval function args...
						if(*aux_p == '('){ // push arguments...

							aux_p++;
							aux_p=IgnoreBlanks(aux_p,line);

							// grab words separated by ,
							while(*aux_p != 0 && *aux_p != ')'){

								aux_p=IgnoreBlanks(aux_p,line);

								if(arg.size()>0){
									if(*aux_p != ','){
										WriteError(current_parsing_file,line,"Expected ',' ");
										return NULL;
									}
									aux_p=IgnoreBlanks(aux_p+1,line);
								}

								if(*aux_p == ')' || *aux_p == ','){
									WriteError(current_parsing_file,line,"Expected arg");
									return NULL;
								}

								//int m_start_arg=line;
								if((end_var=GetIdentifierToken(aux_p,arg_value)) == NULL){
									 WriteError(current_parsing_file,line ,"Expected symbol");
									 return NULL;
								}

								// check if repeats...
								for(unsigned k = 0; k < arg.size(); k++){
									if(arg[k].arg_name == arg_value){
										WriteError(current_parsing_file,line,"Repeated argument '%s' argument ",arg_value.c_str());
										return NULL;
									}
								}

								// check whether parameter name's matches with some global variable...
								if((irv=body_scope->getSymbol(symbol_value)) != NULL){
									WriteError(current_parsing_file,line,"Ambiguous symbol argument \"%s\" name with var defined at %i", symbol_value, -1);
									return NULL;
								}
									// ok register symbol into the object function ...
								ParamArgInfo arg_info;
								arg_info.arg_name=symbol_value;
								arg.push_back(arg_info);

								aux_p=end_var;
								aux_p=IgnoreBlanks(aux_p,line);

								if(*aux_p != ')'){

								}
							}

							aux_p++;
							aux_p=IgnoreBlanks(aux_p,line);

							if(*aux_p != '{'){
								WriteError(current_parsing_file,line,"Expected '{'");
								return NULL;
							}

							// ok let's go to body..
							if((aux_p = EvalBlock(
									aux_p,
									line,
									GET_SCOPE(idx_scope),
									error
								)) != NULL){

								if(!error){
									// register function symbol...
									//int n_params=arg.size();

									/*if(named_function){ // register named function...
										if((irv=GET_SCOPE(idx_scope)->getSymbol(function_name,n_params)) != NULL){

											WriteError(current_parsing_file,line,"Function name \"%s\" is already defined with same args at %s:%i", function_name.c_str(),irv->file.c_str(),irv->line);
											return NULL;
										}

										if((irv=GET_SCOPE(idx_scope)->registerSymbol(current_parsing_file,line,function_name,n_params))==NULL){
											return NULL;
										}

									}else{ // register anonymouse function at global scope...
										irv=GET_SCOPE(IDX_GLOBAL_SCOPE)->registerAnonymouseFunction(current_parsing_file,line,n_params);
									}*/

									if(!named_function){ // register named function...
										function_name="_afun_"+zs_strutils::IntToString(n_anonymous_function++);
									}
									//--- OP
									if(sc!=NULL){ // register as variable member...
										sc->registerFunction(current_parsing_file, line, function_name,arg);
									}
									else{ // register as local variable in the function...
										current_function_info->function_info_object->registerFunction(current_parsing_file, line, function_name,arg);
									}

									//---


									GET_SCOPE(idx_scope)->popScope();
									return aux_p;
								}
							}
						}
						else{
							WriteError(current_parsing_file,line," Expected '('");
						}
					}else{
						WriteError(current_parsing_file,line,"Expected operator or function operator");
					}
				}
				return NULL;
			}

			char *  EvalKeywordTypeReturn(const char *s,int & line,  Scope *scope_info, bool & error){
				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				KeywordType key_w;
				std::string s_aux;

				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){

					if(key_w == KeywordType::KEYWORD_TYPE_RETURN){ // possible variable...
						//PASTNode child_node=NULL;
						aux_p += strlen(defined_keyword[key_w].str);
						aux_p=IgnoreBlanks(aux_p,line);
						if((aux_p = EvalExpression(aux_p, line, scope_info,&current_function_info->instruction))!= NULL){

							if(*aux_p!=';'){
								WriteError(current_parsing_file,line,"Expected ';'");
								return NULL;
							}
							aux_p=IgnoreBlanks(aux_p+1,line);
							return aux_p;
						}
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeWhile(const char *s,int & line, Scope *scope_info,  bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				char *end_expr,*start_symbol;
				KeywordType key_w;
				Scope *_currentScope=NULL;

				//PASTNode conditional_expression=NULL, while_node=NULL;
				std::string conditional_str;
				error = false;

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_WHILE){

						aux_p += strlen(defined_keyword[key_w].str);
						// evaluate conditional line ...
						aux_p=IgnoreBlanks(aux_p,line);
						if(*aux_p == '('){

							if((end_expr = EvalExpression(aux_p+1,line,_currentScope,&current_function_info->instruction)) != NULL){

								if(*end_expr != ')'){
									WriteError(current_parsing_file,line,"Expected ')'");
									return NULL;
								}

								if((start_symbol = zs_strutils::CopyFromPointerDiff(aux_p+1, end_expr))==NULL){
									return NULL;
								}

								aux_p=IgnoreBlanks(end_expr+1,line);
								if(*aux_p != '{'){
									WriteError(current_parsing_file,line,"Expected while-block open block ('{') ");
									return NULL;
								}
								if((aux_p=EvalBlock(aux_p
										,line
										,_currentScope
										,error
										))!= NULL){
									if(!error){
										scope_info->popScope();
										return aux_p;
									}
								}
							}else{
								WriteError(current_parsing_file,line,"Expected ')' while ");
								return NULL;
							}

						}else{
							WriteError(current_parsing_file,line,"Expected '(' while ");
							return NULL;
						}
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeDoWhile(const char *s,int & line, Scope *scope_info,  bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				char *end_expr,*start_symbol;
				KeywordType key_w;
				Scope *_currentScope=NULL;
				std::string conditional_str;
				error = false;

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_DO_WHILE){


						aux_p += strlen(defined_keyword[key_w].str);

						//1st evaluate body ..
						aux_p=IgnoreBlanks(aux_p,line);
						if(*aux_p != '{'){
							WriteError(current_parsing_file,line,"Expected open block ('{') in do-while expression");
							return NULL;
						}
						if((aux_p=EvalBlock(aux_p
								,line
								,_currentScope
								,error

								))!= NULL){
							if(!error){

								// Finally evaluate conditional line ...
								aux_p=IgnoreBlanks(aux_p,line);

								// check for keyword ...
								key_w = IsKeywordType(aux_p);

								if(key_w!=KEYWORD_TYPE_WHILE){
									WriteError(current_parsing_file,line,"expected while keyword");
									return NULL;
								}

								aux_p += strlen(defined_keyword[key_w].str);

								aux_p=IgnoreBlanks(aux_p,line);

								if(*aux_p == '('){

									if((end_expr = EvalExpression(aux_p+1,line,_currentScope,&current_function_info->instruction)) != NULL){

										if(*end_expr != ')'){
											WriteError(current_parsing_file,line,"Expected ')'");
											return NULL;
										}
										if((start_symbol = zs_strutils::CopyFromPointerDiff(aux_p+1, end_expr))==NULL){
											return NULL;
										}
									}else{
										WriteError(current_parsing_file,line,"Expected ')' do-while expression");
										return NULL;
									}

								}else{
									WriteError(current_parsing_file,line,"Expected '(' do-while expression");
									return NULL;
								}

								scope_info->popScope();
								return end_expr+1;
							}
						}
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeIf(const char *s,int & line,  Scope *scope_info, bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				char *end_expr,*start_symbol;
				int dl=-1;
				KeywordType key_w;
				std::string conditional_str;
				error = false;
				//int conditional_line=0;

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_IF){

						do{

							aux_p += strlen(defined_keyword[key_w].str);
							aux_p=IgnoreBlanks(aux_p,line);

							if(*aux_p != '('){
								WriteError(current_parsing_file,line,"Expected '(' if");
								return NULL;
							}

							//conditional_line=line;

							if((end_expr = EvalExpression(aux_p+1,line,scope_info,&current_function_info->instruction)) == NULL){
								WriteError(current_parsing_file,line,"Expected ')' if ");
								return NULL;
							}

							if(*end_expr != ')'){
								WriteError(current_parsing_file,line,"Expected ')'");
								return NULL;
							}

							if(IgnoreBlanks(aux_p+1,dl)==end_expr){
								WriteError(current_parsing_file,line,"no conditional expression");
								return NULL;
							}

							if((start_symbol = zs_strutils::CopyFromPointerDiff(aux_p+1, end_expr))==NULL){
								return NULL;
							}

							conditional_str=start_symbol;

							aux_p=IgnoreBlanks(end_expr+1,line);
							if(*aux_p != '{'){

								WriteError(current_parsing_file,line,"Expected if-block open block ('{')");
								return NULL;
							}

							if((aux_p=EvalBlock(aux_p
									,line
									,scope_info
									,error
									))== NULL){
								return NULL;
							}

							if(error){
								return NULL;
							}

							aux_p=IgnoreBlanks(aux_p,line);

							bool else_key = false;
							if((key_w = IsKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
								else_key = (key_w == KeywordType::KEYWORD_TYPE_ELSE);
							}

							if(else_key){
								aux_p += strlen(defined_keyword[key_w].str);

								if(*aux_p != '{'){
									aux_p++;
								}

								aux_p=IgnoreBlanks(aux_p,line);

								bool if_key = false;
								if((key_w = IsKeywordType(aux_p)) != KeywordType::KEYWORD_TYPE_UNKNOWN){
									if_key = (key_w == KeywordType::KEYWORD_TYPE_IF);
								}

								if(!if_key){

									if(*aux_p != '{'){
										WriteError(current_parsing_file,line,"Expected else-block open block ('{')");
										return NULL;
									}

									if((aux_p=EvalBlock(aux_p
											,line
											,scope_info
											,error
											))!= NULL){
											if(!error){
												return aux_p;
											}
											else{
												return NULL;
											}
									}else{
										return NULL;
									}
								} // else keep up parsing if nodes case ...
							}else{
								return aux_p;
							}
						}while(true); // loop
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeFor(const char *s,int & line,  Scope *scope_info,  bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				KeywordType key_w;
				error=false;
				std::string eval_for;
				Scope *_currentScope=NULL;

				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_FOR){

						aux_p += strlen(defined_keyword[key_w].str);
						aux_p=IgnoreBlanks(aux_p,line);

						if(*aux_p == '('){ // ready ...

							// save scope pointer ...
							_currentScope =scope_info->pushScope(); // push current scope


							aux_p=IgnoreBlanks(aux_p+1,line);

							if(*aux_p != ';'){ // there's some var init...
								// init node ...
								KeywordType key_w = IsKeywordType(aux_p);

								if(key_w == KEYWORD_TYPE_VAR){
									if((aux_p = EvalKeywordTypeVar(aux_p,line, _currentScope,error))==NULL){
										return NULL;
									}

									aux_p = aux_p - 1; // redirect aux_p to ';'
								}
								else{

									WriteError(current_parsing_file,line,"Expected 'var' keyword");
									return NULL;
								}

							}

							aux_p=IgnoreBlanks(aux_p,line);

							key_w = IsKeywordType(aux_p);
							if(key_w == KeywordType::KEYWORD_TYPE_IN){

								//PASTNode node_for_in_right_op_expression=NULL;

								aux_p=IgnoreBlanks(aux_p+strlen(defined_keyword[KeywordType::KEYWORD_TYPE_IN].str),line);


								if((aux_p = EvalExpression((const char *)aux_p,line,_currentScope,&current_function_info->instruction)) == NULL){
									return NULL;
								}
							}

							else{ // expects conditional and post (i.e for(;;) )


								if(*aux_p != ';'){
									WriteError(current_parsing_file,line,"Expected ';'");
									return NULL;

								}

								aux_p=IgnoreBlanks(aux_p+1,line);

								if(*aux_p != ';'){ // conditional...
									char * end_p=IgnoreBlanks(aux_p+1,line);

									if(*end_p != ';'){// there's some condition if not, then is like for(X;true;X)

										if((aux_p = EvalExpression((const char *)aux_p,line,_currentScope,&current_function_info->instruction)) == NULL){
											return NULL;
										}
									}
								}

								aux_p=IgnoreBlanks(aux_p,line);

								if(*aux_p != ';'){
									WriteError(current_parsing_file,line,"Expected ';'");
									return NULL;

								}
								aux_p=IgnoreBlanks(aux_p+1,line);

								if(*aux_p != ')' ){ // finally do post op...

									if(*aux_p == ',' ){
										WriteError(current_parsing_file,line,"Unexpected ) ");
										return NULL;
									}

									do{
										if((aux_p = EvalExpression(aux_p,line,_currentScope,&current_function_info->instruction))==NULL){
											return NULL;
										}

										if(*aux_p == ',' ){
											aux_p=IgnoreBlanks(aux_p+1,line);
										}else{
											if(*aux_p != ')' ){
												WriteError(current_parsing_file,line,"Expected ')'");
												return NULL;
											}
										}

									}while(*aux_p != ')' && *aux_p != 0);
								}
							}

							if(*aux_p != ')'){
								WriteError(current_parsing_file,line,"Expected ')'");
								return NULL;
							}

							aux_p=IgnoreBlanks(aux_p+1,line);
							if(*aux_p != '{'){
								WriteError(current_parsing_file,line,"Expected '{' for-block");
								return NULL;
							}

							// eval block ...
							if((aux_p=EvalBlock(aux_p
									,line
									,_currentScope
									,error
								))!= NULL){ // true: We treat declared variables into for as another scope.
								if(!error){

									scope_info->popScope(); // push current scope
									return aux_p;
								}
							}
						}else{
							WriteError(current_parsing_file,line,"Expected '(' for");
							return NULL;
						}
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeSwitch(const char *s,int & line,  Scope *scope_info,  bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;
				Scope *scope_case=NULL;
				std::string val;
				KeywordType key_w;//,key_w2;
				Scope *currentScope=scope_info;

				error=false;
				// check for keyword ...
				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_SWITCH){

						currentScope=scope_info->pushScope();

						aux_p += strlen(defined_keyword[key_w].str);
						aux_p=IgnoreBlanks(aux_p,line);

						if(*aux_p == '('){
								aux_p=IgnoreBlanks(aux_p+1,line);
								// evaluate switch vale expression ...
								if((aux_p = EvalExpression(
										aux_p,
										line,
										scope_info,
										&current_function_info->instruction
										))==NULL)
								{
									return NULL;
								}


								if(*aux_p != ')'){
									WriteError(current_parsing_file,line,"Expected ')' switch");
									error = true;
									return NULL;
								}

								aux_p=IgnoreBlanks(aux_p+1,line);

								if(*aux_p == '{'){

									aux_p++;

									if((aux_p=EvalRecursive(aux_p, line, currentScope, error))==NULL){
										return NULL;
									}

									aux_p=IgnoreBlanks(aux_p,line);

									if(*aux_p != '}'){
										WriteError(current_parsing_file,line,"Expected '}' switch");
										return NULL;
									}

									scope_info->popScope();
									return aux_p+1;
								}
								else{
									WriteError(current_parsing_file,line,"Expected '{' switch");
									return NULL;
								}
						}
						else{
							WriteError(current_parsing_file,line,"Expected '(' switch ");
							return NULL;
						}
					}
				}
				return NULL;
			}

			char * EvalKeywordTypeVar(const char *s,int & line,  Scope *scope_info, bool & error){

				// PRE: if ifc != NULL will accept expression, if NULL it means that no expression is allowed and it will add into scriptclass

				char *aux_p = (char *)s;
				KeywordType key_w;
				char *start_var,*end_var;
				std::string s_aux,variable_name;
				//char *symbol_value;
				bool end=false;
				bool allow_for_in=true;
				//short idxScopeClass=-1;
				//short idx_scope=scope_info->getCurrentScopePointer()->idx_scope;
				//bool is_class_member=false;
				int m_startLine=0;

				ScriptClass *sc=NULL;
				ScriptClass *sc_come_from=NULL;

				if(scope_info->idx_scope!=IDX_GLOBAL_SCOPE){
					sc_come_from=scope_info->getScriptClass();//) { // BYTE_CODE_NOT GLOBAL
				}
				//is_class_member = scope_info->getIdxBaseScope() == scope_info->getCurrentScopePointer()->idx_scope;

				key_w = IsKeywordType(aux_p);

				if(key_w != KeywordType::KEYWORD_TYPE_UNKNOWN){
					if(key_w == KeywordType::KEYWORD_TYPE_VAR){ // possible variable...

						aux_p += strlen(defined_keyword[key_w].str);
						aux_p=IgnoreBlanks(aux_p,line);


						while(*aux_p != ';' && *aux_p != 0 && !end){ // JE: added multivar feature.

							//bool is_class_member=parent_scope_is_class;

							aux_p=IgnoreBlanks(aux_p,line);
							start_var=aux_p;
							m_startLine = line;
							//is_class_member=false;
							//idx_scope=scope_info->getCurrentScopePointer()->idx_scope;
							sc=NULL;

							if(sc_come_from != NULL){ // it comes from class declaration itself
								sc=sc_come_from;
								//idx_scope=sc->symbol_info.symbol->idx_scope;
								//is_class_member=true;

							}
							else{ // check if type var ClasS::v1 or v1
								if((end_var=IsClassMember(aux_p,line,&sc,variable_name, error))!=NULL){ // check if particular case extension attribute class
									//idx_scope = idxScopeClass; // override scope info
								}
								else{ // causal variable
									if(error){
										return NULL;
									}
									else{ // get normal name...

										line = m_startLine;

										// check whwther the function is anonymous with a previous arithmetic operation ....
										if((end_var=GetIdentifierToken(aux_p,variable_name))==NULL){
											WriteError(current_parsing_file,line,"Expected symbol");
											return NULL;
										}
									}
								}
							}

							KeywordType keyw = IsKeywordType(variable_name.c_str());

							if(keyw != KeywordType::KEYWORD_TYPE_UNKNOWN){ // a keyword was detected...
								WriteError(current_parsing_file,line,"Cannot use symbol name as reserverd symbol \"%s\"",defined_keyword[keyw].str);
								return NULL;
							}

							aux_p=end_var;
							aux_p=IgnoreBlanks(aux_p,line);
							//}
							bool ok_char=*aux_p == ';' || *aux_p == ',' || *aux_p == '=' ;
							if(sc!=NULL && *aux_p == '='){
								WriteError(current_parsing_file,line,"Variable member is not assignable on its declaration. Should be initialized within constructor.");
								return NULL;
							}

							if(ok_char){//(*aux_p == ';' || (*aux_p == ',' && !extension_prop))){ // JE: added multivar feature (',)).
								allow_for_in=false;
								if(*aux_p == '='){ // only for variables (not class members)
									// try to evaluate expression...
									aux_p=IgnoreBlanks(aux_p,line);

									if((aux_p = EvalExpression(start_var,m_startLine,scope_info,&current_function_info->instruction)) == NULL){
										return NULL;
									}
									line = m_startLine;
								}
								 // define as many vars is declared within ','

								//--- OP
								if(sc!=NULL){ // register as variable member...
									sc->RegisterVariable(current_parsing_file, line, variable_name);
								}
								else{ // register as local variable in the function...
									current_function_info->function_info_object->RegisterVariable(current_parsing_file, line, variable_name);
								}

								//---

								ZS_PRINT_DEBUG("registered symbol \"%s\" line %i ",variable_name.c_str(), line);
							}
							else{

								KeywordType keyw = IsKeywordType(variable_name.c_str());
								if(keyw == KeywordType::KEYWORD_TYPE_IN){ // in keyword was detected (return to evalr)...
									if(!allow_for_in){
										WriteError(current_parsing_file,line,"'in' keyword should be used with an uninitialized variable (example: for ( var e in v) {...} )", *aux_p);
										return NULL;
									}
									end=true;
								}
								else{
									WriteError(current_parsing_file,line,"unexpected '%c'", *aux_p);
									return NULL;
								}
							}

							// ignores ';' or ','
							if(*aux_p == ',')
								aux_p++;
						}

						if(*aux_p == ';'){
							aux_p++;
						}
						else{
							WriteError(current_parsing_file,line,"Expected ';'");
							return NULL;
						}
						return aux_p;
					}
				}
				return NULL;
			}

			char * EvalBlock(const char *s,int & line,  Scope *scope_info, bool & error){
				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p = (char *)s;

				//Scope *_localScope =  scope_info != NULL ? scope_info->symbol_info.ast->scope_info_ptr: NULL;
				Scope *currentScope=  NULL;
				aux_p=IgnoreBlanks(aux_p,line);

				// check for keyword ...
				if(*aux_p == '{'){
					aux_p++;

					currentScope =scope_info->getCurrentScopePointer();
					currentScope = scope_info->pushScope(); // special case... ast is created later ...

					if((aux_p = EvalRecursive(aux_p, line, currentScope,error)) != NULL){
						if(error){
							return NULL;
						}

						if(*aux_p != '}'){
							error = true;
							WriteError(current_parsing_file,line,"Expected } ");
							return NULL;
						}

						scope_info->popScope();
						return aux_p+1;
					}
				}
				return NULL;
			}

			char *EvalKeywordTypeBreak(const char *s, int & line, Scope *scope_info, bool & error){
				// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
				char *aux_p=(char *)s;

				fprintf(stderr,"find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n");

				{ // ok break is valid in current scope...

					WriteError(current_parsing_file,line,"\"break\" allowed within loop or case-switch statements");
					error = true;
					return NULL;
				}

				if(*aux_p != ';'){
					WriteError(current_parsing_file,line,"expected ';'");
					error = true;
					return NULL;
				}
			}

			char *EvalKeywordTypeContinue(const char *s, int & line, Scope *scope_info, bool & error){
				// TODO: "find findConditionForBreak if current stackBreakForWhileSwitch.size() > 0\n"
				char *aux_p=(char*)s;
				{ // ok break is valid in current scope...

					WriteError(current_parsing_file,line,"\"continue\" allowed within loop or case-switch statements");
					error = true;
					return NULL;
				}

				if(*aux_p != ';'){
					WriteError(current_parsing_file,line,"expected ';'");
					error = true;
					return NULL;
				}
			}

			char *EvalKeywordTypeDefault(const char *s, int & line, Scope *scope_info,  bool & error){
				char *aux_p=(char *)s;
				std::string value_to_eval;
				TokenNode token_node;

				//std::vector<OpCodeInstructionEval> *tokenCompiled = NULL;

				aux_p=IgnoreBlanks(aux_p, line);

				KeywordType keyw = IsKeywordType(aux_p);

				if(keyw == KeywordType::KEYWORD_TYPE_CASE){ // a keyword was detected...

					aux_p=IgnoreBlanks(aux_p,line);

					// get the symbol...
					if(*aux_p=='-'){
						// INSERT NEGATE OP.
					}
					aux_p++;

					aux_p=EvalSymbol(aux_p,line,&token_node);

					if(aux_p==NULL){ return NULL;}
				}

				aux_p=IgnoreBlanks(aux_p,line);

				if(*aux_p != ':'){
					WriteError(current_parsing_file,line,"Expected  ':' ");
					return NULL;
				}
				return aux_p+1;
			}

			char *EvalKeywordType(const char *s, int & line, Scope *scope_info, bool & error){

				// PRE: **ast_node_to_be_evaluated must be created and is i/o ast pointer variable where to write changes.
				char *aux_p= (char *)s;
				//FunctionInfo *ifc_new = NULL;
				KeywordType keyw=KeywordType::KEYWORD_TYPE_UNKNOWN;//,keyw2nd=KeywordType::KEYWORD_TYPE_UNKNOWN;

				aux_p=IgnoreBlanks(aux_p, line);

				// check if condition...
				keyw = IsKeywordType(aux_p);

				if(keyw != KeywordType::KEYWORD_TYPE_UNKNOWN){ // a keyword was detected...

					switch(keyw){
					case KEYWORD_TYPE_CASE:
					case KEYWORD_TYPE_DEFAULT:
						return EvalKeywordTypeDefault(s,line,scope_info,error);

					case KEYWORD_TYPE_FUNCTION:
						if((aux_p = EvalKeywordTypeFunction(s,line,scope_info,error)) != NULL){
							return aux_p;
						}
						error = true;
						return NULL;
					case KEYWORD_TYPE_CLASS:
						if((aux_p = EvalKeywordTypeClass(s,line,scope_info,error)) != NULL){
							return aux_p;
						}
						error = true;
						return NULL;

					default:
						if(defined_keyword[keyw].eval_fun != NULL){

							return  (*defined_keyword[keyw].eval_fun)(s,line,scope_info,error);
						}
						WriteError(current_parsing_file,line,"Not implemented");
						error = true;
						return NULL;
					}
				}
				return NULL;
			}

			char * EvalRecursive(const char *s, int & line, Scope *scope_info,  bool & error){

				// PRE: *node_to_be_evaluated must be created (the pointer is only read mode)

				//KeywordType keyw=KeywordType::KEYWORD_TYPE_UNKNOWN;
				bool custom_quit = false;
				char *aux = (char *)s;
				char *end_expr=0;
				bool processed_directive=false;

				aux=IgnoreBlanks(aux, line);

				while(*aux != 0 && !custom_quit){

					processed_directive=false;
					//children = NULL;
					// ignore all ;
					while(*aux==';' && *aux != 0){
						aux =IgnoreBlanks(aux+1, line);
					}

					if(*aux==0){ // custom case exit..
						return aux;
					}

					if(*aux == '}'){ // trivial cases...
						return aux;
					}else{

						// try directive ...
						DirectiveType directive = IsDirectiveType(aux);
						char *start_var,* end_var,*str_symbol;
						if(directive != DirectiveType::DIRECTIVE_TYPE_UNKNOWN){
							switch(directive){
							case DIRECTIVE_TYPE_INCLUDE:
								aux += strlen(defined_directive[directive].str);
								aux = IgnoreBlanks(aux,line);
								if(*aux != '\"'){
									WriteError(current_parsing_file,line,"expected starting \" directive");
									THROW_SCRIPT_ERROR();
									return NULL;
								}
								aux++;
								start_var=aux;

								while(*aux != '\n' && *aux!=0 && !(*aux=='\"' && *(aux-1)!='\\')) aux++;

								if(*aux != '\"'){
									WriteError(current_parsing_file,line,"expected end \" directive");
									THROW_SCRIPT_ERROR();
									return NULL;
								}

								end_var=aux;

								if((str_symbol=zs_strutils::CopyFromPointerDiff(start_var,end_var)) == NULL){
									THROW_SCRIPT_ERROR();
									return NULL;
								}

								ZS_PRINT_DEBUG("include file: %s",str_symbol);

								{

									// save current file info...
									std::string current_file_str=current_parsing_file;
									//int current_file_idx=CURRENT_PARSING_FILE_IDX;
									//std::string file_to_eval=str_symbol;



									try{
										zs->evalFile(str_symbol);
									}catch(exception::ScriptExceptionError & error){
										THROW_EXCEPTION(error);
										return NULL;
									}

									//restore current file info...
									current_parsing_file=current_file_str.c_str();
									//CURRENT_PARSING_FILE_IDX=current_file_idx;
								}

								aux++;// advance ..
								break;
							default:
								WriteError(current_parsing_file,line,"directive \"%s\" not supported",defined_directive[directive].str);
								break;
							}

							processed_directive = true;
							end_expr=aux;
						}
					}

					// 0st special case member class extension ...
					if(!processed_directive){ // not processed yet ...
						// 1st. check whether eval a keyword...
						if((end_expr = EvalKeywordType(aux, line, scope_info, error)) == NULL){

							// If was unsuccessful then try to eval expression.
							if(error){

								THROW_SCRIPT_ERROR();
								return NULL;
							}
							// 2nd. check whether eval a block
							if((end_expr = EvalBlock(aux
									,line
									, scope_info
									, error
									))==NULL){

								// If was unsuccessful then try to eval expression.
								if(error){
									THROW_SCRIPT_ERROR();
									return NULL;
								}
								// 2nd. try expression
								//int starting_expression=line;

								if((end_expr = EvalExpression(aux,line, scope_info,&current_function_info->instruction)) == NULL){ // something wrong was happen.
									THROW_SCRIPT_ERROR();
									return NULL;
								}

								if(*end_expr != ';'){

									WriteError(current_parsing_file,line,"expected end ;");
									THROW_SCRIPT_ERROR();
									return NULL;
								}

								end_expr=IgnoreBlanks(end_expr+1, line);
							}
						}
					}

					aux=end_expr;
					aux=IgnoreBlanks(aux, line);
				}
				return aux;
			}

			void PushFunction(ScriptFunction *sf){
				function_infos.push_back(current_function_info=new FunctionInfo(sf));
			}

			void PopFunction(){

				current_function_info->function_info_object->instruction=NULL;


				// get total size op + 1 ends with NULL
				unsigned size = (current_function_info->instruction.size() + 1) * sizeof(Instruction);
				current_function_info->function_info_object->instruction = (PtrInstruction)malloc(size);
				memset(current_function_info->function_info_object->instruction, 0, size);


				for(unsigned i=0; i < current_function_info->instruction.size(); i++){

					SymbolInfo *vis=NULL;
					OpCodeInstructionEval *instruction = &current_function_info->instruction[i];
					LinkSymbolFirstAccess *ls=&instruction->linkSymbolFirstAccess;


					if(ls->idx_script_function != ZS_UNDEFINED_IDX){ // solve first symbol first access...

						ScriptFunction *sf=GET_SCRIPT_FUNCTION(ls->idx_script_function);
						ScriptClass *sc = GET_SCRIPT_CLASS(sf->idx_class);

						if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){ // trivial this.

							if(ls->n_params==NO_PARAMS_IS_VARIABLE){
								if((vis=sc->getVariable(ls->value,sc->symbol_info.symbol->idx_scope))==0){
									THROW_RUNTIME_ERROR(zs_strutils::Format("Cannot find variable %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
									return;
								}

								instruction->op2_value=vis->idx_symbol;
							}
							else{
								if((instruction->op2_value=(intptr_t)sc->getFunction(ls->value,sc->symbol_info.symbol->idx_scope,ls->n_params))==0){
									THROW_RUNTIME_ERROR(zs_strutils::Format("Cannot find function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
									return;
								}
							}

						}else if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER){ // trivial super.

							ScriptFunction *sf_found=NULL;
							std::string str_symbol_to_find = sf->symbol_info.symbol->name;

							for(int i = sf->idx_local_function-1; i >=0 && sf_found==NULL; i--){

								if(
										(ls->n_params == (int)sc->local_function[i]->arg_info.size())
									&& (str_symbol_to_find == sc->local_function[i]->symbol_info.symbol->name)
									){
									sf_found = sc->local_function[i];
								}
							}

							// ok get the super function...
							if(sf_found == NULL){

								THROW_RUNTIME_ERROR(zs_strutils::Format("Cannot find super function %s::%s",sf->symbol_info.symbol->name.c_str(),ls->value.c_str()));
								return;
							}

							instruction->op2_value=(intptr_t)sf_found;


						}else{ // find local/global var/function ...
							bool local_found=false;
							LoadType load_type=LoadType::LOAD_TYPE_UNDEFINED;

							// try find local symbol  ...
							Scope *scope=GET_SCOPE(ls->idx_scope);
							Symbol * sc_var = scope->getSymbol(ls->value, ls->n_params);
							if(ls->n_params==NO_PARAMS_IS_VARIABLE){
								if((vis=sf->getVariable(ls->value,sc_var->idx_scope))!=NULL){
									load_type=LoadType::LOAD_TYPE_VARIABLE;
									instruction->op2_value=vis->idx_symbol;
									local_found=true;
								}
							}
							else{

								if((instruction->op2_value=(intptr_t)sf->getFunction(ls->value,sc_var->idx_scope,ls->n_params))!=0){
									load_type=LoadType::LOAD_TYPE_FUNCTION;
									local_found =true;
								}
							}

							if(!local_found){ // try global...
								Symbol * sc_var = MAIN_SCOPE->getSymbol(ls->value, ls->n_params);

								if(sc_var != NULL){
									if(ls->n_params==NO_PARAMS_IS_VARIABLE){

										if((vis=MAIN_FUNCTION->getVariable(ls->value,sc_var->idx_scope))==NULL){
											THROW_RUNTIME_ERROR(zs_strutils::Format("Cannot find variable \"%s\"",ls->value.c_str()));
											return;
										}

										load_type=LoadType::LOAD_TYPE_VARIABLE;
										instruction->op2_value=vis->idx_symbol;
									}
									else{

										if((instruction->op2_value=(intptr_t)MAIN_FUNCTION->getFunction(ls->value,sc_var->idx_scope,ls->n_params))==0){
											THROW_RUNTIME_ERROR(zs_strutils::Format("Cannot find function \"%s\"",ls->value.c_str()));
											return;
										}

										load_type=LoadType::LOAD_TYPE_FUNCTION;

									}
								}
							}

							instruction->op1_value=load_type;

							if(load_type==LoadType::LOAD_TYPE_FUNCTION){
								ScriptFunction *sf = ((ScriptFunction *)instruction->op2_value);
								if((sf->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != 0){ // function will be solved at run time because it has to check param type
									instruction->op2_value=ZS_SOLVE_AT_RUNTIME; // late binding, solve at runtime...
								}
							}

						}
					}

					// save instruction ...
					current_function_info->function_info_object->instruction[i].byte_code_op = instruction->byte_code_op;
					current_function_info->function_info_object->instruction[i].op1_value = instruction->op1_value;
					current_function_info->function_info_object->instruction[i].op2_value = instruction->op2_value;
					current_function_info->function_info_object->instruction[i].properties = instruction->properties;


					// symbol value to save at runtime ...
					if(instruction->instructionInfo.str_symbol != NULL){
						current_function_info->function_info_object->instruction_info[i]=instruction->instructionInfo;
					}

				}

				current_function_info->function_info_object->BuildLutScopeSymbols();

				// delete and popback function information...
				delete(current_function_info);
				function_infos.pop_back();

				current_function_info = NULL;
				if(function_infos.size() > 0){
					current_function_info = function_infos.at(function_infos.size()-1);
				}


			}



			/*~ScriptEval() {
				for(int i=0;i<KEYWORD_TYPE_MAX;i++){
					if(defined_keyword[i].eval_fun != NULL ){
						delete defined_keyword[i].eval_fun;

					}
				}


			}*/
	}

}

