#include "zetscript.h"

namespace zetscript{

	const char * byte_code_to_str(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        			return "EQU";  // ==
			case BYTE_CODE_INSTANCEOF:  		return "INSTANCEOF";  // ==
			case BYTE_CODE_NOT_EQU:     		return "NOT_EQU" ;  // !=
			case BYTE_CODE_LT:          		return "LT";  // <
			case BYTE_CODE_LTE:         		return "LTE";  // <=
			case BYTE_CODE_NOT:         		return "NOT"; // !
			case BYTE_CODE_GT:          		return "GT";  // >
			case BYTE_CODE_GTE:        			return "GTE"; // >=
			case BYTE_CODE_NEG:         		return "NEG"; // !
			case BYTE_CODE_ADD:         		return "ADD"; // +
			case BYTE_CODE_SUB:         		return "SUB"; // -
			case BYTE_CODE_LOGIC_AND:   		return "LOGIC_AND"; // &&
			case BYTE_CODE_LOGIC_OR:    		return "LOGIC_OR";  // ||
			case BYTE_CODE_DIV:         		return "DIV"; // /
			case BYTE_CODE_MUL:         		return "MUL"; // *
			case BYTE_CODE_MOD:         		return "MOD";  // %
			case BYTE_CODE_BITWISE_AND:         		return "AND"; // bitwise logic and
			case BYTE_CODE_BITWISE_OR:         			return "OR"; // bitwise logic or
			case BYTE_CODE_BITWISE_XOR:         		return "XOR"; // logic xor
			case BYTE_CODE_SHL:         		return "SHL"; // shift left
			case BYTE_CODE_SHR:         		return "SHR"; // shift right
			case BYTE_CODE_STORE:       		return "STORE"; // =
			case BYTE_CODE_STORE_ADD:       	return "STORE_ADD"; // +=
			case BYTE_CODE_STORE_SUB:       	return "STORE_SUB"; // -=
			case BYTE_CODE_STORE_MUL:       	return "STORE_MUL"; // *=
			case BYTE_CODE_STORE_DIV:       	return "STORE_DIV"; // /=
			case BYTE_CODE_STORE_MOD:       	return "STORE_MOD"; // %=
			case BYTE_CODE_STORE_BITWISE_AND:       	return "STORE_AND"; // &=
			case BYTE_CODE_STORE_BITWISE_OR: 	      	return "STORE_OR"; // |=
			case BYTE_CODE_STORE_BITWISE_XOR: 	      	return "STORE_XOR"; // ^=
			case BYTE_CODE_STORE_SHL: 	      	return "STORE_SHL"; // <<=
			case BYTE_CODE_STORE_SHR: 	      	return "STORE_SHR"; // >>=
			case BYTE_CODE_PUSH_STK_GLOBAL:		return "PUSH_STK_GLOBAL"; // push stk global
			case BYTE_CODE_PUSH_STK_LOCAL:		return "PUSH_STK_LOCAL"; // push stk global
			//case BYTE_CODE_PUSH_STK_REF:		return "PUSH_STK_REF"; // push stk global
			case BYTE_CODE_PUSH_STK_THIS:		return "PUSH_STK_THIS"; // push stk global
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:		return "PUSH_STK_MEMBER_VAR"; // push stk global
			case BYTE_CODE_PUSH_STK_ELEMENT_VECTOR:	return "PUSH_STK_E@VECTOR"; // load element vector
			case BYTE_CODE_PUSH_STK_ELEMENT_THIS:	return "PUSH_STK_E@THIS"; // load element object
			case BYTE_CODE_PUSH_STK_ELEMENT_OBJECT:	return "PUSH_STK_E@OBJ"; // load element object
			case BYTE_CODE_LOAD_GLOBAL:			return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:			return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_REF:			return "LOAD_REF"; // load type var
			case BYTE_CODE_LOAD_THIS:			return "LOAD_THIS"; // load type var
			case BYTE_CODE_LOAD_CONSTRUCTOR:	return "LOAD_CTOR"; // load type var
			case BYTE_CODE_LOAD_MEMBER_VAR:			return "LOAD_MEMBER"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      	return "LOAD_???"; // load to find global
			case BYTE_CODE_LOAD_ELEMENT_THIS:	return "LOAD_E@THIS"; // load element object
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:	return "LOAD_E@VEC"; // load element vector
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:	return "LOAD_E@OBJ"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:		return "LOAD_FUNCT"; // load function
			case BYTE_CODE_LOAD_NULL:		return "LOAD_NULL"; // load undfined
			case BYTE_CODE_LOAD_STACK_ELEMENT:	return "LOAD_STK"; // load stack element

			case BYTE_CODE_LOAD_STRING:			return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_FLOAT:			return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:			return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_ZS_INT:			return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_CLASS:			return "LOAD_CLASS"; // load to find
			case BYTE_CODE_JMP:         		return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         		return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          		return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE:					return "JE"; // goto if equal ... goes end to conditional.
			case BYTE_CODE_CALL: 				return "CALL"; // calling function after all of arguments are processed...
			case BYTE_CODE_CALL_CONSTRUCTOR: 	return "CALL_CTOR"; // calling function after all of arguments are processed...
			case BYTE_CODE_NEW_VECTOR: 			return "NEW_VEC"; // Vector object (CREATE)
			case BYTE_CODE_PUSH_VECTOR_ELEMENT: return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 				return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_CLASS: 			return "NEW_CLASS"; // New object (CREATE)
			case BYTE_CODE_DELETE:				return "DELETE";
			case BYTE_CODE_POP_SCOPE: 			return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 			return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:	return "PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT:		return "NEW_OBJ"; // New object (CREATE)
			case BYTE_CODE_NEW_STRING:			return "NEW_STR";
			case BYTE_CODE_IT_END: 				return "IT_END"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_IT_NEXT:				return "IT_NEXT"; // BYTE_CODE_SET_AND_NEXT
			case BYTE_CODE_STORE_CONST:			return "STORE_CONST"; // BYTE_CODE_STORE_CONST
			case BYTE_CODE_PRE_INC:				return "PRE_INC"; // ++i
			case BYTE_CODE_PRE_DEC:				return "PRE_DEC"; // --i
			case BYTE_CODE_POST_INC:			return "POST_INC"; // i++
			case BYTE_CODE_NEG_POST_INC:		return "NEG_POST_INC"; // -i++
			case BYTE_CODE_POST_DEC:			return "POST_DEC"; // i--
			case BYTE_CODE_NEG_POST_DEC:		return "NEG_POST_DEC"; // -i--
			case BYTE_CODE_RESET_STACK:			return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION

			default:
				break;
		}
		return "unknow_op";
	}

	const char * byte_code_to_operator_str(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        			return "==";  // ==
			case BYTE_CODE_INSTANCEOF:  		return "instanceof";  // ==
			case BYTE_CODE_NOT_EQU:     		return "!=" ;  // !=
			case BYTE_CODE_LT:          		return "<";  // <
			case BYTE_CODE_LTE:         		return "<=";  // <=
			case BYTE_CODE_NOT:         		return "!"; // !
			case BYTE_CODE_GT:          		return ">";  // >
			case BYTE_CODE_GTE:        			return ">="; // >=
			case BYTE_CODE_NEG:         		return "-"; // -
			case BYTE_CODE_ADD:         		return "+"; // +
			case BYTE_CODE_SUB:         		return "-"; // -
			case BYTE_CODE_LOGIC_AND:   		return "&&"; // &&
			case BYTE_CODE_LOGIC_OR:    		return "||";  // ||
			case BYTE_CODE_DIV:         		return "/"; // /
			case BYTE_CODE_MUL:         		return "*"; // *
			case BYTE_CODE_MOD:         		return "%";  // %
			case BYTE_CODE_BITWISE_AND:         return "&"; // bitwise logic and
			case BYTE_CODE_BITWISE_OR:         	return "|"; // bitwise logic or
			case BYTE_CODE_BITWISE_XOR:      	return "^"; // logic xor
			case BYTE_CODE_SHL:         		return ">>"; // shift left
			case BYTE_CODE_SHR:         		return "<<"; // shift right
			case BYTE_CODE_STORE:       		return "="; // =
			case BYTE_CODE_STORE_ADD:       	return "+="; // +=
			case BYTE_CODE_STORE_SUB:       	return "-="; // -=
			case BYTE_CODE_STORE_MUL:       	return "*="; // *=
			case BYTE_CODE_STORE_DIV:       	return "/="; // /=
			case BYTE_CODE_STORE_MOD:       	return "%="; // %=
			case BYTE_CODE_STORE_BITWISE_AND:   return "&="; // &=
			case BYTE_CODE_STORE_BITWISE_OR: 	return "|="; // |=
			case BYTE_CODE_STORE_BITWISE_XOR: 	return "^="; // ^=
			case BYTE_CODE_STORE_SHL: 	      	return ">>="; // <<=
			case BYTE_CODE_STORE_SHR: 	      	return "<<="; // >>=
			case BYTE_CODE_LOAD_GLOBAL:			return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:			return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_THIS:			return "LOAD_THIS"; // load type var
			case BYTE_CODE_LOAD_MEMBER_VAR:			return "LOAD_MEMBER"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      	return "LOAD_???"; // load to find global
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:	return "LOAD_EVEC"; // load element vector
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:	return "LOAD_EOBJ"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:		return "LOAD_FUNCT"; // load function
			case BYTE_CODE_LOAD_NULL:		return "LOAD_NULL"; // load undfined
			case BYTE_CODE_LOAD_STACK_ELEMENT:	return "LOAD_STK"; // load stack element
			case BYTE_CODE_LOAD_STRING:			return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_FLOAT:			return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:			return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_ZS_INT:			return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_CLASS:			return "LOAD_CLASS"; // load to find
			case BYTE_CODE_JMP:         		return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         		return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          		return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE:					return "JE"; // goto if equal ... goes end to conditional.
			case BYTE_CODE_CALL: 				return "CALL"; // calling function after all of arguments are processed...
			case BYTE_CODE_NEW_VECTOR: 			return "NEW_VEC"; // Vector object (CREATE)
			case BYTE_CODE_PUSH_VECTOR_ELEMENT: return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 				return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_CLASS: 			return "NEW_CLASS"; // New object (CREATE)
			case BYTE_CODE_DELETE:				return "DELETE";
			case BYTE_CODE_POP_SCOPE: 			return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 			return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:	return "PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT:		return "NEW_OBJ"; // New object (CREATE)
			case BYTE_CODE_IT_END: 				return "IT_END"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_IT_NEXT:				return "IT_NEXT"; // BYTE_CODE_SET_AND_NEXT
			case BYTE_CODE_STORE_CONST:			return "STORE_CONST"; // BYTE_CODE_STORE_CONST
			case BYTE_CODE_PRE_INC:				return "PRE_INC"; // ++i
			case BYTE_CODE_PRE_DEC:				return "PRE_DEC"; // --i
			case BYTE_CODE_POST_INC:			return "POST_INC"; // i++
			case BYTE_CODE_NEG_POST_INC:		return "NEG_POST_INC"; // -i++
			case BYTE_CODE_POST_DEC:			return "POST_DEC"; // i--
			case BYTE_CODE_NEG_POST_DEC:		return "NEG_POST_DEC"; // -i--
			case BYTE_CODE_RESET_STACK:			return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION

			default:
				break;
		}
		return "unknow_op";
	}

	const char * byte_code_metamethod_to_operator_str(ByteCodeMetamethod op){
		switch (op) {
			case	BYTE_CODE_METAMETHOD_EQU:		return  "==";  		// ==
			case	BYTE_CODE_METAMETHOD_NOT_EQU: 	return  "!=";  		// !=,
			case	BYTE_CODE_METAMETHOD_LT:		return  "<";  		// <
			case	BYTE_CODE_METAMETHOD_LTE:		return  "<=";  		// <=
			case	BYTE_CODE_METAMETHOD_NOT:		return  "!"; 		// !
			case	BYTE_CODE_METAMETHOD_GT:		return  ">";  		// >
			case	BYTE_CODE_METAMETHOD_GTE:		return  ">="; 		// >=
			case	BYTE_CODE_METAMETHOD_NEG:		return  "-"; 		// -a, !a
			case	BYTE_CODE_METAMETHOD_ADD:		return  "+"; 		// +
			case	BYTE_CODE_METAMETHOD_SUB:		return  "-"; 		// -
			case	BYTE_CODE_METAMETHOD_DIV:		return  "/"; 		// /
			case	BYTE_CODE_METAMETHOD_MUL:		return  "*"; 		// *
			case	BYTE_CODE_METAMETHOD_MOD:		return  "%";  		// %
			case	BYTE_CODE_METAMETHOD_AND:		return  "&"; 		// binary and
			case	BYTE_CODE_METAMETHOD_OR:		return  "|";		// binary or
			case	BYTE_CODE_METAMETHOD_XOR:		return  "^"; 		// binary xor
			case	BYTE_CODE_METAMETHOD_SHL:		return  "<<"; 		// binary shift left
			case	BYTE_CODE_METAMETHOD_SHR:		return  ">>"; 		// binary shift right
			case	BYTE_CODE_METAMETHOD_SET:		return  "=";		// _set
			//case	BYTE_CODE_METAMETHOD_GET:		return  "_get";		// _get
			case	BYTE_CODE_METAMETHOD_TO_STRING: return  "_toString";// _toString
			case 	BYTE_CODE_METAMETHOD_NEXT: 		return  "++";	// _next
			case 	BYTE_CODE_METAMETHOD_PREVIOUS: 	return  "--";// _previous
			default:
				return "none";
		}
	}

	const char * byte_code_metamethod_to_symbol_str(ByteCodeMetamethod op){

		switch (op) {
			case	BYTE_CODE_METAMETHOD_EQU:		return  "_equ"; 	// ==
			case	BYTE_CODE_METAMETHOD_NOT_EQU: 	return  "_nequ";  	// !=,
			case	BYTE_CODE_METAMETHOD_LT:		return  "_lt";  	// <
			case	BYTE_CODE_METAMETHOD_LTE:		return  "_lte";  	// <=
			case	BYTE_CODE_METAMETHOD_NOT:		return  "_not"; 	// !
			case	BYTE_CODE_METAMETHOD_GT:		return  "_gt";  	// >
			case	BYTE_CODE_METAMETHOD_GTE:		return  "_gte"; 	// >=
			case	BYTE_CODE_METAMETHOD_NEG:		return  "_neg"; 	// -a, !a
			case	BYTE_CODE_METAMETHOD_ADD:		return  "_add"; 	// +
			case	BYTE_CODE_METAMETHOD_SUB:		return  "_sub"; 	// -
			case	BYTE_CODE_METAMETHOD_DIV:		return  "_div"; 	// /
			case	BYTE_CODE_METAMETHOD_MUL:		return  "_mul"; 	// *
			case	BYTE_CODE_METAMETHOD_MOD:		return  "_mod";  	// %
			case	BYTE_CODE_METAMETHOD_AND:		return  "_and"; 	// binary and
			case	BYTE_CODE_METAMETHOD_OR:		return  "_or"; 		// binary or
			case	BYTE_CODE_METAMETHOD_XOR:		return  "_xor"; 	// binary xor
			case	BYTE_CODE_METAMETHOD_SHL:		return  "_shl"; 	// binary shift left
			case	BYTE_CODE_METAMETHOD_SHR:		return  "_shr"; 	// binary shift right
			case	BYTE_CODE_METAMETHOD_SET:		return  "_set"; 	// _set
			//case	BYTE_CODE_METAMETHOD_GET:		return  "_get"; 	// _get
			case	BYTE_CODE_METAMETHOD_TO_STRING:	return  "_toString";// _toString
			case 	BYTE_CODE_METAMETHOD_NEXT: 		return  "_next";	// _next
			case 	BYTE_CODE_METAMETHOD_PREVIOUS:	return  "_previous";// _previous
			default:
				return "none";
		}

		return "none";
	}

	int			 byte_code_get_num_arguments_static_metamethod(ByteCodeMetamethod op){
		switch(op){
			case BYTE_CODE_METAMETHOD_NOT:
			case BYTE_CODE_METAMETHOD_NEG:
			case BYTE_CODE_METAMETHOD_SET:
			case BYTE_CODE_METAMETHOD_TO_STRING:
			case BYTE_CODE_METAMETHOD_NEXT:
			case BYTE_CODE_METAMETHOD_PREVIOUS:
				return 1;
		}

		return 2;
	}

	int			 get_num_arguments_non_static_metamethod(ByteCodeMetamethod op){
		return byte_code_get_num_arguments_static_metamethod(op)-1;
	}


	ByteCode			 byte_code_load_to_push_stk(ByteCode byte_code){
	// load ptr var
		switch(byte_code){
			// load var content
			case BYTE_CODE_LOAD_GLOBAL:return BYTE_CODE_PUSH_STK_GLOBAL;
			case BYTE_CODE_LOAD_LOCAL:return BYTE_CODE_PUSH_STK_LOCAL;
			case BYTE_CODE_LOAD_REF:return BYTE_CODE_LOAD_REF; // PUSH STK NOT EXIST, IS A REF ITSELF
			case BYTE_CODE_LOAD_THIS:return BYTE_CODE_PUSH_STK_THIS;
			case BYTE_CODE_LOAD_MEMBER_VAR:return BYTE_CODE_PUSH_STK_MEMBER_VAR;
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:return BYTE_CODE_PUSH_STK_ELEMENT_VECTOR;
			case BYTE_CODE_LOAD_ELEMENT_THIS:return BYTE_CODE_PUSH_STK_ELEMENT_THIS;
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:return BYTE_CODE_PUSH_STK_ELEMENT_OBJECT;
			default:
				THROW_RUNTIME_ERROR("internal: op code not type load");
				break;
		}

		return byte_code;

	}

	bool		 byte_code_is_load_type(ByteCode byte_code){
		return byte_code==BYTE_CODE_LOAD_GLOBAL
				|| byte_code==BYTE_CODE_LOAD_LOCAL
				|| byte_code==BYTE_CODE_LOAD_REF
				|| byte_code==BYTE_CODE_LOAD_THIS
				|| byte_code==BYTE_CODE_LOAD_MEMBER_VAR
				|| byte_code==BYTE_CODE_LOAD_ELEMENT_VECTOR
				|| byte_code==BYTE_CODE_LOAD_ELEMENT_THIS
				|| byte_code==BYTE_CODE_LOAD_ELEMENT_OBJECT;
	}

}
