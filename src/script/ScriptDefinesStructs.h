#pragma once

//#include  "script/ast/CScopeInfo.h"
class tASTNode;
typedef tASTNode *PASTNode;
struct tInfoRegisteredFunctionSymbol;



//typedef tInfoStatementOp *PInfoStatementOp;
enum SYMBOL_INFO_PROPERTIES{
	C_OBJECT_REF = 0x1 <<0

};

#define MAX_PARAM_C_FUNCTION 5

enum NODE_TYPE{
	UNKNOWN_NODE=0,
	MAIN_NODE=1,
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
	CLASS_KEYWORD,
	THIS_KEYWORD,
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

	BINARY_XOR_PUNCTUATOR, // ^
	BINARY_AND_PUNCTUATOR, // &
	BINARY_OR_PUNCTUATOR, // |

	LOGIC_GT_PUNCTUATOR, // >
	LOGIC_LT_PUNCTUATOR, // <
	LOGIC_NOT_PUNCTUATOR, // !

	INLINE_IF_PUNCTUATOR, // ?
	INLINE_ELSE_PUNCTUATOR, // :


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
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_ARGUMENT
};


enum ASM_OPERATOR{
		INVALID_OP=-1,
		NOP=0,
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
		MAX_OPERATORS


	};

	enum ASM_PRE_POST_OPERATORS{
		UNKNOW_PRE_POST_OPERATOR=0,
		PRE_INC, // ++
		POST_INC, // ++
		PRE_DEC, // --
		POST_DEC // --

	};

	typedef int (*fntConversionType)(CObject *obj);


	typedef struct{
		int		 ref_aux; // pointer ref to C Var/Function
		PASTNode  ast;
		unsigned int properties;
	}tInfoRegisteredVariableSymbol;


	typedef struct{
		KEYWORD_TYPE id;
		const char *str;
		char * (* parse_fun )(const char *,int & ,  CScopeInfo *, PASTNode *);
	}tInfoKeyword;



	typedef struct{
		PUNCTUATOR_TYPE id;
		const char *str;
		bool (* parse_fun )(const char *);
	}tInfoPunctuator;


	enum{
		LEFT_NODE=0,
		RIGHT_NODE
	};

	class tASTNode{

		void destroyChildren_Recursive(PASTNode _node){

			if(_node != NULL){

				for(unsigned i = 0; i < _node->children.size(); i++){
					if(_node->children[i]!= NULL){
						destroyChildren_Recursive(_node->children[i]);
					}
				}

				/*if(_node->keyword_info!=NULL){
					print_info_cr("deallocating %s ",_node->keyword_info->str);
				}else if(_node->operator_info!=NULL){
					print_info_cr("deallocating %s ",_node->operator_info->str);
				}
				else{
					print_info_cr("deallocating %s ",_node->value_symbol.c_str());
				}*/

				_node->children.clear();
				delete _node;
				_node = NULL;
			}

		}

		void destroyChildren(){
			for(unsigned i = 0; i < children.size(); i++){
				destroyChildren_Recursive(children[i]);
			}
			children.clear();
		}

	public:

		NODE_TYPE node_type;
		tInfoKeyword *keyword_info;
		tInfoPunctuator *operator_info,*pre_post_operator_info;
		string 	value_symbol;
		string type_ptr;
		CScopeInfo *scope_info_ptr; // saves scope info ptr (only for global vars).
		string type_class;
		int definedValueline;
		PASTNode parent;
		vector<PASTNode> children; //left,right;
		void *aux_value;

		tASTNode(int preallocate_num_nodes=0){
			node_type = UNKNOWN_NODE;
			keyword_info = NULL;
			pre_post_operator_info = NULL;
			definedValueline=-1;
			operator_info=NULL;
			value_symbol="";
			parent=NULL;
			aux_value = NULL;
			scope_info_ptr = NULL;

			if(preallocate_num_nodes > 0){
				for(int i = 0; i < preallocate_num_nodes; i++){
					children.push_back(NULL);
				}
			}
		}

		~tASTNode(){
			destroyChildren();
		}

	};

	//-----------------------------

	typedef struct{
		string name; // var name
		CObject *m_obj; // in case of static objects like object-functions
		PASTNode ast; // ast node info.
		int index_var; // idx position in
	}tInfoScopeVar;


	//-----------------------------


	typedef struct{
		const char *op_str;
		ASM_OPERATOR op_id;
		int n_ops;
	}tDefOperator;




	class tInfoAsmOp{

	public:


	     //int type_op;
	     //tInfoObjectOperator *funOp;
	     //CObject *left_var_obj;
	    // void *result_obj; // can be float/bool/string or variable.

	    // string symbol_name;
		CVariable::VAR_TYPE variable_type;
	     PASTNode ast_node; // define ast node for give some information at run time
	    // int definedLine;
	     //string type_res;

	     //------------------

	     ASM_OPERATOR operator_type;
	     ASM_PRE_POST_OPERATORS pre_post_operator_type;
	     //void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_op1,index_op2; // left and right respectively

	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			variable_type = CVariable::VAR_TYPE::OBJECT;
			//variable_type=VAR_TYPE::NOT_DEFINED;
			operator_type=ASM_OPERATOR::INVALID_OP;
			pre_post_operator_type =ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
			ast_node = NULL;
			//isconvertable=NULL;
			//left_var_obj=NULL;
		  //   type_op=0;
		   //  funOp=NULL;
			//result_obj=NULL; // oject type...

		   // type_res="none";
			index_op1=index_op2=-1;
		   // ptr_value=NULL;
		}

	};

	struct tInfoStatementOp{

	    vector<tInfoAsmOp *> asm_op;
	};



	//-------------------------------------------------------

	typedef struct {
		vector<tInfoRegisteredVariableSymbol> 	m_registeredSymbol; // member variables to be copied in every new instance
		vector<tInfoRegisteredFunctionSymbol> 	m_registeredFunction; // member functions
	}tMemberDataInfo;

	struct tBaseObjectInfo{
		tInfoRegisteredVariableSymbol symbol_info;
		tMemberDataInfo 			  member_data;

		// the info asm op for each function. Will be filled at compile time.
		vector<tInfoStatementOp> statment_op;
	};


	struct tInfoRegisteredFunctionSymbol{

		tBaseObjectInfo	object_info;

		// var for function ...
		vector<string> m_arg; // tells var arg name or var type name (in of C )
		string return_type;


	};


	/**
	 * Stores the basic information to build a object through built AST structure
	 */
	typedef struct _tInfoRegisteredClass{

		tBaseObjectInfo	object_info;

		_tInfoRegisteredClass *extendClass; // in the case is and extension of class.

	}tInfoRegisteredClass;

