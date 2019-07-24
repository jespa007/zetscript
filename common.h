/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __ZETSCRIPT_DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif


#define ZS_SOLVE_AT_RUNTIME				 0
#define ZS_ERROR						-1
#define ZS_UNDEFINED_IDX 				-1
//#define ZS_FUNCTION_NOT_FOUND_IDX	 	-2
#define ZS_THIS_IDX						-3

#define MAX_N_ARGS						 6
#define NO_PARAMS_IS_VARIABLE			-1
//#define NO_PARAMS_SYMBOL_ONLY			-2
#define NO_PARAMS_IS_CLASS				-2
#define DEFAULT_NO_FILENAME				"no_file"

// HELPER FUNCTIONS
#define NEW_CLASS_VAR_BY_IDX(idx)	 					((this->zs)->instanceScriptVariableByIdx(idx))
#define GET_SCRIPT_CLASS(idx)	 						((this->zs)->getScriptClass(idx))
#define GET_SCRIPT_CLASS_NAME(idx) 						((this->zs)->getScriptClassName(idx))
#define SCRIPT_CLASS_MAIN								((this->zs)->getScriptClass(IDX_CLASS_MAIN))    // 0 is the main class
#define SCRIPT_CLASS_STRING								((this->zs)->getScriptClass(IDX_CLASS_STRING))
#define SCRIPT_CLASS_STRUCT								((this->zs)->getScriptClass(IDX_CLASS_STRUCT))
#define SCRIPT_CLASS_VECTOR								((this->zs)->getScriptClass(IDX_CLASS_VECTOR))
#define SCRIPT_CLASS_FUNCTOR							((this->zs)->getScriptClass(IDX_CLASS_FUNCTOR))
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(s)			((this->zs)->getScriptClassBy_C_ClassPtr(s))    // 0 is the main class
#define GET_IDX_2_CLASS_C_STR(idx) 						((this->zs)->getScriptClass(idx)->str_class_ptr_type)
#define REGISTER_C_BASE_SYMBOLS(o)		   				((this->zs)->register_C_BaseSymbols(o))
#define GET_SCOPE						 				((this->scope_factory))->getScope
#define MAIN_SCOPE										((this->scope_factory))->getScope(IDX_GLOBAL_SCOPE)
#define NEW_SCOPE										((this->scope_factory))->newScope

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)						((this->scope_factory)->getScope(idx)->getIdxBaseScope()==IDX_GLOBAL_SCOPE)

#define DO_CAST											((this->zs))->doCast
#define GET_IDX_CLASS_FROM_ITS_C_TYPE					((this->zs))->getIdxClassFromIts_C_Type
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX					((this->zs))->instanceScriptVariableByIdx
#define GET_METAMETHOD(m)								((this->zs))->getMetamethod(m)
#define IS_IDX_CLASS_INSTANCEOF(zs,_class,_instance)	((this->zs))->isIdxClassInstanceOf(_class,_instance)

#define NEW_SCRIPT_FUNCTION								((this->script_function_factory))->newScriptFunction
#define GET_SCRIPT_FUNCTION(idx)	 					((this->script_function_factory))->getScriptFunction(idx)
#define MAIN_FUNCTION									GET_SCRIPT_FUNCTION(IDX_MAIN_FUNCTION)


namespace zetscript{





	//typedef CASTNode *PASTNode;
	class CScriptFunction;
	class CScope;
	class CScriptClass;
	class CScriptVariable;
	struct Symbol;
	struct FunctionInfo;
	struct ScopeVarInnerBlockInfo;


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

	/*enum __PUNCTUATOR_TYPE_OLD__
		:unsigned char {

		UNKNOWN_PUNCTUATOR = 0,

		//--------------------------------
		// First OPERATORS 2 char size

		SHIFT_LEFT_PUNCTUATOR = 1, // <<
		SHIFT_RIGHT_PUNCTUATOR, // >>

		LOGIC_AND_PUNCTUATOR, // &&
		LOGIC_OR_PUNCTUATOR, // ||
		LOGIC_EQUAL_PUNCTUATOR, // =
		LOGIC_NOT_EQUAL_PUNCTUATOR, // !=
		LOGIC_GTE_PUNCTUATOR, // >=
		LOGIC_LTE_PUNCTUATOR, // <=
		INSTANCEOF_PUNCTUATOR, // instanceof

		PRE_INC_PUNCTUATOR, // ++
		PRE_DEC_PUNCTUATOR, // --

		POST_INC_PUNCTUATOR, // ++
		POST_DEC_PUNCTUATOR, // --

		// Then OPERATORS 1 char size
		ADD_PUNCTUATOR, // a+b
		SUB_PUNCTUATOR, // a+(-b)
		MUL_PUNCTUATOR, // *
		DIV_PUNCTUATOR, // /
		MOD_PUNCTUATOR, // %

		FIELD_PUNCTUATOR, // .

		ASSIGN_PUNCTUATOR, // =
		ADD_ASSIGN_PUNCTUATOR, // +=
		SUB_ASSIGN_PUNCTUATOR, // -=
		MUL_ASSIGN_PUNCTUATOR, // *=
		DIV_ASSIGN_PUNCTUATOR, // /=
		MOD_ASSIGN_PUNCTUATOR, // %=

		BINARY_XOR_PUNCTUATOR, // ^
		BINARY_AND_PUNCTUATOR, // &
		BINARY_OR_PUNCTUATOR, // |

		LOGIC_GT_PUNCTUATOR, // >
		LOGIC_LT_PUNCTUATOR, // <
		LOGIC_NOT_PUNCTUATOR, // !

		TERNARY_IF_PUNCTUATOR, // ?
		TERNARY_ELSE_PUNCTUATOR, // :

		MAX_OPERATOR_PUNCTUATORS,

		//---------------------------
		// SPECIAL CHARACTERS
		START_SPECIAL_PUNCTUATORS = MAX_OPERATOR_PUNCTUATORS,
		COMA_PUNCTUATOR = START_SPECIAL_PUNCTUATORS,
		SEMICOLON_PUNCTUATOR,

		OPEN_PARENTHESIS_PUNCTUATOR,
		CLOSE_PARENTHESIS_PUNCTUATOR,

		OPEN_BRAKET_PUNCTUATOR,
		CLOSE_BRAKET_PUNCTUATOR,

		OPEN_SQUARE_BRAKET_PUNCTUATOR,
		CLOSE_SQUARE_BRAKET_PUNCTUATOR,

		MAX_SPECIAL_PUNCTUATORS,
		MAX_PUNCTUATORS

	};*/

	typedef enum
		:unsigned char {

		LOAD_TYPE_NOT_DEFINED = 0,
		LOAD_TYPE_NULL,
		LOAD_TYPE_UNDEFINED,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_FUNCTION,
		LOAD_TYPE_ARGUMENT
	}LOAD_TYPE;

	typedef enum {
		IDX_INVALID = -1, IDX_THIS = -10
	}IDX_OBJ_SPECIAL_VALUE;

	typedef enum {
		CREATE_FUNCTION = 0, DESTROY_FUNCTION
	}PROXY_CREATOR;

	typedef enum:char{

		// ARITMETHIC OPERATORS.

		INVALID_OP=-1,
		END_FUNCTION = 0,
		LOAD, // primitive value like number/std::string or boolean...
		VGET, // std::vector access after each index is processed...
		STORE, // mov expression to var
		VPUSH, // Value push for std::vector
		PUSH_ATTR,
		EQU,  // ==
		NOT_EQU,  // !=
		LT,  // <
		LTE,  // <=
		GT,  // >
		GTE, // >=
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		NOT, // !
		NEG, // -a
		ADD, // +
		MUL, // *
		DIV, // /
		MOD,  // %
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		JMP,
		INSTANCEOF,
		JNT, // goto if not true ... goes end to conditional.
		JT, // goto if true ... goes end to conditional.
		CALL, // call function...
		NEW, // new operator...
		DELETE_OP,
		DECL_VEC, // Vector object
		DECL_STRUCT,
		RET, // ret instruction ..
		PUSH_SCOPE,
		POP_SCOPE,
		IT_INI,
		IT_SET_AND_NEXT,
		IT_CHK_END,
		MAX_OP_CODES

	}OP_CODE;


	typedef enum:char {
			EQU_METAMETHOD=0,  // ==
			NOT_EQU_METAMETHOD,  // !=
			LT_METAMETHOD,  // <
			LTE_METAMETHOD,  // <=
			NOT_METAMETHOD, // !
			GT_METAMETHOD,  // >
			GTE_METAMETHOD, // >=
			NEG_METAMETHOD, // -a
			ADD_METAMETHOD, // +
			DIV_METAMETHOD, // /
			MUL_METAMETHOD, // *
			MOD_METAMETHOD,  // %
			AND_METAMETHOD, // bitwise logic and
			OR_METAMETHOD, // bitwise logic or
			XOR_METAMETHOD, // logic xor
			SHL_METAMETHOD, // shift left
			SHR_METAMETHOD, // shift right
			SET_METAMETHOD, // store '='

			MAX_METAMETHOD_OPERATORS
	}METAMETHOD_OPERATOR;

	/*
	 enum ASM_PRE_POST_OPERATORS:char{
	 UNKNOW_PRE_POST_OPERATOR=0,
	 PRE_INC, // ++
	 POST_INC, // ++
	 PRE_DEC, // --getIdxArgument
	 POST_DEC, // --
	 PRE_NEG
	 };
	 */

	// properties shared by compiler + VM
	typedef enum:unsigned char {

			//-- COMPILER/VM TYPE VAR
		BIT_TYPE_UNDEFINED = 0,	// 0x1
		BIT_TYPE_NULL,			// 0x2
		BIT_TYPE_INTEGER,		// 0x4
		BIT_TYPE_NUMBER,		// 0x8
		BIT_TYPE_BOOLEAN,		// 0x10
		BIT_TYPE_STRING,		// 0x20
		BIT_TYPE_FUNCTION,		// 0x40
		BIT_TYPE_SCRIPTVAR,		// 0x80
		MAX_BIT_VAR_TYPE,
		//-- VM RUNTIME
		BIT_IS_C_VAR = MAX_BIT_VAR_TYPE, // 0x100
		BIT_IS_STACKVAR,				 // 0x200
		BIT_IS_INSTRUCTIONVAR,			 // 0x400
		BIT_POP_ONE,				 	 // 0x800
		BIT_UNRESOLVED_FUNCTION,		 // 0x1000
		BIT_CONSTRUCTOR_FUNCTION,		 // 0x2000
		BIT_IS_THIS_VAR,				 // 0x4000
		MAX_BIT_RUNTIME

	}STACK_ELEMENT_PROPERTY;

	enum
		:unsigned short {
		STK_PROPERTY_TYPE_UNDEFINED = (0x1 << BIT_TYPE_UNDEFINED), // is a variable not defined...
		STK_PROPERTY_TYPE_NULL = (0x1 << BIT_TYPE_NULL), // null is a assigned var ..
		STK_PROPERTY_TYPE_INTEGER = (0x1 << BIT_TYPE_INTEGER), // primitive int
		STK_PROPERTY_TYPE_NUMBER = (0x1 << BIT_TYPE_NUMBER), // primitive number
		STK_PROPERTY_TYPE_BOOLEAN = (0x1 << BIT_TYPE_BOOLEAN), // primitive bool
		STK_PROPERTY_TYPE_STRING = (0x1 << BIT_TYPE_STRING), // constant / script var
		STK_PROPERTY_TYPE_FUNCTION = (0x1 << BIT_TYPE_FUNCTION), // primitive function
		STK_PROPERTY_TYPE_SCRIPTVAR = (0x1 << BIT_TYPE_SCRIPTVAR) // always is an script class...

	};

	#define MASK_VAR_PRIMITIVE_TYPES				((0x1<<BIT_TYPE_FUNCTION)-1)
	#define GET_INS_PROPERTY_PRIMITIVE_TYPES(prop)	((prop)&MASK_VAR_PRIMITIVE_TYPES)

	#define MASK_VAR_TYPE							((0x1<<MAX_BIT_VAR_TYPE)-1)
	#define GET_INS_PROPERTY_VAR_TYPE(prop)			((prop)&MASK_VAR_TYPE)

	enum:unsigned short {
		STK_PROPERTY_IS_C_VAR = (0x1 << BIT_IS_C_VAR),
		STK_PROPERTY_IS_STACKVAR = (0x1 << BIT_IS_STACKVAR),
		STK_PROPERTY_IS_INSTRUCTIONVAR = (0x1 << BIT_IS_INSTRUCTIONVAR),
		STK_PROPERTY_READ_TWO_POP_ONE = (0x1 << BIT_POP_ONE),
		STK_PROPERTY_UNRESOLVED_FUNCTION = (0x1 << BIT_UNRESOLVED_FUNCTION),
		STK_PROPERTY_CONSTRUCTOR_FUNCTION = (0x1 << BIT_CONSTRUCTOR_FUNCTION),
		STK_PROPERTY_IS_THIS_VAR = (0x1 << BIT_IS_THIS_VAR)
	//INS_PROPERTY_START_FUNCTION_ARGS=	(0x1<<BIT_START_FUNCTION_ARGS)
	};

	#define MASK_RUNTIME						(((0x1<<(MAX_BIT_RUNTIME-BIT_IS_C_VAR))-1)<<(BIT_IS_C_VAR))
	#define GET_INS_PROPERTY_RUNTIME(prop)		((prop)&MASK_RUNTIME)

	// properties shared by compiler + instruction ..
	typedef enum{
		//-- PRE/POST OPERATORS (Byy default there's no operators involved)
		BIT_PRE_INC = 0,	// 0x1
		BIT_POST_INC,		// 0x2
		BIT_PRE_DEC,		// 0x4
		BIT_POST_DEC,		// 0x8
		BIT_PRE_NOT,		// 0x10
		MAX_BIT_PRE_POST_OP,

		//-- SCOPE TYPE (By default is global scope)
		BIT_LOCAL_SCOPE = MAX_BIT_PRE_POST_OP, 	// 0x20
		BIT_THIS_SCOPE,							// 0x40
		BIT_SUPER_SCOPE,						// 0x80
		BIT_ACCESS_SCOPE,						// 0x100
		MAX_BIT_SCOPE_TYPE,

		//-- CALL TYPE
		BIT_CHECK_IS_FUNCTION = MAX_BIT_SCOPE_TYPE,// 0x200
		BIT_DIRECT_CALL_RETURN,					// 0x400
		BIT_DEDUCE_C_CALL,						// 0x800
		BIT_CONSTRUCT_CALL,						// 0x1000
		BIT_NO_FUNCTION_CALL,					// 0x2000
		MAX_BIT_CALL_PROPERTIES,
	}INSTRUCTION_PROPERTY;

	// PRE/POST OP
	#define INS_PROPERTY_PRE_INC				(0x1<<BIT_PRE_INC) // ++
	#define INS_PROPERTY_POST_INC				(0x1<<BIT_POST_INC) // ++
	#define INS_PROPERTY_PRE_DEC				(0x1<<BIT_PRE_DEC) // --
	#define INS_PROPERTY_POST_DEC				(0x1<<BIT_POST_DEC) // --
	#define INS_PROPERTY_PRE_NEG				(0x1<<BIT_PRE_NOT)  // !
	#define MASK_PRE_POST_OP					(((0x1<<(MAX_BIT_PRE_POST_OP-BIT_PRE_INC))-1)<<(BIT_PRE_INC))
	#define GET_INS_PROPERTY_PRE_POST_OP(prop)	((prop)&MASK_PRE_POST_OP)

	//GLOBAL_SCOPE,   // by default
	#define	INS_PROPERTY_LOCAL_SCOPE			(0x1<<BIT_LOCAL_SCOPE)
	#define INS_PROPERTY_THIS_SCOPE				(0x1<<BIT_THIS_SCOPE)
	#define INS_PROPERTY_SUPER_SCOPE			(0x1<<BIT_SUPER_SCOPE)
	#define INS_PROPERTY_ACCESS_SCOPE			(0x1<<BIT_ACCESS_SCOPE)
	#define MASK_SCOPE_TYPE						(((0x1<<(MAX_BIT_SCOPE_TYPE-BIT_LOCAL_SCOPE))-1)<<(BIT_LOCAL_SCOPE))

	#define GET_INS_PROPERTY_SCOPE_TYPE(prop)	((prop)&MASK_SCOPE_TYPE)
	#define REMOVE_SCOPES(prop)					((prop)&=~MASK_SCOPE_TYPE)

	// CALL TYPE
	#define	INS_CHECK_IS_FUNCTION				(0x1<<BIT_CHECK_IS_FUNCTION)
	#define INS_PROPERTY_DIRECT_CALL_RETURN		(0x1<<BIT_DIRECT_CALL_RETURN)
	#define INS_PROPERTY_DEDUCE_C_CALL			(0x1<<BIT_DEDUCE_C_CALL)
	#define INS_PROPERTY_CONSTRUCT_CALL			(0x1<<BIT_CONSTRUCT_CALL)
	#define INS_PROPERTY_NO_FUNCTION_CALL		(0x1<<BIT_NO_FUNCTION_CALL)
	#define MASK_CALL_TYPE						(((0x1<<(MAX_BIT_CALL_PROPERTIES-INS_PROPERTY_CALLING_OBJECT))-1)<<(INS_PROPERTY_CALLING_OBJECT))
	#define GET_INS_PROPERTY_CALL_TYPE(prop)	((prop)&MASK_CALL_TYPE)

	#define MAIN_SCRIPT_CLASS_NAME 				"__MainClass__"
	#define MAIN_SCRIPT_FUNCTION_NAME 			"__MainFunction__"



	//typedef tInfoStatementOp *PInfoStatementOp;
	typedef enum {
		PROPERTY_C_OBJECT_REF 			= 0x1 << 0,
		PROPERTY_IS_DERIVATED 			= 0x1 << 1,
		PROPERTY_STATIC_REF 			= 0x1 << 2,
		PROPERTY_IS_POLYMORPHIC			= 0x1 << 3
	}SYMBOL_INFO_PROPERTY;

	//typedef tInfoStatementOp *PInfoStatementOp;
	/*enum ALE_INFO_PROPERTIES{
	 PROPERTY_ARG_VAR = 0x1 <<0,
	 };*/

	typedef enum {

		// built-in types...
		IDX_CLASS_MAIN = 0, 	// Main class ...

		// built in C types...
		IDX_TYPE_VOID_C,
		IDX_TYPE_INT_PTR_C,
		IDX_TYPE_FLOAT_PTR_C,
		IDX_TYPE_CONST_CHAR_PTR_C,
		IDX_TYPE_STRING_PTR_C,
		IDX_TYPE_BOOL_PTR_C,
		IDX_TYPE_INT_C,
		IDX_TYPE_UNSIGNED_INT_C,
		IDX_TYPE_INTPTR_T_C,
		IDX_TYPE_FLOAT_C,
		IDX_TYPE_BOOL_C,

		// built in classes...
		IDX_STACK_ELEMENT,
		IDX_CLASS_SCRIPT_VAR,
		IDX_CLASS_STRING,
		IDX_CLASS_VECTOR,
		IDX_CLASS_FUNCTOR,
		IDX_CLASS_STRUCT,

		MAX_BUILT_IN_TYPES
	}BUILT_IN_TYPE;


	typedef enum
		:unsigned char{
		 BREAK=0x1 << 0
		,CONTINUE=0x1 << 1
		,FOR_IN=0x1 << 2

	}SCOPE_PROPERTY;


	typedef void  (* PrintFunctionCallback)(const char *filename, int line, const  char  *string_text);

	typedef intptr_t (*fntConversionType)(intptr_t);

	/*typedef struct {
		KEYWORD_TYPE id;
		const char *str;
		char * (*eval_fun)(const char *, int &, CScope *, PASTNode *);
	} tKeywordInfo_Old;*/


	typedef struct {
		DIRECTIVE_TYPE id;
		const char *str;
		//char * (*parse_fun)(const char *, int &, CScope *, PASTNode *);
	} DirectiveInfo;

	/*typedef struct {
		__PUNCTUATOR_TYPE_OLD__ id;
		const char *str;
		bool (*eval_fun)(const char *);
	} PunctuatorInfo;*/



	enum {
		LEFT_NODE = 0, RIGHT_NODE
	};

	//-----------------------------

	struct Symbol {
		//public:
		std::string file;
		short line;
		short idxScope;

		std::string name;

		char n_params;

		Symbol(){
			file="";
			line=-1;

			idxScope = ZS_UNDEFINED_IDX;
			name="";
			n_params = NO_PARAMS_IS_VARIABLE;
		}

		/*Symbol(const std::string & _name, char _n_params= NO_PARAMS_IS_VARIABLE){
			file=ZS_UNDEFINED_IDX;
			line=-1;
			idxScope = ZS_UNDEFINED_IDX;

			name=_name;
			n_params=_n_params;
		}*/

		bool operator == (const Symbol & s1){
			return this->name == s1.name
				  && this->idxScope == s1.idxScope
				  && this->n_params == s1.n_params;
		}

		bool isFunction(){
			return n_params != NO_PARAMS_IS_VARIABLE;
		}

		bool matchSymbol(const Symbol & s1){
			return this->name == s1.name
				  && this->n_params == s1.n_params;
		}
	};

	//-----------------------------


	#pragma pack(push, 1)

	struct VariableSymbolInfo { // it can be a variable or function
		intptr_t ref_ptr; // pointer ref just in case is C var/function
		Symbol *symbol; // symbol name
		//short idx_class; //CScriptClass		 *class_info;
		//short idxScope;

		short idxSymbol; // idx of class function/variable symbol that keeps.

		unsigned short properties; // SYMBOL_INFO_PROPERTY
		std::string c_type; // In case is C, we need to know its type ...

		VariableSymbolInfo() {
			properties = 0;
			c_type = "";
			//idxScope = -1;
			ref_ptr = 0;
			symbol=NULL;
			//class_info=NULL;

			//idx_class = -1;
			//idxScopeVar=-1;
			idxSymbol = -1;
		}
	};

	struct OpCodeInstruction {
		OP_CODE op_code;
		unsigned char index_op1;	// left and right respectively
		intptr_t index_op2;
		unsigned short properties;
		//short idxAstNode; // define ast node for give some information at run time
		OpCodeInstruction(OP_CODE _op_code
					 ,unsigned char _index_op1=ZS_UNDEFINED_IDX
					 ,intptr_t _index_op2=ZS_UNDEFINED_IDX
					 ,unsigned short _properties=0
					 ){
			op_code=_op_code;
			index_op1=_index_op1;
			index_op2=_index_op2;
			properties=_properties;

		}
	};

	struct OpCodeInstructionSourceInfo {

			const char * file;
			short line;
			//Symbol * _symbol;
			std::string * symbol_name;

			OpCodeInstructionSourceInfo(){
				symbol_name=NULL;
				file="unknow_file";
				line=-1;
			}

			OpCodeInstructionSourceInfo(const char * _file, short _line,std::string *_symbol_name){

				file=_file;
				line=_line;
				symbol_name=_symbol_name;
			}
	};

	struct LinkSymbolFirstAccess{

		short idxScriptFunction;
		short idxScope;
		std::string value;
		char n_params;

		LinkSymbolFirstAccess(){

			idxScriptFunction=ZS_UNDEFINED_IDX;
			idxScope=ZS_UNDEFINED_IDX;
			value = "";
			n_params=ZS_UNDEFINED_IDX;
		}

		LinkSymbolFirstAccess(
				 int _idxScriptFunction
				,short _idxScope
				,const std::string & _value
				,char _n_params=0
				){
			idxScriptFunction=_idxScriptFunction;
			idxScope=_idxScope;
			value=_value;
			n_params=_n_params;
		}
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

	typedef OpCodeInstruction *PtrInstruction;

	struct StackElement {
		//VALUE_INSTRUCTION_TYPE 		type; // tells what kind of variable is. By default is object.
		void * stkValue; // operable value
		void * varRef; // stack ref in case to assign new value.
		unsigned short properties; // it tells its properties
	};

	typedef StackElement ConstantValueInfo;

	struct FunctionSymbol {

		StackElement object; // created object. undefined by default.
		intptr_t  proxy_ptr; // for proxy functions...
		//FunctionSymbol *super_function; // only for functions ...
		std::string key_value;
		//short idxAstNode; // in case there's ast node...

		FunctionSymbol() {
			proxy_ptr = 0;
			object= {

				0,// 0 value
				0,// no var ref related
				STK_PROPERTY_TYPE_UNDEFINED|STK_PROPERTY_IS_C_VAR // undefined and c ?.
			};
			key_value="";

			//idxAstNode = ZS_UNDEFINED_IDX;
			//super_function = NULL;
		}

	};

	//-------------------------------------------------------


	/**
	 * Scope register
	 */
	struct ScopeVarInnerBlockInfo {
		int *var_index;
		char n_var_index;
		short idxScope;
	};

	typedef struct _SharedPointerInfo *PInfoSharedPointer;

	typedef struct _SharedPointerInfo {
		CScriptVariable *shared_ptr;
		unsigned char n_shares;

		//short idx_0_shares;
		//PInfoSharedPointer next;
	} SharedPointerInfo;

	typedef struct _tNode * PInfoSharedPointerNode;
	typedef struct _tNode {
		SharedPointerInfo data;
		unsigned short currentStack;
		PInfoSharedPointerNode previous, next;
	} tInfoSharedPointerNode;

	typedef struct{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
	}ParamArgInfo;

	#pragma pack(pop)




}
