#include "ZetScript.h"

namespace ZetScript{


		ByteCode::ByteCode(OP_CODE _op_code
					 ,unsigned char _index_op1
					 ,intptr_t _index_op2
					 ,unsigned short _properties
					 ){
			op_code=_op_code;
			op1_value=_index_op1;
			op2_value=_index_op2;
			properties=_properties;

		}

		const char * ByteCode::opCodeToStr(OP_CODE  op){
			switch(op){
				case EQU:        return "EQU";  // ==
				case INSTANCEOF:  return "INSTANCEOF";  // ==
				case NOT_EQU:     return "NOT_EQU" ;  // !=
				case LT:          return "LT"  ;  // <
				case LTE:         return "LTE";  // <=
				case NOT:         return "NOT"; // !
				case GT:          return "GT";  // >
				case GTE:         return "GTE"; // >=
				case NEG:         return "NEG"; // !
				case ADD:         return "ADD"; // +
				case LOGIC_AND:   return "LOGIC_AND"; // &&
				case LOGIC_OR:    return "LOGIC_OR";  // ||
				case DIV:         return "DIV"; // /
				case MUL:         return "MUL"; // *
				case MOD:         return "MOD";  // %

				case AND:         return "AND"; // bitwise logic and
				case OR:         return "OR"; // bitwise logic or
				case XOR:         return "XOR"; // logic xor
				case SHL:         return "SHL"; // shift left
				case SHR:         return "SHR"; // shift right

				case STORE:       return "STORE"; // mov expression to var
				case LOAD:        return "LOAD"; // primitive value like number/stdstring or boolean...

				case JMP:         return "JMP"; // Unconditional jump.
				case JNT:         return "JNT"; // goto if not true ... goes end to conditional.
				case JT:          return "JT"; // goto if true ... goes end to conditional.

				case CALL: return "CALL"; // calling function after all of arguments are processed...


				case VGET: return "VGET"; // stdvector access after each index is processed...

				case DECL_VEC: return "DECL_VEC"; // Vector object (CREATE)

				case VPUSH: return "VPUSH"; // Value push for stdvector
				case RET: return "RET"; // Value pop for stdvector

				case NEW: return "NEW"; // New object (CREATE)
				case DELETE_OP:return "DELETE_OP";

				case POP_SCOPE: return "POP_SCOPE"; // New object (CREATE)
				case PUSH_SCOPE: return "PUSH_SCOPE"; // New object (CREATE)
				case PUSH_ATTR: return "PUSH_ATTR"; // New object (CREATE)
				case DECL_STRUCT: return "DECL_STRUCT"; // New object (CREATE)

				case IT_INI:	 return "IT_INI"; // IT_INI
				case IT_CHK_END: return "IT_CHK_END"; // IT_CHK_END
				case IT_SET_AND_NEXT:return "IT_SET_AND_NEXT"; // IT_SET_AND_NEXT
				default:
					break;
			}


			return "unknow_op";
		}

		const char * ByteCode::metamethodOpCodeToStr(METAMETHOD_OP_CODE op){

			switch (op) {
				case	EQU_METAMETHOD:		return  "_equ";  // ==
				case	NOT_EQU_METAMETHOD: return  "_nequ";  // !=,
				case	LT_METAMETHOD:		return  "_lt";  // <
				case	LTE_METAMETHOD:		return  "_lte";  // <=
				case	NOT_METAMETHOD:		return  "_not"; // !
				case	GT_METAMETHOD:		return  "_gt";  // >
				case	GTE_METAMETHOD:		return  "_gte"; // >=

				case	NEG_METAMETHOD:		return  "_neg"; // -a, !a
				case	ADD_METAMETHOD:		return  "_add"; // +
				case	DIV_METAMETHOD:		return  "_div"; // /
				case	MUL_METAMETHOD:		return  "_mul"; // *
				case	MOD_METAMETHOD:		return  "_mod";  // %
				case	AND_METAMETHOD:		return  "_and"; // binary and
				case	OR_METAMETHOD:		return  "_or"; //   binary or
				case	XOR_METAMETHOD:		return  "_xor"; // binary xor
				case	SHL_METAMETHOD:		return  "_shl"; // binary shift left
				case	SHR_METAMETHOD:		return  "_shr"; // binary shift right
				case	SET_METAMETHOD:		return  "_set"; // set
				default:
					return "none";
			}

			return "none";
		}

}
