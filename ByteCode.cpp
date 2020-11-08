#include "zetscript.h"

namespace zetscript{

	const char * ByteCodeToStr(ByteCode _byte_code){
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
			case BYTE_CODE_AND:         		return "AND"; // bitwise logic and
			case BYTE_CODE_OR:         			return "OR"; // bitwise logic or
			case BYTE_CODE_XOR:         		return "XOR"; // logic xor
			case BYTE_CODE_SHL:         		return "SHL"; // shift left
			case BYTE_CODE_SHR:         		return "SHR"; // shift right
			case BYTE_CODE_STORE:       		return "STORE"; // mov expression to var
			case BYTE_CODE_LOAD_TYPE_VARIABLE:	return "LOAD_VAR"; // load type var
			case BYTE_CODE_LOAD_TYPE_FIND:      return "LOAD_???"; // load to find
			case BYTE_CODE_LOAD_TYPE_FUNCTION:	return "LOAD_FUN"; // load to find
			case BYTE_CODE_LOAD_TYPE_UNDEFINED:	return "LOAD_UNDEF"; // load to find
			case BYTE_CODE_LOAD_TYPE_NULL:		return "LOAD_NULL"; // load to find
			case BYTE_CODE_LOAD_TYPE_CONSTANT:	return "LOAD_CONST"; // load to find
			case BYTE_CODE_LOAD_TYPE_STATIC:	return "LOAD_STATIC"; // load to find
			case BYTE_CODE_LOAD_TYPE_CLASS:		return "LOAD_CLASS"; // load to find
			case BYTE_CODE_JMP:         		return "JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         		return "JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          		return "JT"; // goto if true ... goes end to conditional.
			case BYTE_CODE_JE:					return "JE"; // goto if equal ... goes end to conditional.
			case BYTE_CODE_CALL: 				return "CALL"; // calling function after all of arguments are processed...
			case BYTE_CODE_NEW_VECTOR: 			return "NEW_VEC"; // Vector object (CREATE)
			case BYTE_CODE_PUSH_VECTOR_ELEMENT: return "VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: 				return "RET"; // Value pop for stdvector
			case BYTE_CODE_NEW: 				return "NEW"; // New object (CREATE)
			case BYTE_CODE_DELETE:				return "DELETE";
			case BYTE_CODE_POP_SCOPE: 			return "POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: 			return "PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:	return "PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_NEW_OBJECT: 			return "NEW_OBJ"; // New object (CREATE)
			case BYTE_CODE_IT_END: 				return "IT_END"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_IT_NEXT:				return "IT_NEXT"; // BYTE_CODE_SET_AND_NEXT
			case BYTE_CODE_RESET_STACK:			return "RESET_STACK"; // BYTE_CODE_SET_AND_NEXT

			default:
				break;
		}
		return "unknow_op";
	}

	const char * ByteCodeMetamethodToOperatorStr(ByteCodeMetamethod op){
		switch (op) {
			case	BYTE_CODE_METAMETHOD_EQU:		return  "==";  // ==
			case	BYTE_CODE_METAMETHOD_NOT_EQU: 	return  "!=";  // !=,
			case	BYTE_CODE_METAMETHOD_LT:		return  "<";  // <
			case	BYTE_CODE_METAMETHOD_LTE:		return  "<=";  // <=
			case	BYTE_CODE_METAMETHOD_NOT:		return  "!"; // !
			case	BYTE_CODE_METAMETHOD_GT:		return  ">";  // >
			case	BYTE_CODE_METAMETHOD_GTE:		return  ">="; // >=

			case	BYTE_CODE_METAMETHOD_NEG:		return  "-"; // -a, !a
			case	BYTE_CODE_METAMETHOD_ADD:		return  "+"; // +
			case	BYTE_CODE_METAMETHOD_SUB:		return  "-"; // -
			case	BYTE_CODE_METAMETHOD_DIV:		return  "/"; // /
			case	BYTE_CODE_METAMETHOD_MUL:		return  "*"; // *
			case	BYTE_CODE_METAMETHOD_MOD:		return  "%";  // %
			case	BYTE_CODE_METAMETHOD_AND:		return  "&"; // binary and
			case	BYTE_CODE_METAMETHOD_OR:		return  "|"; //   binary or
			case	BYTE_CODE_METAMETHOD_XOR:		return  "^"; // binary xor
			case	BYTE_CODE_METAMETHOD_SHL:		return  "<<"; // binary shift left
			case	BYTE_CODE_METAMETHOD_SHR:		return  ">>"; // binary shift right
			case	BYTE_CODE_METAMETHOD_SET:		return  "="; // set
			default:
				return "none";
		}
	}

	const char * ByteCodeMetamethodToSymbolStr(ByteCodeMetamethod op){

		switch (op) {
			case	BYTE_CODE_METAMETHOD_EQU:		return  "_equ";  // ==
			case	BYTE_CODE_METAMETHOD_NOT_EQU: 	return  "_nequ";  // !=,
			case	BYTE_CODE_METAMETHOD_LT:		return  "_lt";  // <
			case	BYTE_CODE_METAMETHOD_LTE:		return  "_lte";  // <=
			case	BYTE_CODE_METAMETHOD_NOT:		return  "_not"; // !
			case	BYTE_CODE_METAMETHOD_GT:		return  "_gt";  // >
			case	BYTE_CODE_METAMETHOD_GTE:		return  "_gte"; // >=

			case	BYTE_CODE_METAMETHOD_NEG:		return  "_neg"; // -a, !a
			case	BYTE_CODE_METAMETHOD_ADD:		return  "_add"; // +
			case	BYTE_CODE_METAMETHOD_SUB:		return  "_sub"; // -
			case	BYTE_CODE_METAMETHOD_DIV:		return  "_div"; // /
			case	BYTE_CODE_METAMETHOD_MUL:		return  "_mul"; // *
			case	BYTE_CODE_METAMETHOD_MOD:		return  "_mod";  // %
			case	BYTE_CODE_METAMETHOD_AND:		return  "_and"; // binary and
			case	BYTE_CODE_METAMETHOD_OR:		return  "_or"; //   binary or
			case	BYTE_CODE_METAMETHOD_XOR:		return  "_xor"; // binary xor
			case	BYTE_CODE_METAMETHOD_SHL:		return  "_shl"; // binary shift left
			case	BYTE_CODE_METAMETHOD_SHR:		return  "_shr"; // binary shift right
			case	BYTE_CODE_METAMETHOD_SET:		return  "_set"; // set
			default:
				return "none";
		}

		return "none";
	}

}
