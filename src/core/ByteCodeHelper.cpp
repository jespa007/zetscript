/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	const char * ByteCodeHelper::getByteCodeName(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        						return "EQU";  // ==
			case BYTE_CODE_INSTANCEOF:  					return "INSTANCEOF";  // ==
			case BYTE_CODE_NOT_EQU:     					return "NOT_EQU" ;  // !=
			case BYTE_CODE_LT:          					return "LT";  // <
			case BYTE_CODE_LTE:         					return "LTE";  // <=
			case BYTE_CODE_NOT:         					return "NOT"; // !
			case BYTE_CODE_GT:          					return "GT";  // >
			case BYTE_CODE_GTE:        						return "GTE"; // >=
			case BYTE_CODE_NEG:         					return "NEG"; // !
			case BYTE_CODE_BWC:								return "BWC"; // ~
			case BYTE_CODE_ADD:         					return "ADD"; // +
			case BYTE_CODE_SUB:         					return "SUB"; // -
			case BYTE_CODE_LOGIC_AND:   					return "LOGIC_AND"; // &&
			case BYTE_CODE_LOGIC_OR:    					return "LOGIC_OR";  // ||
			case BYTE_CODE_DIV:         					return "DIV"; // /
			case BYTE_CODE_MUL:         					return "MUL"; // *
			case BYTE_CODE_MOD:         					return "MOD";  // %
			case BYTE_CODE_BITWISE_AND:         			return "AND"; // bitwise logic and
			case BYTE_CODE_BITWISE_OR:         				return "OR"; // bitwise logic or
			case BYTE_CODE_BITWISE_XOR:         			return "XOR"; // logic xor
			case BYTE_CODE_SHL:         					return "SHL"; // shift left
			case BYTE_CODE_SHR:         					return "SHR"; // shift right
			case BYTE_CODE_STORE:       					return "STORE"; // =
			case BYTE_CODE_ADD_ASSIGN:       				return "ADD_ASSIGN"; // +=
			case BYTE_CODE_SUB_ASSIGN:       				return "SUB_ASSIGN"; // -=
			case BYTE_CODE_MUL_ASSIGN:       				return "MUL_ASSIGN"; // *=
			case BYTE_CODE_DIV_ASSIGN:       				return "DIV_ASSIGN"; // /=
			case BYTE_CODE_MOD_ASSIGN:       				return "MOD_ASSIGN"; // %=
			case BYTE_CODE_BITWISE_AND_ASSIGN:      		return "AND_ASSIGN"; // &=
			case BYTE_CODE_BITWISE_OR_ASSIGN: 	   			return "OR_ASSIGN"; // |=
			case BYTE_CODE_BITWISE_XOR_ASSIGN: 	   			return "XOR_ASSIGN"; // ^=
			case BYTE_CODE_SHL_ASSIGN: 	      				return "SHL_ASSIGN"; // <<=
			case BYTE_CODE_SHR_ASSIGN: 	      				return "SHR_ASSIGN"; // >>=
			case BYTE_CODE_PUSH_STK_GLOBAL_IRGO:			return "PUSH_STK_GLOBAL_IRGO"; // load type var
			case BYTE_CODE_PUSH_STK_GLOBAL:					return "PUSH_STK_GLOBAL"; // push stk global
			case BYTE_CODE_PUSH_STK_LOCAL:					return "PUSH_STK_LOCAL"; // push stk global
			//case BYTE_CODE_PUSH_STK_REF:					return "PUSH_STK_REF"; // push stk global
			case BYTE_CODE_PUSH_STK_THIS:					return "PUSH_STK_THIS"; // push stk global
			case BYTE_CODE_PUSH_STK_ARRAY_ITEM:				return "PUSH_STK_ARRAY@ITEM"; // load element vector
			case BYTE_CODE_PUSH_STK_OBJECT_ITEM:			return "PUSH_STK_OBJ@ITEM"; // load element object
			case BYTE_CODE_PUSH_STK_THIS_VARIABLE:			return "PUSH_STK_THIS@VAR"; // load element object

			case BYTE_CODE_LOAD_GLOBAL:						return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:						return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_REF:						return "LOAD_REF"; // load type var
			case BYTE_CODE_LOAD_THIS:						return "LOAD_THIS"; // load type var
			case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:			return "LOAD_CONSTRUCTOR_FUNCT"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      				return "LOAD_???"; // load to find global
			// CALL
			case BYTE_CODE_INDIRECT_LOCAL_CALL:
			case BYTE_CODE_INDIRECT_GLOBAL_CALL:
			case BYTE_CODE_SUPER_CALL:
			case BYTE_CODE_THIS_CALL:
			case BYTE_CODE_INDIRECT_THIS_CALL:
			case BYTE_CODE_CALL:							return "CALL"; // direct call
			case BYTE_CODE_UNRESOLVED_CALL:					return "CALL???"; // direct call
			case BYTE_CODE_STACK_CALL:						return "STK_CALL"; // stack call


			case BYTE_CODE_MEMBER_CALL:						return "MEMBER_CALL"; // this indirect call through dynamic symbol
			case BYTE_CODE_LOAD_THIS_VARIABLE:				return "LOAD_THIS@VAR"; // load element object
			case BYTE_CODE_LOAD_THIS_FUNCTION:				return "LOAD_THIS@FUN"; // load element object
			case BYTE_CODE_LOAD_ARRAY_ITEM:					return "LOAD_ARRAY@ITEM"; // load element vector
			case BYTE_CODE_LOAD_OBJECT_ITEM:				return "LOAD_OBJ@ITEM"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:					return "LOAD_FUN"; // load function
			case BYTE_CODE_LOAD_UNDEFINED:					return "LOAD_UNDEFINED"; // load undfined
			case BYTE_CODE_LOAD_NULL:						return "LOAD_NULL"; // load null
			case BYTE_CODE_LOAD_STACK_ELEMENT:				return "LOAD_STK"; // load stack element

			case BYTE_CODE_LOAD_STRING:						return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_FLOAT:						return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:						return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_INT:						return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_TYPE:						return "LOAD_TYPE"; // load to find
			case BYTE_CODE_JMP:         					return "JMP"; // Unconditional jump.
			case BYTE_CODE_JMP_CASE:         				return "JMP_CASE"; // Last jmp case
			case BYTE_CODE_JNT:         					return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          					return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE_CASE:							return "JE_CASE"; // je case equal ... goes end to conditional.
			case BYTE_CODE_CONSTRUCTOR_CALL: 				return "CALL_CONSTRUCTOR"; // calling function after all of arguments are processed...
			case BYTE_CODE_NEW_ARRAY: 						return "NEW_ARRAY"; // Array object (CREATE)
			case BYTE_CODE_PUSH_ARRAY_ITEM: 				return "PUSH_AITEM"; // Value push for stdvector
			case BYTE_CODE_RET: 							return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_OBJECT_BY_TYPE:				return "NEW_OBJECT_BY_TYPE"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT_BY_VALUE:				return "NEW_OBJECT_BY_VALUE"; // New object (CREATE)
			case BYTE_CODE_DELETE:							return "DELETE";
			case BYTE_CODE_POP_SCOPE: 						return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 						return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ITEM:				return "PUSH_OITEM"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT:						return "NEW_OBJECT"; // New object (CREATE)
			case BYTE_CODE_NEW_STRING:						return "NEW_STR";
			case BYTE_CODE_IT_INIT: 						return "IT_INIT"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_STORE_CONST:						return "STORE_CONST"; // BYTE_CODE_STORE_CONST
			case BYTE_CODE_PRE_INC:							return "PRE_INC"; // ++i
			case BYTE_CODE_PRE_DEC:							return "PRE_DEC"; // --i
			case BYTE_CODE_POST_INC:						return "POST_INC"; // i++
			//case BYTE_CODE_NEG_POST_INC:					return "NEG_POST_INC"; // -i++
			//case BYTE_CODE_BWC_POST_INC:					return "BWC_POST_INC"; // ~i++
			case BYTE_CODE_POST_DEC:						return "POST_DEC"; // i--
			//case BYTE_CODE_BWC_POST_DEC:					return "BWC_POST_DEC"; // ~i--
			case BYTE_CODE_RESET_STACK:						return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case BYTE_CODE_CLEAR_ZERO_POINTERS:			return "CLEAR_ZERO_POINTERS";
			case BYTE_CODE_TYPEOF:							return "TYPEOF"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_IN:								return "IN"; // POP ONE AND IT MARKS END EXPRESSION


			default:
				break;
		}
		return "unknow_op";
	}

	const char * ByteCodeHelper::getByteCodeOperatorName(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        				return "==";  // ==
			case BYTE_CODE_INSTANCEOF:  			return "instanceof";  // ==
			case BYTE_CODE_NOT_EQU:     			return "!=" ;  // !=
			case BYTE_CODE_LT:          			return "<";  // <
			case BYTE_CODE_LTE:         			return "<=";  // <=
			case BYTE_CODE_NOT:         			return "!"; // !
			case BYTE_CODE_GT:          			return ">";  // >
			case BYTE_CODE_GTE:        				return ">="; // >=
			case BYTE_CODE_NEG:         			return "-"; // -
			case BYTE_CODE_BWC:						return "~"; // ~
			case BYTE_CODE_ADD:         			return "+"; // +
			case BYTE_CODE_SUB:         			return "-"; // -
			case BYTE_CODE_LOGIC_AND:   			return "&&"; // &&
			case BYTE_CODE_LOGIC_OR:    			return "||";  // ||
			case BYTE_CODE_DIV:         			return "/"; // /
			case BYTE_CODE_MUL:         			return "*"; // *
			case BYTE_CODE_MOD:         			return "%";  // %
			case BYTE_CODE_BITWISE_AND:         	return "&"; // bitwise and
			case BYTE_CODE_BITWISE_OR:         		return "|"; // bitwise or
			case BYTE_CODE_BITWISE_XOR:      		return "^"; // bitwise xor
			case BYTE_CODE_SHL:         			return ">>"; // shift left
			case BYTE_CODE_SHR:         			return "<<"; // shift right
			case BYTE_CODE_STORE:       			return "="; // =
			case BYTE_CODE_ADD_ASSIGN:       		return "+="; // +=
			case BYTE_CODE_SUB_ASSIGN:       		return "-="; // -=
			case BYTE_CODE_MUL_ASSIGN:       		return "*="; // *=
			case BYTE_CODE_DIV_ASSIGN:       		return "/="; // /=
			case BYTE_CODE_MOD_ASSIGN:       		return "%="; // %=
			case BYTE_CODE_BITWISE_AND_ASSIGN:   	return "&="; // &=
			case BYTE_CODE_BITWISE_OR_ASSIGN: 		return "|="; // |=
			case BYTE_CODE_BITWISE_XOR_ASSIGN: 		return "^="; // ^=
			case BYTE_CODE_SHL_ASSIGN: 	      		return ">>="; // <<=
			case BYTE_CODE_SHR_ASSIGN: 	      		return "<<="; // >>=
			case BYTE_CODE_LOAD_GLOBAL:				return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:				return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_THIS:				return "LOAD_THIS"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      		return "LOAD_???"; // load to find global
			case BYTE_CODE_LOAD_ARRAY_ITEM:			return "LOAD_EVEC"; // load element vector
			case BYTE_CODE_LOAD_OBJECT_ITEM:		return "LOAD_EOBJ"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:			return "LOAD_FUNCT"; // load function
			case BYTE_CODE_LOAD_UNDEFINED:			return "LOAD_UNDEFINED"; // load undefined
			case BYTE_CODE_LOAD_NULL:				return "LOAD_NULL"; // load null
			case BYTE_CODE_LOAD_STACK_ELEMENT:		return "LOAD_STK"; // load stack element
			case BYTE_CODE_LOAD_STRING:				return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_FLOAT:				return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:				return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_INT:				return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_TYPE:				return "LOAD_TYPE"; // load to find
			case BYTE_CODE_JMP:         			return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         			return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          			return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE_CASE:					return "JE"; // goto if equal ... goes end to conditional.
			case BYTE_CODE_JMP_CASE:       			return "JMP_CASE"; // Last jmp case
			case BYTE_CODE_NEW_ARRAY: 				return "NEW_VEC"; // Array object (CREATE)
			case BYTE_CODE_PUSH_ARRAY_ITEM: 		return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 					return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_OBJECT_BY_TYPE:		return "NEW_OBJECT_TYPE"; // New object (CREATE)
			case BYTE_CODE_DELETE:					return "DELETE";
			case BYTE_CODE_POP_SCOPE: 				return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 				return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ITEM:		return "PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT:				return "NEW_OBJ"; // New object (CREATE)
			case BYTE_CODE_IT_INIT: 				return "IT_INIT"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_STORE_CONST:				return "STORE_CONST"; // BYTE_CODE_STORE_CONST
			case BYTE_CODE_PRE_INC:					return "PRE_INC"; // ++i
			case BYTE_CODE_PRE_DEC:					return "PRE_DEC"; // --i
			case BYTE_CODE_POST_INC:				return "POST_INC"; // i++
			//case BYTE_CODE_NEG_POST_INC:			return "NEG_POST_INC"; // -i++
			//case BYTE_CODE_BWC_POST_INC:			return "BWC_POST_INC"; // ~i++
			case BYTE_CODE_POST_DEC:				return "POST_DEC"; // i--
			//case BYTE_CODE_NEG_POST_DEC:			return "NEG_POST_DEC"; // -i--
			//case BYTE_CODE_BWC_POST_DEC:			return "BWC_POST_DEC"; // ~i--
			case BYTE_CODE_RESET_STACK:				return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case BYTE_CODE_CLEAR_ZERO_POINTERS:		return "CLEAR_ZERO_POINTERS"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_TYPEOF:					return "typeof"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_IN:						return "in"; // POP ONE AND IT MARKS END EXPRESSION

			default:
				break;
		}
		return "unknow_op";
	}
}
