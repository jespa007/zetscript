/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	const char * byte_code_to_str(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        							return "EQU";  // ==
			case BYTE_CODE_INSTANCEOF:  						return "INSTANCEOF";  // ==
			case BYTE_CODE_NOT_EQU:     						return "NOT_EQU" ;  // !=
			case BYTE_CODE_LT:          						return "LT";  // <
			case BYTE_CODE_LTE:         						return "LTE";  // <=
			case BYTE_CODE_NOT:         						return "NOT"; // !
			case BYTE_CODE_GT:          						return "GT";  // >
			case BYTE_CODE_GTE:        							return "GTE"; // >=
			case BYTE_CODE_NEG:         						return "NEG"; // !
			case BYTE_CODE_ADD:         						return "ADD"; // +
			case BYTE_CODE_SUB:         						return "SUB"; // -
			case BYTE_CODE_LOGIC_AND:   						return "LOGIC_AND"; // &&
			case BYTE_CODE_LOGIC_OR:    						return "LOGIC_OR";  // ||
			case BYTE_CODE_DIV:         						return "DIV"; // /
			case BYTE_CODE_MUL:         						return "MUL"; // *
			case BYTE_CODE_MOD:         						return "MOD";  // %
			case BYTE_CODE_BITWISE_AND:         				return "AND"; // bitwise logic and
			case BYTE_CODE_BITWISE_OR:         					return "OR"; // bitwise logic or
			case BYTE_CODE_BITWISE_XOR:         				return "XOR"; // logic xor
			case BYTE_CODE_SHL:         						return "SHL"; // shift left
			case BYTE_CODE_SHR:         						return "SHR"; // shift right
			case BYTE_CODE_STORE:       						return "STORE"; // =
			case BYTE_CODE_ADD_STORE:       					return "STORE_ADD"; // +=
			case BYTE_CODE_SUB_STORE:       					return "STORE_SUB"; // -=
			case BYTE_CODE_MUL_STORE:       					return "STORE_MUL"; // *=
			case BYTE_CODE_DIV_STORE:       					return "STORE_DIV"; // /=
			case BYTE_CODE_MOD_STORE:       					return "STORE_MOD"; // %=
			case BYTE_CODE_BITWISE_AND_STORE:      				return "STORE_AND"; // &=
			case BYTE_CODE_BITWISE_OR_STORE: 	   				return "STORE_OR"; // |=
			case BYTE_CODE_BITWISE_XOR_STORE: 	   				return "STORE_XOR"; // ^=
			case BYTE_CODE_SHL_STORE: 	      					return "STORE_SHL"; // <<=
			case BYTE_CODE_SHR_STORE: 	      					return "STORE_SHR"; // >>=
			case BYTE_CODE_PUSH_STK_GLOBAL:						return "PUSH_STK_GLOBAL"; // push stk global
			case BYTE_CODE_PUSH_STK_LOCAL:						return "PUSH_STK_LOCAL"; // push stk global
			//case BYTE_CODE_PUSH_STK_REF:						return "PUSH_STK_REF"; // push stk global
			case BYTE_CODE_PUSH_STK_THIS:						return "PUSH_STK_THIS"; // push stk global
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:					return "PUSH_STK_MEMBER_VAR"; // push stk global
			case BYTE_CODE_PUSH_STK_VECTOR_ITEM:				return "PUSH_STK_VEC@ITEM"; // load element vector
			case BYTE_CODE_PUSH_STK_OBJECT_ITEM:				return "PUSH_STK_OBJ@ITEM"; // load element object
			case BYTE_CODE_PUSH_STK_THIS_VARIABLE:				return "PUSH_STK_THIS@VAR"; // load element object

			case BYTE_CODE_LOAD_GLOBAL:							return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:							return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_REF:							return "LOAD_REF"; // load type var
			case BYTE_CODE_LOAD_THIS:							return "LOAD_THIS"; // load type var
			case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:				return "LOAD_CONSTRUCTOR_FUNCT"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      					return "LOAD_???"; // load to find global
			// CALL
			case BYTE_CODE_INDIRECT_LOCAL_CALL:
			case BYTE_CODE_INDIRECT_GLOBAL_CALL:
			case BYTE_CODE_THIS_CALL:
			case BYTE_CODE_THIS_MEMBER_CALL:
			case BYTE_CODE_CALL:								return "CALL"; // direct call

			case BYTE_CODE_UNRESOLVED_THIS_CALL:
			case BYTE_CODE_UNRESOLVED_CALL:						return "CALL???"; // direct call

			case BYTE_CODE_MEMBER_CALL:							return "MEMBER_CALL"; // this indirect call through dynamic symbol
			case BYTE_CODE_LOAD_THIS_VARIABLE:					return "LOAD_THIS@VAR"; // load element object
			case BYTE_CODE_LOAD_THIS_FUNCTION:					return "LOAD_THIS@FUN"; // load element object
			case BYTE_CODE_LOAD_VECTOR_ITEM:					return "LOAD_VEC@ITEM"; // load element vector
			case BYTE_CODE_LOAD_OBJECT_ITEM:					return "LOAD_OBJ@ITEM"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:						return "LOAD_FUN"; // load function
			case BYTE_CODE_LOAD_NULL:							return "LOAD_NULL"; // load undfined
			case BYTE_CODE_LOAD_STACK_ELEMENT:					return "LOAD_STK"; // load stack element

			case BYTE_CODE_LOAD_STRING:							return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_ZS_FLOAT:						return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:							return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_ZS_INT:							return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_TYPE:							return "LOAD_TYPE"; // load to find
			case BYTE_CODE_JMP:         						return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         						return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          						return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE_CASE:								return "JE_CASE"; // je case equal ... goes end to conditional.
			case BYTE_CODE_CONSTRUCTOR_CALL: 					return "CALL_CONSTRUCTOR"; // calling function after all of arguments are processed...
			case BYTE_CODE_NEW_VECTOR: 							return "NEW_VEC"; // Vector object (CREATE)
			case BYTE_CODE_PUSH_VECTOR_ITEM: 					return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 								return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_OBJECT_BY_KNOWN_TYPE:			return "NEW_CLASS"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT_BY_VALUE:					return "NEW_CLASS_FROM_VALUE"; // New object (CREATE)
			case BYTE_CODE_DELETE:								return "DELETE";
			case BYTE_CODE_POP_SCOPE: 							return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 							return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ITEM:					return "PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT:							return "NEW_OBJ"; // New object (CREATE)
			case BYTE_CODE_NEW_STRING:							return "NEW_STR";
			case BYTE_CODE_IT_INIT: 							return "IT_INIT"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_STORE_CONST:							return "STORE_CONST"; // BYTE_CODE_STORE_CONST
			case BYTE_CODE_PRE_INC:								return "PRE_INC"; // ++i
			case BYTE_CODE_PRE_DEC:								return "PRE_DEC"; // --i
			case BYTE_CODE_POST_INC:							return "POST_INC"; // i++
			case BYTE_CODE_NEG_POST_INC:						return "NEG_POST_INC"; // -i++
			case BYTE_CODE_POST_DEC:							return "POST_DEC"; // i--
			case BYTE_CODE_NEG_POST_DEC:						return "NEG_POST_DEC"; // -i--
			case BYTE_CODE_RESET_STACK:							return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_TYPEOF:								return "TYPEOF"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_IN:									return "IN"; // POP ONE AND IT MARKS END EXPRESSION


			default:
				break;
		}
		return "unknow_op";
	}

	const char * byte_code_to_operator_str(ByteCode _byte_code){
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
			case BYTE_CODE_ADD:         			return "+"; // +
			case BYTE_CODE_SUB:         			return "-"; // -
			case BYTE_CODE_LOGIC_AND:   			return "&&"; // &&
			case BYTE_CODE_LOGIC_OR:    			return "||";  // ||
			case BYTE_CODE_DIV:         			return "/"; // /
			case BYTE_CODE_MUL:         			return "*"; // *
			case BYTE_CODE_MOD:         			return "%";  // %
			case BYTE_CODE_BITWISE_AND:         	return "&"; // bitwise logic and
			case BYTE_CODE_BITWISE_OR:         		return "|"; // bitwise logic or
			case BYTE_CODE_BITWISE_XOR:      		return "^"; // logic xor
			case BYTE_CODE_SHL:         			return ">>"; // shift left
			case BYTE_CODE_SHR:         			return "<<"; // shift right
			case BYTE_CODE_STORE:       			return "="; // =
			case BYTE_CODE_ADD_STORE:       		return "+="; // +=
			case BYTE_CODE_SUB_STORE:       		return "-="; // -=
			case BYTE_CODE_MUL_STORE:       		return "*="; // *=
			case BYTE_CODE_DIV_STORE:       		return "/="; // /=
			case BYTE_CODE_MOD_STORE:       		return "%="; // %=
			case BYTE_CODE_BITWISE_AND_STORE:   	return "&="; // &=
			case BYTE_CODE_BITWISE_OR_STORE: 		return "|="; // |=
			case BYTE_CODE_BITWISE_XOR_STORE: 		return "^="; // ^=
			case BYTE_CODE_SHL_STORE: 	      		return ">>="; // <<=
			case BYTE_CODE_SHR_STORE: 	      		return "<<="; // >>=
			case BYTE_CODE_LOAD_GLOBAL:				return "LOAD_GLOBAL"; // load type var
			case BYTE_CODE_LOAD_LOCAL:				return "LOAD_LOCAL"; // load type var
			case BYTE_CODE_LOAD_THIS:				return "LOAD_THIS"; // load type var
			case BYTE_CODE_FIND_VARIABLE:      		return "LOAD_???"; // load to find global
			case BYTE_CODE_LOAD_VECTOR_ITEM:		return "LOAD_EVEC"; // load element vector
			case BYTE_CODE_LOAD_OBJECT_ITEM:		return "LOAD_EOBJ"; // load element object
			case BYTE_CODE_LOAD_FUNCTION:			return "LOAD_FUNCT"; // load function
			case BYTE_CODE_LOAD_NULL:				return "LOAD_NULL"; // load undfined
			case BYTE_CODE_LOAD_STACK_ELEMENT:		return "LOAD_STK"; // load stack element
			case BYTE_CODE_LOAD_STRING:				return "LOAD_STR"; // load string
			case BYTE_CODE_LOAD_ZS_FLOAT:			return "LOAD_FLT"; // load float
			case BYTE_CODE_LOAD_BOOL:				return "LOAD_BOOL"; // load bool
			case BYTE_CODE_LOAD_ZS_INT:				return "LOAD_INT"; // load zs_int
			case BYTE_CODE_LOAD_TYPE:				return "LOAD_TYPE"; // load to find
			case BYTE_CODE_JMP:         			return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         			return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          			return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE_CASE:					return "JE"; // goto if equal ... goes end to conditional.
			case BYTE_CODE_NEW_VECTOR: 				return "NEW_VEC"; // Vector object (CREATE)
			case BYTE_CODE_PUSH_VECTOR_ITEM: 		return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 					return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW_OBJECT_BY_KNOWN_TYPE:return "NEW_CLASS"; // New object (CREATE)
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
			case BYTE_CODE_NEG_POST_INC:			return "NEG_POST_INC"; // -i++
			case BYTE_CODE_POST_DEC:				return "POST_DEC"; // i--
			case BYTE_CODE_NEG_POST_DEC:			return "NEG_POST_DEC"; // -i--
			case BYTE_CODE_RESET_STACK:				return "RESET_STACK"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_TYPEOF:					return "typeof"; // POP ONE AND IT MARKS END EXPRESSION
			case BYTE_CODE_IN:						return "in"; // POP ONE AND IT MARKS END EXPRESSION

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
			case	BYTE_CODE_METAMETHOD_NEG:		return  "-"; 		// -a
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
			case	BYTE_CODE_METAMETHOD_ADD_SET:	return  "+=";		// _set_add
			case	BYTE_CODE_METAMETHOD_SUB_SET:	return  "-=";		// _set_sub
			case	BYTE_CODE_METAMETHOD_DIV_SET:	return  "/=";		// _set_div
			case	BYTE_CODE_METAMETHOD_MOD_SET:	return  "%=";		// _set_mod
			case	BYTE_CODE_METAMETHOD_AND_SET:	return  "&=";		// _set_and
			case	BYTE_CODE_METAMETHOD_OR_SET:	return  "|=";		// _set_or
			case	BYTE_CODE_METAMETHOD_XOR_SET:	return  "^=";		// _set_xor
			case	BYTE_CODE_METAMETHOD_SHL_SET:	return  "<<=";		// _set_shl
			case	BYTE_CODE_METAMETHOD_SHR_SET:	return  ">>=";		// _set_shr
			//case	BYTE_CODE_METAMETHOD_GET:		return  "_get";		// _get
			case	BYTE_CODE_METAMETHOD_TO_STRING: return  "_toString";// _toString
			case 	BYTE_CODE_METAMETHOD_POST_INC: 	return  "++";		// i++
			case 	BYTE_CODE_METAMETHOD_POST_DEC: 	return  "--";		//  i--
			case 	BYTE_CODE_METAMETHOD_PRE_INC: 	return  "++";		// ++i
			case 	BYTE_CODE_METAMETHOD_PRE_DEC: 	return  "--";		// --i
			case 	BYTE_CODE_METAMETHOD_IN: 		return  "in";		// in
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
			case	BYTE_CODE_METAMETHOD_ADD_SET:	return  "_set_add";	// _set_add
			case	BYTE_CODE_METAMETHOD_SUB_SET:	return  "_set_sub";	// _set_sub
			case	BYTE_CODE_METAMETHOD_DIV_SET:	return  "_set_div";	// _set_div
			case	BYTE_CODE_METAMETHOD_MOD_SET:	return  "_set_mod";	// _set_mod
			case	BYTE_CODE_METAMETHOD_AND_SET:	return  "_set_and";	// _set_and
			case	BYTE_CODE_METAMETHOD_OR_SET:	return  "_set_or"; 	// _set_or
			case	BYTE_CODE_METAMETHOD_XOR_SET:	return  "_set_xor";	// _set_xor
			case	BYTE_CODE_METAMETHOD_SHL_SET:	return  "_set_shl";	// _set_shl
			case	BYTE_CODE_METAMETHOD_SHR_SET:	return  "_set_shr";	// _set_shr

			//case	BYTE_CODE_METAMETHOD_GET:		return  "_get"; 	// _get
			case	BYTE_CODE_METAMETHOD_TO_STRING:	return  "_toString";// _toString
			case 	BYTE_CODE_METAMETHOD_POST_INC: 	return  "_post_inc";// i++
			case 	BYTE_CODE_METAMETHOD_POST_DEC:	return  "_post_dec";// i--
			case 	BYTE_CODE_METAMETHOD_PRE_INC: 	return  "_pre_inc";	// ++i
			case 	BYTE_CODE_METAMETHOD_PRE_DEC:	return  "_pre_dec";	// --i
			case 	BYTE_CODE_METAMETHOD_IN: 		return  "_in";		// 	_in
			default:
				return "none";
		}

		return "none";
	}

	int			 byte_code_metamethod_get_num_arguments(ByteCodeMetamethod op){
		switch(op){
			case BYTE_CODE_METAMETHOD_TO_STRING:
			case BYTE_CODE_METAMETHOD_POST_INC:
			case BYTE_CODE_METAMETHOD_POST_DEC:
			case BYTE_CODE_METAMETHOD_PRE_INC:
			case BYTE_CODE_METAMETHOD_PRE_DEC:
			case BYTE_CODE_METAMETHOD_NOT:
			case BYTE_CODE_METAMETHOD_NEG:
				return 0;
			case BYTE_CODE_METAMETHOD_SET:
			case BYTE_CODE_METAMETHOD_IN:
				return 1;
		}

		return 2;
	}

	bool		 byte_code_metamethod_should_be_static(ByteCodeMetamethod op){

		switch(op){
		case BYTE_CODE_METAMETHOD_EQU:
		case BYTE_CODE_METAMETHOD_NOT_EQU:
		case BYTE_CODE_METAMETHOD_LT:
		case BYTE_CODE_METAMETHOD_LTE:
		case BYTE_CODE_METAMETHOD_GT:
		case BYTE_CODE_METAMETHOD_GTE:
		case BYTE_CODE_METAMETHOD_ADD:
		case BYTE_CODE_METAMETHOD_SUB:
		case BYTE_CODE_METAMETHOD_DIV:
		case BYTE_CODE_METAMETHOD_MUL:
		case BYTE_CODE_METAMETHOD_MOD:
		case BYTE_CODE_METAMETHOD_AND:
		case BYTE_CODE_METAMETHOD_OR:
		case BYTE_CODE_METAMETHOD_XOR:
		case BYTE_CODE_METAMETHOD_SHL:
		case BYTE_CODE_METAMETHOD_SHR:
			return true;
			break;
		default:
			break;
		}
		return false;
	}

	/*int			 get_num_arguments_non_metamethod(ByteCodeMetamethod op){
		return byte_code_get_num_arguments_static_metamethod(op)-1;
	}*/


	ByteCode			 byte_code_load_var_type_to_push_stk(ByteCode byte_code){
	// load ptr var
		switch(byte_code){
			// load var content
			case BYTE_CODE_LOAD_GLOBAL:return BYTE_CODE_PUSH_STK_GLOBAL;
			case BYTE_CODE_LOAD_LOCAL:return BYTE_CODE_PUSH_STK_LOCAL;
			case BYTE_CODE_LOAD_REF:return BYTE_CODE_LOAD_REF; // PUSH STK NOT EXIST, IS A REF ITSELF
			case BYTE_CODE_LOAD_THIS:return BYTE_CODE_PUSH_STK_THIS;
			case BYTE_CODE_LOAD_VECTOR_ITEM:return BYTE_CODE_PUSH_STK_VECTOR_ITEM;
			case BYTE_CODE_LOAD_THIS_VARIABLE:return BYTE_CODE_PUSH_STK_THIS_VARIABLE;
			case BYTE_CODE_LOAD_OBJECT_ITEM:return BYTE_CODE_PUSH_STK_OBJECT_ITEM;
			default:
				THROW_RUNTIME_ERROR("internal: op code not type load");
				break;
		}

		return byte_code;

	}

	bool		 byte_code_is_load_var_type(ByteCode byte_code){
		return byte_code==BYTE_CODE_LOAD_GLOBAL
				|| byte_code==BYTE_CODE_LOAD_LOCAL
				|| byte_code==BYTE_CODE_LOAD_REF
				|| byte_code==BYTE_CODE_LOAD_THIS
				|| byte_code==BYTE_CODE_LOAD_VECTOR_ITEM
				|| byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
				|| byte_code==BYTE_CODE_LOAD_OBJECT_ITEM;
	}






}
