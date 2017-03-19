#pragma once


class CASTNode;
typedef CASTNode *PASTNode;
struct CScriptFunctionObject;
class tScopeVar;
class CScriptClass;
struct tFunctionInfo;
struct tInfoVarScopeBlock;

//int insertAstNode(CASTNode * ast_node);
//int insertScopeVar(tScopeVar * ast_node);
#define ZS_ERROR			-1
#define ZS_UNDEFINED_IDX 	-1
#define MAX_PARAM_C_FUNCTION 6

enum NODE_TYPE{
	UNKNOWN_NODE=0,
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
	BODY_NODE,
	GROUP_CASES_NODE,
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
	ACCESS_OBJECT_MEMBER_NODE,
	STRUCT_NODE,
	MAX_NODE_TYPE
};


enum KEYWORD_TYPE{
	UNKNOWN_KEYWORD=0,
	IF_KEYWORD,
	ELSE_KEYWORD,
	FOR_KEYWORD,
	WHILE_KEYWORD,
	VAR_KEYWORD,
	SWITCH_KEYWORD,
	CASE_KEYWORD,
	DEFAULT_KEYWORD,
	BREAK_KEYWORD,
	RETURN_KEYWORD,
	FUNCTION_KEYWORD,
	OPERATOR_KEYWORD,
	CLASS_KEYWORD,
	THIS_KEYWORD,
//	SUPER_KEYWORD,
	NEW_KEYWORD,
	DELETE_KEYWORD,
	MAX_KEYWORD
};

enum PUNCTUATOR_TYPE{

	UNKNOWN_PUNCTUATOR=0,

	//--------------------------------
	// First OPERATORS 2 char size


	SHIFT_LEFT_PUNCTUATOR=1, // <<
	SHIFT_RIGHT_PUNCTUATOR, // >>


	LOGIC_AND_PUNCTUATOR, // &&
	LOGIC_OR_PUNCTUATOR, // ||
	LOGIC_EQUAL_PUNCTUATOR, // =
	LOGIC_NOT_EQUAL_PUNCTUATOR, // !=
	LOGIC_GTE_PUNCTUATOR, // >=
	LOGIC_LTE_PUNCTUATOR, // <=

	PRE_INC_PUNCTUATOR, // ++
	PRE_DEC_PUNCTUATOR, // --

	POST_INC_PUNCTUATOR, // ++
	POST_DEC_PUNCTUATOR, // --

	// Then OPERATORS 1 char size
	ADD_PUNCTUATOR, // +
	SUB_PUNCTUATOR, // -
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

	COMA_PUNCTUATOR=1,
	SEMICOLON_PUNCTUATOR,

	OPEN_PARENTHESIS_PUNCTUATOR,
	CLOSE_PARENTHESIS_PUNCTUATOR,

	OPEN_BRAKET_PUNCTUATOR,
	CLOSE_BRAKET_PUNCTUATOR,

	OPEN_SQUARE_BRAKET_PUNCTUATOR,
	CLOSE_SQUARE_BRAKET_PUNCTUATOR,

	MAX_SPECIAL_PUNCTUATORS


};


enum LOAD_TYPE{

		LOAD_TYPE_NOT_DEFINED=0,
		LOAD_TYPE_NULL,
		LOAD_TYPE_UNDEFINED,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_FUNCTION,
		LOAD_TYPE_ARGUMENT
};

/*
enum SCOPE_TYPE:char{
	UNKNOWN_SCOPE=0,
	GLOBAL_SCOPE,
	LOCAL_SCOPE,
	THIS_SCOPE,
	SUPER_SCOPE,
	ACCESS_SCOPE
};*/

enum IDX_OBJ_SPECIAL_VALUE{
	IDX_INVALID = -1,
	IDX_THIS=-10
};

enum PROXY_CREATOR{
	CREATE_FUNCTION=0,
	DESTROY_FUNCTION
};


enum ASM_OPERATOR{
		INVALID_OP=0,
		NOP,
		MOV, // mov expression to var
		LOAD, // primitive value like number/string or boolean...
		EQU,  // ==
		NOT_EQU,  // !=
		LT,  // <
		LTE,  // <=
		NOT, // !
		GT,  // >
		GTE, // >=

		ADD, // +
		NEG, // -a
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		DIV, // /
		MUL, // *
		MOD,  // %
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		// special internal ops...
		JMP,
		JNT, // goto if not true ... goes end to conditional.
		JT, // goto if true ... goes end to conditional.
		CALL, // calling function after all of args are processed...
		PUSH, // push arg
		CLR, // clear args
		VGET, // vector access after each index is processed...

		VPUSH, // Value push for vector
		VPOP, // Value pop for vector

		VEC, // Vector object
		RET, // ret instruction ..

		NEW, // new operator...
		OBJECT_ACCESS, // object access .
		SAVE_I, // save current instruction...
		LOAD_I, // load value that points saved instruction...

		POP_SCOPE,
		DECL_STRUCT,
		PUSH_ATTR,
		MAX_OPERATORS


};

/*
enum ASM_PRE_POST_OPERATORS:char{
	UNKNOW_PRE_POST_OPERATOR=0,
	PRE_INC, // ++
	POST_INC, // ++
	PRE_DEC, // --
	POST_DEC, // --
	PRE_NEG
};
*/

// properties shared by compiler + VM
enum{

	//-- COMPILER/VM TYPE VAR
	BIT_TYPE_NULL=0,
	BIT_TYPE_UNDEFINED,
	BIT_TYPE_INTEGER,
	BIT_TYPE_NUMBER,
	BIT_TYPE_BOOLEAN,
	BIT_TYPE_STRING,
	BIT_TYPE_FUNCTION,
	BIT_TYPE_SCRIPTVAR,
	MAX_BIT_VAR_TYPE,

	//-- VM RUNTIME
	BIT_IS_ARG=MAX_BIT_VAR_TYPE

};


#define INS_PROPERTY_IS_ARG					(0x1<<BIT_IS_ARG)
#define MASK_RUNTIME						(((0x1<<(MAX_BIT_RUNTIME-BIT_IS_ARG))-1)<<(BIT_IS_ARG))
#define GET_INS_PROPERTY_RUNTIME(prop)		((prop)&MASK_SCOPE_TYPE)

#define INS_PROPERTY_TYPE_NULL				(0x1<<BIT_TYPE_NULL)
#define	INS_PROPERTY_TYPE_UNDEFINED			(0x1<<BIT_TYPE_UNDEFINED)
#define INS_PROPERTY_TYPE_INTEGER			(0x1<<BIT_TYPE_INTEGER) // primitive int
#define INS_PROPERTY_TYPE_NUMBER			(0x1<<BIT_TYPE_NUMBER) // primitive number
#define INS_PROPERTY_TYPE_BOOLEAN			(0x1<<BIT_TYPE_BOOLEAN) // primitive bool
#define INS_PROPERTY_TYPE_STRING			(0x1<<BIT_TYPE_STRING) // primitive string
#define INS_PROPERTY_TYPE_FUNCTION			(0x1<<BIT_TYPE_FUNCTION) // primitive function
#define INS_PROPERTY_TYPE_SCRIPTVAR			(0x1<<BIT_TYPE_SCRIPTVAR) // always is an script class...
#define MASK_VAR_TYPE						((0x1<<MAX_BIT_VAR_TYPE)-1)
#define GET_INS_PROPERTY_TYPE_VAR(prop)		((prop)&MASK_VAR_TYPE)





// properties shared by compiler + instruction ..
enum{
	//-- PRE/POST OPERATORS
	BIT_PRE_INC=0,
	BIT_POST_INC,
	BIT_PRE_DEC,
	BIT_POST_DEC,
	BIT_PRE_NOT,
	MAX_BIT_PRE_POST_OP,

	//-- SCOPE TYPE (By default is global scope)
	BIT_LOCAL_SCOPE=MAX_BIT_PRE_POST_OP,
	BIT_THIS_SCOPE,
	BIT_SUPER_SCOPE,
	BIT_ACCESS_SCOPE,
	MAX_BIT_SCOPE_TYPE,

	//-- CALL TYPE
	BIT_CALLING_OBJECT=MAX_BIT_SCOPE_TYPE,
	BIT_DIRECT_CALL_RETURN,
	MAX_BIT_CALL_PROPERTIES,
};

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
#define	INS_PROPERTY_CALLING_OBJECT			(0x1<<BIT_CALLING_OBJECT)
#define INS_PROPERTY_DIRECT_CALL_RETURN		(0x1<<BIT_DIRECT_CALL_RETURN)
#define MASK_CALL_TYPE						(((0x1<<(MAX_BIT_CALL_PROPERTIES-INS_PROPERTY_CALLING_OBJECT))-1)<<(INS_PROPERTY_CALLING_OBJECT))
#define GET_INS_PROPERTY_CALL_TYPE(prop)	((prop)&MASK_CALL_TYPE)




#define MAIN_SCRIPT_CLASS_NAME 				"__MainClass__"
#define MAIN_SCRIPT_FUNCTION_OBJECT_NAME 	"__mainFunction__"

typedef int (*fntConversionType)(CScriptVariable *obj);

//typedef tInfoStatementOp *PInfoStatementOp;
enum SYMBOL_INFO_PROPERTIES{
	PROPERTY_C_OBJECT_REF 	= 0x1 << 0,
	PROPERTY_IS_DERIVATED 	= 0x1 << 1,
	PROPERTY_VARIABLE 		= 0x1 << 2,
	PROPERTY_FUNCTION 		= 0x1 << 3,
	PROPERTY_STATIC_REF 	= 0x1 << 4,
	PROPERTY_CONSTRUCTOR 	= 0x1 << 5
};


//typedef tInfoStatementOp *PInfoStatementOp;
/*enum ALE_INFO_PROPERTIES{
	PROPERTY_IS_ARG = 0x1 <<0,
};*/

enum C_TYPE_VALID_PRIMITIVE_VAR{
	VOID_TYPE,
	INT_PTR_TYPE,
	FLOAT_PTR_TYPE,
	STRING_PTR_TYPE,
	BOOL_PTR_TYPE,
	MAX_C_TYPE_VALID_PRIMITIVE_VAR
};

enum BASIC_CLASS_TYPE{

	IDX_CLASS_MAIN=0, 	// Main class ...
	IDX_CLASS_UNDEFINED,	// 1
	IDX_CLASS_VOID,			// 2
	IDX_CLASS_NULL,			// 3
	IDX_CLASS_SCRIPT_VAR, 	// 4 script base that all object derive from it...
	IDX_CLASS_INTEGER, 	  	// 5 then our basics types ...
	IDX_CLASS_NUMBER,     	// 6
	IDX_CLASS_STRING,     	// 7
	IDX_CLASS_BOOLEAN,		// 8
	IDX_CLASS_VECTOR,		// 9
	IDX_CLASS_FUNCTOR,		// 10
	IDX_CLASS_STRUCT,		// 11
	MAX_BASIC_CLASS_TYPES
};

struct tInfoVariableSymbol{ // it can be a variable or function
	unsigned int	 ref_ptr; // pointer ref to C Var/Function
	string 	 symbol_name; // symbol name
	int idxScriptClass;//CScriptClass		 *class_info;
	int idxSymbol; // idx of class function/variable symbol that keeps.
	int idxAstNode;
	unsigned int properties; // SYMBOL_INFO_PROPERTIES
	string c_type; // In case is C, we need to know its type ...

	tInfoVariableSymbol(){
		properties=0;
		c_type="";
		idxAstNode=-1;
		symbol_name = "";
		ref_ptr=0;
		//class_info=NULL;
		//info_var_scope=NULL;
		idxScriptClass=-1;
		//idxScopeVar=-1;
		idxSymbol=-1;
	}
};


typedef struct{
	KEYWORD_TYPE id;
	const char *str;
	char * (* parse_fun )(const char *,int & ,  CScope *, PASTNode *);
}tKeywordInfo;



typedef struct{
	PUNCTUATOR_TYPE id;
	const char *str;
	bool (* parse_fun )(const char *);
}tPunctuatorInfo;


enum{
	LEFT_NODE=0,
	RIGHT_NODE
};


//-----------------------------

struct tScopeVar{
//public:
	string symbol_ref;
	string name; // var name
	//int idxScopeVar;
	int idxAstNode; // ast node info.
};

//-----------------------------


typedef struct{
	const char *op_str;
	ASM_OPERATOR op_id;
	int n_ops;
}tDefOperator;

/*
enum ASM_PROPERTIES{

};
*/

#pragma pack(1)
struct tInfoAsmOp{

	unsigned char operator_type;
	unsigned char index_op1;	// left and right respectively
	int  index_op2;
	unsigned short instruction_properties;
	short idxAstNode; // define ast node for give some information at run time

	tInfoAsmOp(){
		operator_type=ASM_OPERATOR::INVALID_OP;
		idxAstNode = -1;
		instruction_properties=0;
		index_op1=index_op2=-1;
	}

};


typedef   tInfoAsmOp **PtrStatment;

/*
struct tInfoStatementOp{

	tInfoAsmOp * asm_op;
	//unsigned          n_asm_op;

	tInfoStatementOp(){
		asm_op = NULL;
		//n_asm_op=0;
	}
};*/

//-------------------------------------------------------

struct tLocalSymbolInfo{
	vector<tInfoVariableSymbol> 	m_registeredVariable; // member variables to be copied in every new instance
	vector<int> 							vec_idx_registeredFunction; // member functions
};

struct tFunctionInfo{ // script function is shared by class and function ...

	tInfoVariableSymbol 	symbol_info;
	tLocalSymbolInfo 		local_symbols;

	//--------------------------------------
	// optimized ones...
	PtrStatment 				statment_op;
	//unsigned					 n_statment_op;

	tInfoVarScopeBlock 	*info_var_scope;
	unsigned					n_info_var_scope;

	int idxScriptFunctionObject;

	tFunctionInfo(){
		idxScriptFunctionObject=-1;
		statment_op=NULL;
		info_var_scope=NULL;
		//n_statment_op=0;
		n_info_var_scope=0;
	}
};

typedef struct {
	string filename;
	unsigned char *data;
}tInfoParsedSource;


/**
 * Scope register
 */
struct tInfoVarScopeBlock{
	int *var_index;
	unsigned  n_var_index;
	int idxScope;
};






