/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	const char * ByteCode::toString(ByteCodeId _byte_code){
		switch(_byte_code){
			case ByteCode::ByteCodeId::BYTE_CODE_ID_EQU:        						return "EQU";  // ==
			case ByteCode::ByteCodeId::BYTE_CODE_ID_INSTANCEOF:  						return "INSTANCEOF";  // ==
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT_EQU:     						return "NOT_EQU" ;  // !=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LT:          						return "LT";  // <
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LTE:         						return "LTE";  // <=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT:         						return "NOT"; // !
			case ByteCode::ByteCodeId::BYTE_CODE_ID_GT:          						return "GT";  // >
			case ByteCode::ByteCodeId::BYTE_CODE_ID_GTE:        						return "GTE"; // >=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG:         						return "NEG"; // !
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC:								return "BWC"; // ~
			case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD:         						return "ADD"; // +
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB:         						return "SUB"; // -
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_AND:   						return "LOGIC_AND"; // &&
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_OR:    						return "LOGIC_OR";  // ||
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV:         						return "DIV"; // /
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL:         						return "MUL"; // *
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD:         						return "MOD";  // %
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND:         				return "AND"; // bitwise logic and
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR:         				return "OR"; // bitwise logic or
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR:         				return "XOR"; // logic xor
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL:         						return "SHL"; // shift left
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR:         						return "SHR"; // shift right
			case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE:       						return "STORE"; // =
			case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD_ASSIGN:       						return "ADD_ASSIGN"; // +=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB_ASSIGN:       						return "SUB_ASSIGN"; // -=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL_ASSIGN:       						return "MUL_ASSIGN"; // *=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV_ASSIGN:       						return "DIV_ASSIGN"; // /=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD_ASSIGN:       						return "MOD_ASSIGN"; // %=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND_ASSIGN:      				return "AND_ASSIGN"; // &=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR_ASSIGN: 	   				return "OR_ASSIGN"; // |=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR_ASSIGN: 	   				return "XOR_ASSIGN"; // ^=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL_ASSIGN: 	      					return "SHL_ASSIGN"; // <<=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR_ASSIGN: 	      					return "SHR_ASSIGN"; // >>=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL_IRGO:				return "PUSH_STK_GLOBAL_IRGO"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL:						return "PUSH_STK_GLOBAL"; // push stk global
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_LOCAL:						return "PUSH_STK_LOCAL"; // push stk global
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_REF:						return "PUSH_STK_REF"; // push stk global
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS:						return "PUSH_STK_THIS"; // push stk global
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_ARRAY_ITEM:				return "PUSH_STK_ARRAY@ITEM"; // load element vector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_OBJECT_ITEM:				return "PUSH_STK_OBJ@ITEM"; // load element object
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS_VARIABLE:				return "PUSH_STK_THIS@VAR"; // load element object

			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_GLOBAL:							return "LOAD_GLOBAL"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL:							return "LOAD_LOCAL"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_REF:							return "LOAD_REF"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS:							return "LOAD_THIS"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_CONSTRUCTOR_FUNCT:				return "LOAD_CONSTRUCTOR_FUNCT"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE:      					return "LOAD_???"; // load to find global
			// CALL
			case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_LOCAL_CALL:
			case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_GLOBAL_CALL:
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SUPER_CALL:
			case ByteCode::ByteCodeId::BYTE_CODE_ID_THIS_CALL:
			case ByteCode::ByteCodeId::BYTE_CODE_ID_INDIRECT_THIS_CALL:
			case ByteCode::ByteCodeId::BYTE_CODE_ID_CALL:								return "CALL"; // direct call
			case ByteCode::ByteCodeId::BYTE_CODE_ID_UNRESOLVED_CALL:						return "CALL???"; // direct call
			case ByteCode::ByteCodeId::BYTE_CODE_ID_STACK_CALL:							return "STK_CALL"; // stack call


			case ByteCode::ByteCodeId::BYTE_CODE_ID_MEMBER_CALL:							return "MEMBER_CALL"; // this indirect call through dynamic symbol
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_VARIABLE:					return "LOAD_THIS@VAR"; // load element object
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_FUNCTION:					return "LOAD_THIS@FUN"; // load element object
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_ARRAY_ITEM:						return "LOAD_ARRAY@ITEM"; // load element vector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_OBJECT_ITEM:					return "LOAD_OBJ@ITEM"; // load element object
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FUNCTION:						return "LOAD_FUN"; // load function
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_UNDEFINED:						return "LOAD_UNDEFINED"; // load undfined
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_NULL:							return "LOAD_NULL"; // load null
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STACK_ELEMENT:					return "LOAD_STK"; // load stack element

			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING:							return "LOAD_STR"; // load string
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT:						return "LOAD_FLT"; // load float
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_BOOL:							return "LOAD_BOOL"; // load bool
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT:							return "LOAD_INT"; // load zs_int
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_TYPE:							return "LOAD_TYPE"; // load to find
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP:         						return "JMP"; // Unconditional jump.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP_CASE:         					return "JMP_CASE"; // Last jmp case
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JNT:         						return "JNT"; // goto if not true ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JT:          						return "JT"; // goto if true ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JE_CASE:								return "JE_CASE"; // je case equal ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_CONSTRUCTOR_CALL: 					return "CALL_CONSTRUCTOR"; // calling function after all of arguments are processed...
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_ARRAY: 							return "NEW_ARRAY"; // Array object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_ARRAY_ITEM: 					return "PUSH_AITEM"; // Value push for stdvector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_RET: 								return "RET"; // Value pop for stdvector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT_BY_TYPE:					return "NEW_OBJECT_BY_TYPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT_BY_VALUE:					return "NEW_OBJECT_BY_VALUE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DELETE:								return "DELETE";
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POP_SCOPE: 							return "POP_SCOPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_SCOPE: 							return "PUSH_SCOPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_OBJECT_ITEM:					return "PUSH_OITEM"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT:							return "NEW_OBJECT"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_STRING:							return "NEW_STR";
			case ByteCode::ByteCodeId::BYTE_CODE_ID_IT_INIT: 							return "IT_INIT"; // ByteCode::ByteCodeId::BYTE_CODE_ID_IT_CHK_END
			case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE_CONST:							return "STORE_CONST"; // ByteCode::ByteCodeId::BYTE_CODE_ID_STORE_CONST
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_INC:								return "PRE_INC"; // ++i
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_DEC:								return "PRE_DEC"; // --i
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_INC:							return "POST_INC"; // i++
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG_POST_INC:						return "NEG_POST_INC"; // -i++
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_INC:						return "BWC_POST_INC"; // ~i++
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_DEC:							return "POST_DEC"; // i--
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_DEC:						return "BWC_POST_DEC"; // ~i--
			case ByteCode::ByteCodeId::BYTE_CODE_ID_RESET_STACK:							return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_CLEAR_ZERO_POINTERS:					return "CLEAR_ZERO_POINTERS";
			case ByteCode::ByteCodeId::BYTE_CODE_ID_TYPEOF:								return "TYPEOF"; // POP ONE AND IT MARKS END EXPRESSION
			case ByteCode::ByteCodeId::BYTE_CODE_ID_IN:									return "IN"; // POP ONE AND IT MARKS END EXPRESSION


			default:
				break;
		}
		return "unknow_op";
	}

	const char * ByteCode::toOperatorString(ByteCodeId _byte_code){
		switch(_byte_code){
			case ByteCode::ByteCodeId::BYTE_CODE_ID_EQU:        				return "==";  // ==
			case ByteCode::ByteCodeId::BYTE_CODE_ID_INSTANCEOF:  			return "instanceof";  // ==
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT_EQU:     			return "!=" ;  // !=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LT:          			return "<";  // <
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LTE:         			return "<=";  // <=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NOT:         			return "!"; // !
			case ByteCode::ByteCodeId::BYTE_CODE_ID_GT:          			return ">";  // >
			case ByteCode::ByteCodeId::BYTE_CODE_ID_GTE:        				return ">="; // >=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG:         			return "-"; // -
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC:						return "~"; // ~
			case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD:         			return "+"; // +
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB:         			return "-"; // -
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_AND:   			return "&&"; // &&
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_OR:    			return "||";  // ||
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV:         			return "/"; // /
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL:         			return "*"; // *
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD:         			return "%";  // %
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND:         	return "&"; // bitwise and
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR:         		return "|"; // bitwise or
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR:      		return "^"; // bitwise xor
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL:         			return ">>"; // shift left
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR:         			return "<<"; // shift right
			case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE:       			return "="; // =
			case ByteCode::ByteCodeId::BYTE_CODE_ID_ADD_ASSIGN:       		return "+="; // +=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SUB_ASSIGN:       		return "-="; // -=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MUL_ASSIGN:       		return "*="; // *=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DIV_ASSIGN:       		return "/="; // /=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_MOD_ASSIGN:       		return "%="; // %=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND_ASSIGN:   	return "&="; // &=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR_ASSIGN: 		return "|="; // |=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR_ASSIGN: 		return "^="; // ^=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHL_ASSIGN: 	      		return ">>="; // <<=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_SHR_ASSIGN: 	      		return "<<="; // >>=
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_GLOBAL:				return "LOAD_GLOBAL"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL:				return "LOAD_LOCAL"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS:				return "LOAD_THIS"; // load type var
			case ByteCode::ByteCodeId::BYTE_CODE_ID_FIND_VARIABLE:      		return "LOAD_???"; // load to find global
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_ARRAY_ITEM:		return "LOAD_EVEC"; // load element vector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_OBJECT_ITEM:		return "LOAD_EOBJ"; // load element object
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FUNCTION:			return "LOAD_FUNCT"; // load function
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_UNDEFINED:			return "LOAD_UNDEFINED"; // load undefined
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_NULL:				return "LOAD_NULL"; // load null
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STACK_ELEMENT:		return "LOAD_STK"; // load stack element
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_STRING:				return "LOAD_STR"; // load string
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_FLOAT:			return "LOAD_FLT"; // load float
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_BOOL:				return "LOAD_BOOL"; // load bool
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_INT:				return "LOAD_INT"; // load zs_int
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_TYPE:				return "LOAD_TYPE"; // load to find
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP:         			return "JMP"; // Unconditional jump.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JNT:         			return "JNT"; // goto if not true ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JT:          			return "JT"; // goto if true ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JE_CASE:					return "JE"; // goto if equal ... goes end to conditional.
			case ByteCode::ByteCodeId::BYTE_CODE_ID_JMP_CASE:       			return "JMP_CASE"; // Last jmp case
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_ARRAY: 				return "NEW_VEC"; // Array object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_ARRAY_ITEM: 		return "VPUSH"; // Value push for stdvector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_RET: 					return "RET"; // Value pop for stdvector
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT_BY_TYPE:		return "NEW_OBJECT_TYPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_DELETE:					return "DELETE";
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POP_SCOPE: 				return "POP_SCOPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_SCOPE: 				return "PUSH_SCOPE"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_OBJECT_ITEM:		return "PUSH_ATTR"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_NEW_OBJECT:				return "NEW_OBJ"; // New object (CREATE)
			case ByteCode::ByteCodeId::BYTE_CODE_ID_IT_INIT: 				return "IT_INIT"; // ByteCode::ByteCodeId::BYTE_CODE_ID_IT_CHK_END
			case ByteCode::ByteCodeId::BYTE_CODE_ID_STORE_CONST:				return "STORE_CONST"; // ByteCode::ByteCodeId::BYTE_CODE_ID_STORE_CONST
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_INC:					return "PRE_INC"; // ++i
			case ByteCode::ByteCodeId::BYTE_CODE_ID_PRE_DEC:					return "PRE_DEC"; // --i
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_INC:				return "POST_INC"; // i++
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG_POST_INC:			return "NEG_POST_INC"; // -i++
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_INC:			return "BWC_POST_INC"; // ~i++
			case ByteCode::ByteCodeId::BYTE_CODE_ID_POST_DEC:				return "POST_DEC"; // i--
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_NEG_POST_DEC:			return "NEG_POST_DEC"; // -i--
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_BWC_POST_DEC:			return "BWC_POST_DEC"; // ~i--
			case ByteCode::ByteCodeId::BYTE_CODE_ID_RESET_STACK:				return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			//case ByteCode::ByteCodeId::BYTE_CODE_ID_CLEAR_ZERO_POINTERS:		return "CLEAR_ZERO_POINTERS"; // POP ONE AND IT MARKS END EXPRESSION
			case ByteCode::ByteCodeId::BYTE_CODE_ID_TYPEOF:					return "typeof"; // POP ONE AND IT MARKS END EXPRESSION
			case ByteCode::ByteCodeId::BYTE_CODE_ID_IN:						return "in"; // POP ONE AND IT MARKS END EXPRESSION

			default:
				break;
		}
		return "unknow_op";
	}

	ByteCode::ByteCodeId			 ByteCode::loadVarTypeToPushStk(ByteCodeId byte_code){
	// load ptr var
		switch(byte_code){
			// load var content
		default:
			break;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_GLOBAL:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_GLOBAL;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_LOCAL;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_REF:return ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_REF; // PUSH STK NOT EXIST, IS A REF ITSELF
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_ARRAY_ITEM:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_ARRAY_ITEM;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_VARIABLE:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_THIS_VARIABLE;
			case ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_OBJECT_ITEM:return ByteCode::ByteCodeId::BYTE_CODE_ID_PUSH_STK_OBJECT_ITEM;
		}

		//ZS_THROW_RUNTIME_ERRORF("internal: op code not type load");
		return ByteCode::ByteCodeId::BYTE_CODE_ID_INVALID;

	}

	bool		 ByteCode::isLoadVarType(ByteCodeId byte_code){
		return byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_GLOBAL
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_LOCAL
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_REF
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_ARRAY_ITEM
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_THIS_VARIABLE
				|| byte_code==ByteCode::ByteCodeId::BYTE_CODE_ID_LOAD_OBJECT_ITEM;
	}
}
