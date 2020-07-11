#include "zetscript.h"

namespace zetscript{

	const char * ByteCodeToStr(ByteCode _byte_code){
		switch(_byte_code){
			case BYTE_CODE_EQU:        return "BYTE_CODE_EQU";  // ==
			case BYTE_CODE_INSTANCEOF:  return "BYTE_CODE_INSTANCEOF";  // ==
			case BYTE_CODE_NOT_EQU:     return "BYTE_CODE_NOT_EQU" ;  // !=
			case BYTE_CODE_LT:          return "BYTE_CODE_LT"  ;  // <
			case BYTE_CODE_LTE:         return "BYTE_CODE_LTE";  // <=
			case BYTE_CODE_NOT:         return "BYTE_CODE_NOT"; // !
			case BYTE_CODE_GT:          return "BYTE_CODE_GT";  // >
			case BYTE_CODE_GTE:         return "BYTE_CODE_GTE"; // >=
			case BYTE_CODE_NEG:         return "BYTE_CODE_NEG"; // !
			case BYTE_CODE_ADD:         return "BYTE_CODE_ADD"; // +
			case BYTE_CODE_LOGIC_AND:   return "BYTE_CODE_LOGIC_AND"; // &&
			case BYTE_CODE_LOGIC_OR:    return "BYTE_CODE_LOGIC_OR";  // ||
			case BYTE_CODE_DIV:         return "BYTE_CODE_DIV"; // /
			case BYTE_CODE_MUL:         return "BYTE_CODE_MUL"; // *
			case BYTE_CODE_MOD:         return "BYTE_CODE_MOD";  // %

			case BYTE_CODE_AND:         return "BYTE_CODE_AND"; // bitwise logic and
			case BYTE_CODE_OR:         return "BYTE_CODE_OR"; // bitwise logic or
			case BYTE_CODE_XOR:         return "BYTE_CODE_XOR"; // logic xor
			case BYTE_CODE_SHL:         return "BYTE_CODE_SHL"; // shift left
			case BYTE_CODE_SHR:         return "BYTE_CODE_SHR"; // shift right

			case BYTE_CODE_STORE:       return "BYTE_CODE_STORE"; // mov expression to var
			case BYTE_CODE_LOAD:        return "BYTE_CODE_LOAD"; // primitive value like number/stdstring or boolean...

			case BYTE_CODE_JMP:         return "BYTE_CODE_JMP"; // Unconditional jump.
			case BYTE_CODE_JNT:         return "BYTE_CODE_JNT"; // goto if not true ... goes end to conditional.
			case BYTE_CODE_JT:          return "BYTE_CODE_JT"; // goto if true ... goes end to conditional.

			case BYTE_CODE_CALL: return "BYTE_CODE_CALL"; // calling function after all of arguments are processed...


			case BYTE_CODE_VGET: return "BYTE_CODE_VGET"; // stdvector access after each index is processed...

			case BYTE_CODE_DECL_VEC: return "BYTE_CODE_DECL_VEC"; // Vector object (CREATE)

			case BYTE_CODE_VPUSH: return "BYTE_CODE_VPUSH"; // Value push for stdvector
			case BYTE_CODE_RET: return "BYTE_CODE_RET"; // Value pop for stdvector

			case BYTE_CODE_NEW: return "BYTE_CODE_NEW"; // New object (CREATE)
			case BYTE_CODE_DELETE:return "BYTE_CODE_DELETE";

			case BYTE_CODE_POP_SCOPE: return "BYTE_CODE_POP_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_SCOPE: return "BYTE_CODE_PUSH_SCOPE"; // New object (CREATE)
			case BYTE_CODE_PUSH_ATTR: return "BYTE_CODE_PUSH_ATTR"; // New object (CREATE)
			case BYTE_CODE_DECL_DICTIONARY: return "BYTE_CODE_DECL_DICTIONARY"; // New object (CREATE)

			case BYTE_CODE_IT_INI:	 return "BYTE_CODE_IT_INI"; // BYTE_CODE_IT_INI
			case BYTE_CODE_IT_CHK_END: return "BYTE_CODE_IT_CHK_END"; // BYTE_CODE_IT_CHK_END
			case BYTE_CODE_SET_AND_NEXT:return "BYTE_CODE_SET_AND_NEXT"; // BYTE_CODE_SET_AND_NEXT
			default:
				break;
		}


		return "unknow_op";
	}

	const char * ByteCodeMetamethodToStr(ByteCodeMetamethod op){

		switch (op) {
			case	BYTE_CODE_METAMETHOD_EQU:		return  "_equ";  // ==
			case	BYTE_CODE_METAMETHOD_NOT_EQU: return  "_nequ";  // !=,
			case	BYTE_CODE_METAMETHOD_LT:		return  "_lt";  // <
			case	BYTE_CODE_METAMETHOD_LTE:		return  "_lte";  // <=
			case	BYTE_CODE_METAMETHOD_NOT:		return  "_not"; // !
			case	BYTE_CODE_METAMETHOD_GT:		return  "_gt";  // >
			case	BYTE_CODE_METAMETHOD_GTE:		return  "_gte"; // >=

			case	BYTE_CODE_METAMETHOD_NEG:		return  "_neg"; // -a, !a
			case	BYTE_CODE_METAMETHOD_ADD:		return  "_add"; // +
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
