/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	const char * byte_code_to_str(ByteCode _byte_code){
		switch(_byte_code){
			case ZS_BYTE_CODE_EQU:        							return "EQU";  // ==
			case ZS_BYTE_CODE_INSTANCEOF:  						return "INSTANCEOF";  // ==
			case ZS_BYTE_CODE_NOT_EQU:     						return "NOT_EQU" ;  // !=
			case ZS_BYTE_CODE_LT:          						return "LT";  // <
			case ZS_BYTE_CODE_LTE:         						return "LTE";  // <=
			case ZS_BYTE_CODE_NOT:         						return "NOT"; // !
			case ZS_BYTE_CODE_GT:          						return "GT";  // >
			case ZS_BYTE_CODE_GTE:        							return "GTE"; // >=
			case ZS_BYTE_CODE_NEG:         						return "NEG"; // !
			case ZS_BYTE_CODE_BWC:					return "BWC"; // ~
			case ZS_BYTE_CODE_ADD:         						return "ADD"; // +
			case ZS_BYTE_CODE_SUB:         						return "SUB"; // -
			case ZS_BYTE_CODE_LOGIC_AND:   						return "LOGIC_AND"; // &&
			case ZS_BYTE_CODE_LOGIC_OR:    						return "LOGIC_OR";  // ||
			case ZS_BYTE_CODE_DIV:         						return "DIV"; // /
			case ZS_BYTE_CODE_MUL:         						return "MUL"; // *
			case ZS_BYTE_CODE_MOD:         						return "MOD";  // %
			case ZS_BYTE_CODE_BITWISE_AND:         				return "AND"; // bitwise logic and
			case ZS_BYTE_CODE_BITWISE_OR:         					return "OR"; // bitwise logic or
			case ZS_BYTE_CODE_BITWISE_XOR:         				return "XOR"; // logic xor
			case ZS_BYTE_CODE_SHL:         						return "SHL"; // shift left
			case ZS_BYTE_CODE_SHR:         						return "SHR"; // shift right
			case ZS_BYTE_CODE_STORE:       						return "STORE"; // =
			case ZS_BYTE_CODE_ADD_STORE:       					return "STORE_ADD"; // +=
			case ZS_BYTE_CODE_SUB_STORE:       					return "STORE_SUB"; // -=
			case ZS_BYTE_CODE_MUL_STORE:       					return "STORE_MUL"; // *=
			case ZS_BYTE_CODE_DIV_STORE:       					return "STORE_DIV"; // /=
			case ZS_BYTE_CODE_MOD_STORE:       					return "STORE_MOD"; // %=
			case ZS_BYTE_CODE_BITWISE_AND_STORE:      				return "STORE_AND"; // &=
			case ZS_BYTE_CODE_BITWISE_OR_STORE: 	   				return "STORE_OR"; // |=
			case ZS_BYTE_CODE_BITWISE_XOR_STORE: 	   				return "STORE_XOR"; // ^=
			case ZS_BYTE_CODE_SHL_STORE: 	      					return "STORE_SHL"; // <<=
			case ZS_BYTE_CODE_SHR_STORE: 	      					return "STORE_SHR"; // >>=
			case ZS_BYTE_CODE_PUSH_STK_GLOBAL_IRGO:				return "PUSH_STK_GLOBAL_IRGO"; // load type var
			case ZS_BYTE_CODE_PUSH_STK_GLOBAL:						return "PUSH_STK_GLOBAL"; // push stk global
			case ZS_BYTE_CODE_PUSH_STK_LOCAL:						return "PUSH_STK_LOCAL"; // push stk global
			//case ZS_BYTE_CODE_PUSH_STK_REF:						return "PUSH_STK_REF"; // push stk global
			case ZS_BYTE_CODE_PUSH_STK_THIS:						return "PUSH_STK_THIS"; // push stk global
			case ZS_BYTE_CODE_PUSH_STK_ARRAY_ITEM:				return "PUSH_STK_ARRAY@ITEM"; // load element vector
			case ZS_BYTE_CODE_PUSH_STK_OBJECT_ITEM:				return "PUSH_STK_OBJ@ITEM"; // load element object
			case ZS_BYTE_CODE_PUSH_STK_THIS_VARIABLE:				return "PUSH_STK_THIS@VAR"; // load element object

			case ZS_BYTE_CODE_LOAD_GLOBAL:							return "LOAD_GLOBAL"; // load type var
			case ZS_BYTE_CODE_LOAD_LOCAL:							return "LOAD_LOCAL"; // load type var
			case ZS_BYTE_CODE_LOAD_REF:							return "LOAD_REF"; // load type var
			case ZS_BYTE_CODE_LOAD_THIS:							return "LOAD_THIS"; // load type var
			case ZS_BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:				return "LOAD_CONSTRUCTOR_FUNCT"; // load type var
			case ZS_BYTE_CODE_FIND_VARIABLE:      					return "LOAD_???"; // load to find global
			// CALL
			case ZS_BYTE_CODE_INDIRECT_LOCAL_CALL:
			case ZS_BYTE_CODE_INDIRECT_GLOBAL_CALL:
			case ZS_BYTE_CODE_SUPER_CALL:
			case ZS_BYTE_CODE_THIS_CALL:
			case ZS_BYTE_CODE_INDIRECT_THIS_CALL:
			case ZS_BYTE_CODE_CALL:								return "CALL"; // direct call
			case ZS_BYTE_CODE_UNRESOLVED_CALL:						return "CALL???"; // direct call
			case ZS_BYTE_CODE_STACK_CALL:							return "STK_CALL"; // stack call


			case ZS_BYTE_CODE_MEMBER_CALL:							return "MEMBER_CALL"; // this indirect call through dynamic symbol
			case ZS_BYTE_CODE_LOAD_THIS_VARIABLE:					return "LOAD_THIS@VAR"; // load element object
			case ZS_BYTE_CODE_LOAD_THIS_FUNCTION:					return "LOAD_THIS@FUN"; // load element object
			case ZS_BYTE_CODE_LOAD_ARRAY_ITEM:						return "LOAD_ARRAY@ITEM"; // load element vector
			case ZS_BYTE_CODE_LOAD_OBJECT_ITEM:					return "LOAD_OBJ@ITEM"; // load element object
			case ZS_BYTE_CODE_LOAD_FUNCTION:						return "LOAD_FUN"; // load function
			case ZS_BYTE_CODE_LOAD_UNDEFINED:						return "LOAD_UNDEFINED"; // load undfined
			case ZS_BYTE_CODE_LOAD_NULL:							return "LOAD_NULL"; // load null
			case ZS_BYTE_CODE_LOAD_STACK_ELEMENT:					return "LOAD_STK"; // load stack element

			case ZS_BYTE_CODE_LOAD_STRING:							return "LOAD_STR"; // load string
			case ZS_BYTE_CODE_LOAD_FLOAT:						return "LOAD_FLT"; // load float
			case ZS_BYTE_CODE_LOAD_BOOL:							return "LOAD_BOOL"; // load bool
			case ZS_BYTE_CODE_LOAD_INT:							return "LOAD_INT"; // load zs_int
			case ZS_BYTE_CODE_LOAD_TYPE:							return "LOAD_TYPE"; // load to find
			case ZS_BYTE_CODE_JMP:         						return "JMP"; // Unconditional jump.
			case ZS_BYTE_CODE_JMP_CASE:         					return "JMP_CASE"; // Last jmp case
			case ZS_BYTE_CODE_JNT:         						return "JNT"; // goto if not true ... goes end to conditional.
			case ZS_BYTE_CODE_JT:          						return "JT"; // goto if true ... goes end to conditional.
			case ZS_BYTE_CODE_JE_CASE:								return "JE_CASE"; // je case equal ... goes end to conditional.
			case ZS_BYTE_CODE_CONSTRUCTOR_CALL: 					return "CALL_CONSTRUCTOR"; // calling function after all of arguments are processed...
			case ZS_BYTE_CODE_NEW_ARRAY: 							return "NEW_ARRAY"; // Array object (CREATE)
			case ZS_BYTE_CODE_PUSH_ARRAY_ITEM: 					return "PUSH_AITEM"; // Value push for stdvector
			case ZS_BYTE_CODE_RET: 								return "RET"; // Value pop for stdvector
			case ZS_BYTE_CODE_NEW_OBJECT_BY_TYPE:					return "NEW_OBJECT_BY_TYPE"; // New object (CREATE)
			case ZS_BYTE_CODE_NEW_OBJECT_BY_VALUE:					return "NEW_OBJECT_BY_VALUE"; // New object (CREATE)
			case ZS_BYTE_CODE_DELETE:								return "DELETE";
			case ZS_BYTE_CODE_POP_SCOPE: 							return "POP_SCOPE"; // New object (CREATE)
			case ZS_BYTE_CODE_PUSH_SCOPE: 							return "PUSH_SCOPE"; // New object (CREATE)
			case ZS_BYTE_CODE_PUSH_OBJECT_ITEM:					return "PUSH_OITEM"; // New object (CREATE)
			case ZS_BYTE_CODE_NEW_OBJECT:							return "NEW_OBJECT"; // New object (CREATE)
			case ZS_BYTE_CODE_NEW_STRING:							return "NEW_STR";
			case ZS_BYTE_CODE_IT_INIT: 							return "IT_INIT"; // ZS_BYTE_CODE_IT_CHK_END
			case ZS_BYTE_CODE_STORE_CONST:							return "STORE_CONST"; // ZS_BYTE_CODE_STORE_CONST
			case ZS_BYTE_CODE_PRE_INC:								return "PRE_INC"; // ++i
			case ZS_BYTE_CODE_PRE_DEC:								return "PRE_DEC"; // --i
			case ZS_BYTE_CODE_POST_INC:							return "POST_INC"; // i++
			//case ZS_BYTE_CODE_NEG_POST_INC:						return "NEG_POST_INC"; // -i++
			//case ZS_BYTE_CODE_BWC_POST_INC:						return "BWC_POST_INC"; // ~i++
			case ZS_BYTE_CODE_POST_DEC:							return "POST_DEC"; // i--
			//case ZS_BYTE_CODE_BWC_POST_DEC:						return "BWC_POST_DEC"; // ~i--
			case ZS_BYTE_CODE_RESET_STACK:							return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case ZS_BYTE_CODE_CLEAR_ZERO_POINTERS:					return "CLEAR_ZERO_POINTERS";
			case ZS_BYTE_CODE_TYPEOF:								return "TYPEOF"; // POP ONE AND IT MARKS END EXPRESSION
			case ZS_BYTE_CODE_IN:									return "IN"; // POP ONE AND IT MARKS END EXPRESSION


			default:
				break;
		}
		return "unknow_op";
	}

	const char * byte_code_to_operator_str(ByteCode _byte_code){
		switch(_byte_code){
			case ZS_BYTE_CODE_EQU:        				return "==";  // ==
			case ZS_BYTE_CODE_INSTANCEOF:  			return "instanceof";  // ==
			case ZS_BYTE_CODE_NOT_EQU:     			return "!=" ;  // !=
			case ZS_BYTE_CODE_LT:          			return "<";  // <
			case ZS_BYTE_CODE_LTE:         			return "<=";  // <=
			case ZS_BYTE_CODE_NOT:         			return "!"; // !
			case ZS_BYTE_CODE_GT:          			return ">";  // >
			case ZS_BYTE_CODE_GTE:        				return ">="; // >=
			case ZS_BYTE_CODE_NEG:         			return "-"; // -
			case ZS_BYTE_CODE_BWC:						return "~"; // ~
			case ZS_BYTE_CODE_ADD:         			return "+"; // +
			case ZS_BYTE_CODE_SUB:         			return "-"; // -
			case ZS_BYTE_CODE_LOGIC_AND:   			return "&&"; // &&
			case ZS_BYTE_CODE_LOGIC_OR:    			return "||";  // ||
			case ZS_BYTE_CODE_DIV:         			return "/"; // /
			case ZS_BYTE_CODE_MUL:         			return "*"; // *
			case ZS_BYTE_CODE_MOD:         			return "%";  // %
			case ZS_BYTE_CODE_BITWISE_AND:         	return "&"; // bitwise and
			case ZS_BYTE_CODE_BITWISE_OR:         		return "|"; // bitwise or
			case ZS_BYTE_CODE_BITWISE_XOR:      		return "^"; // bitwise xor
			case ZS_BYTE_CODE_SHL:         			return ">>"; // shift left
			case ZS_BYTE_CODE_SHR:         			return "<<"; // shift right
			case ZS_BYTE_CODE_STORE:       			return "="; // =
			case ZS_BYTE_CODE_ADD_STORE:       		return "+="; // +=
			case ZS_BYTE_CODE_SUB_STORE:       		return "-="; // -=
			case ZS_BYTE_CODE_MUL_STORE:       		return "*="; // *=
			case ZS_BYTE_CODE_DIV_STORE:       		return "/="; // /=
			case ZS_BYTE_CODE_MOD_STORE:       		return "%="; // %=
			case ZS_BYTE_CODE_BITWISE_AND_STORE:   	return "&="; // &=
			case ZS_BYTE_CODE_BITWISE_OR_STORE: 		return "|="; // |=
			case ZS_BYTE_CODE_BITWISE_XOR_STORE: 		return "^="; // ^=
			case ZS_BYTE_CODE_SHL_STORE: 	      		return ">>="; // <<=
			case ZS_BYTE_CODE_SHR_STORE: 	      		return "<<="; // >>=
			case ZS_BYTE_CODE_LOAD_GLOBAL:				return "LOAD_GLOBAL"; // load type var
			case ZS_BYTE_CODE_LOAD_LOCAL:				return "LOAD_LOCAL"; // load type var
			case ZS_BYTE_CODE_LOAD_THIS:				return "LOAD_THIS"; // load type var
			case ZS_BYTE_CODE_FIND_VARIABLE:      		return "LOAD_???"; // load to find global
			case ZS_BYTE_CODE_LOAD_ARRAY_ITEM:		return "LOAD_EVEC"; // load element vector
			case ZS_BYTE_CODE_LOAD_OBJECT_ITEM:		return "LOAD_EOBJ"; // load element object
			case ZS_BYTE_CODE_LOAD_FUNCTION:			return "LOAD_FUNCT"; // load function
			case ZS_BYTE_CODE_LOAD_UNDEFINED:			return "LOAD_UNDEFINED"; // load undefined
			case ZS_BYTE_CODE_LOAD_NULL:				return "LOAD_NULL"; // load null
			case ZS_BYTE_CODE_LOAD_STACK_ELEMENT:		return "LOAD_STK"; // load stack element
			case ZS_BYTE_CODE_LOAD_STRING:				return "LOAD_STR"; // load string
			case ZS_BYTE_CODE_LOAD_FLOAT:			return "LOAD_FLT"; // load float
			case ZS_BYTE_CODE_LOAD_BOOL:				return "LOAD_BOOL"; // load bool
			case ZS_BYTE_CODE_LOAD_INT:				return "LOAD_INT"; // load zs_int
			case ZS_BYTE_CODE_LOAD_TYPE:				return "LOAD_TYPE"; // load to find
			case ZS_BYTE_CODE_JMP:         			return "JMP"; // Unconditional jump.
			case ZS_BYTE_CODE_JNT:         			return "JNT"; // goto if not true ... goes end to conditional.
			case ZS_BYTE_CODE_JT:          			return "JT"; // goto if true ... goes end to conditional.
			case ZS_BYTE_CODE_JE_CASE:					return "JE"; // goto if equal ... goes end to conditional.
			case ZS_BYTE_CODE_JMP_CASE:       			return "JMP_CASE"; // Last jmp case
			case ZS_BYTE_CODE_NEW_ARRAY: 				return "NEW_VEC"; // Array object (CREATE)
			case ZS_BYTE_CODE_PUSH_ARRAY_ITEM: 		return "VPUSH"; // Value push for stdvector
			case ZS_BYTE_CODE_RET: 					return "RET"; // Value pop for stdvector
			case ZS_BYTE_CODE_NEW_OBJECT_BY_TYPE:		return "NEW_OBJECT_TYPE"; // New object (CREATE)
			case ZS_BYTE_CODE_DELETE:					return "DELETE";
			case ZS_BYTE_CODE_POP_SCOPE: 				return "POP_SCOPE"; // New object (CREATE)
			case ZS_BYTE_CODE_PUSH_SCOPE: 				return "PUSH_SCOPE"; // New object (CREATE)
			case ZS_BYTE_CODE_PUSH_OBJECT_ITEM:		return "PUSH_ATTR"; // New object (CREATE)
			case ZS_BYTE_CODE_NEW_OBJECT:				return "NEW_OBJ"; // New object (CREATE)
			case ZS_BYTE_CODE_IT_INIT: 				return "IT_INIT"; // ZS_BYTE_CODE_IT_CHK_END
			case ZS_BYTE_CODE_STORE_CONST:				return "STORE_CONST"; // ZS_BYTE_CODE_STORE_CONST
			case ZS_BYTE_CODE_PRE_INC:					return "PRE_INC"; // ++i
			case ZS_BYTE_CODE_PRE_DEC:					return "PRE_DEC"; // --i
			case ZS_BYTE_CODE_POST_INC:				return "POST_INC"; // i++
			//case ZS_BYTE_CODE_NEG_POST_INC:			return "NEG_POST_INC"; // -i++
			//case ZS_BYTE_CODE_BWC_POST_INC:			return "BWC_POST_INC"; // ~i++
			case ZS_BYTE_CODE_POST_DEC:				return "POST_DEC"; // i--
			//case ZS_BYTE_CODE_NEG_POST_DEC:			return "NEG_POST_DEC"; // -i--
			//case ZS_BYTE_CODE_BWC_POST_DEC:			return "BWC_POST_DEC"; // ~i--
			case ZS_BYTE_CODE_RESET_STACK:				return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case ZS_BYTE_CODE_CLEAR_ZERO_POINTERS:		return "CLEAR_ZERO_POINTERS"; // POP ONE AND IT MARKS END EXPRESSION
			case ZS_BYTE_CODE_TYPEOF:					return "typeof"; // POP ONE AND IT MARKS END EXPRESSION
			case ZS_BYTE_CODE_IN:						return "in"; // POP ONE AND IT MARKS END EXPRESSION

			default:
				break;
		}
		return "unknow_op";
	}

	ByteCode			 byte_code_load_var_type_to_push_stk(ByteCode byte_code){
	// load ptr var
		switch(byte_code){
			// load var content
		default:
			break;
			case ZS_BYTE_CODE_LOAD_GLOBAL:return ZS_BYTE_CODE_PUSH_STK_GLOBAL;
			case ZS_BYTE_CODE_LOAD_LOCAL:return ZS_BYTE_CODE_PUSH_STK_LOCAL;
			case ZS_BYTE_CODE_LOAD_REF:return ZS_BYTE_CODE_LOAD_REF; // PUSH STK NOT EXIST, IS A REF ITSELF
			case ZS_BYTE_CODE_LOAD_THIS:return ZS_BYTE_CODE_PUSH_STK_THIS;
			case ZS_BYTE_CODE_LOAD_ARRAY_ITEM:return ZS_BYTE_CODE_PUSH_STK_ARRAY_ITEM;
			case ZS_BYTE_CODE_LOAD_THIS_VARIABLE:return ZS_BYTE_CODE_PUSH_STK_THIS_VARIABLE;
			case ZS_BYTE_CODE_LOAD_OBJECT_ITEM:return ZS_BYTE_CODE_PUSH_STK_OBJECT_ITEM;
		}

		//ZS_THROW_RUNTIME_ERRORF("internal: op code not type load");
		return ZS_BYTE_CODE_INVALID;

	}

	bool		 byte_code_is_load_var_type(ByteCode byte_code){
		return byte_code==ZS_BYTE_CODE_LOAD_GLOBAL
				|| byte_code==ZS_BYTE_CODE_LOAD_LOCAL
				|| byte_code==ZS_BYTE_CODE_LOAD_REF
				|| byte_code==ZS_BYTE_CODE_LOAD_THIS
				|| byte_code==ZS_BYTE_CODE_LOAD_ARRAY_ITEM
				|| byte_code==ZS_BYTE_CODE_LOAD_THIS_VARIABLE
				|| byte_code==ZS_BYTE_CODE_LOAD_OBJECT_ITEM;
	}
}
