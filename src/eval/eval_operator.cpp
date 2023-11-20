/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	ByteCode::ByteCodeId eval_operator_to_byte_code(Operator op){
		switch(op){
		default:
			ZS_THROW_RUNTIME_ERROR("Convert %i to byte code not implemented",op);
			break;
		// assign and arithmetic with assign
		case Operator::ZS_OPERATOR_ASSIGN:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_STORE;
		case Operator::ZS_OPERATOR_ASSIGN_ADD:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_ADD_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SUB:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SUB_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_MUL:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_MUL_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_DIV:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_DIV_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_MOD:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_MOD_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_XOR:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_AND:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_OR:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_LEFT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SHL_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_RIGHT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SHR_ASSIGN;

		// arithmetic ops
		case Operator::ZS_OPERATOR_ADD:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_ADD;
		case Operator::ZS_OPERATOR_SUB:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SUB;
		case Operator::ZS_OPERATOR_MUL:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_MUL;
		case Operator::ZS_OPERATOR_DIV:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_DIV;
		case Operator::ZS_OPERATOR_MOD:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_MOD;
		case Operator::ZS_OPERATOR_XOR:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_XOR;
		case Operator::ZS_OPERATOR_AND:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_AND;
		case Operator::ZS_OPERATOR_OR:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_BITWISE_OR;
		case Operator::ZS_OPERATOR_SHIFT_LEFT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SHL;
		case Operator::ZS_OPERATOR_SHIFT_RIGHT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_SHR;

		// logic
		case Operator::ZS_OPERATOR_LOGIC_AND:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_AND;
		case Operator::ZS_OPERATOR_LOGIC_OR:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_LOGIC_OR;
		case Operator::ZS_OPERATOR_EQUAL:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_EQU;
		case Operator::ZS_OPERATOR_NOT_EQUAL:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_NOT_EQU;
		case Operator::ZS_OPERATOR_GT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_GT;
		case Operator::ZS_OPERATOR_LT:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_LT;
		case Operator::ZS_OPERATOR_GTE:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_GTE;
		case Operator::ZS_OPERATOR_LTE:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_LTE;
		case Operator::ZS_OPERATOR_INSTANCEOF:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_INSTANCEOF;
		case Operator::ZS_OPERATOR_IN:
			return ByteCode::ByteCodeId::BYTE_CODE_ID_IN;
		}
		return ByteCode::ByteCodeId::BYTE_CODE_ID_INVALID;
	}

	const char * eval_operator_to_str(Operator op){
		switch(op){
		default:
			break;
		// assign and arithmetic with assign
		case Operator::ZS_OPERATOR_ASSIGN:
			return "=";
		case Operator::ZS_OPERATOR_ASSIGN_ADD:
			return "+=";
		case Operator::ZS_OPERATOR_ASSIGN_SUB:
			return "-=";
		case Operator::ZS_OPERATOR_ASSIGN_MUL:
			return "*=";
		case Operator::ZS_OPERATOR_ASSIGN_DIV:
			return "/=";
		case Operator::ZS_OPERATOR_ASSIGN_MOD:
			return "%=";
		case Operator::ZS_OPERATOR_ASSIGN_XOR:
			return "^=";
		case Operator::ZS_OPERATOR_ASSIGN_AND:
			return "&=";
		case Operator::ZS_OPERATOR_ASSIGN_OR:
			return "|=";
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_LEFT:
			return "<<=";
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_RIGHT:
			return ">>=";

		// arithmetic ops
		case Operator::ZS_OPERATOR_ADD:
			return "+";
		case Operator::ZS_OPERATOR_SUB:
			return "-";
		case Operator::ZS_OPERATOR_MUL:
			return "*";
		case Operator::ZS_OPERATOR_DIV:
			return "/";
		case Operator::ZS_OPERATOR_MOD:
			return "%";
		case Operator::ZS_OPERATOR_XOR:
			return "^";
		case Operator::ZS_OPERATOR_AND:
			return "&";
		case Operator::ZS_OPERATOR_OR:
			return "|";
		case Operator::ZS_OPERATOR_SHIFT_LEFT:
			return ">>";
		case Operator::ZS_OPERATOR_SHIFT_RIGHT:
			return "<<";

		// logic
		case Operator::ZS_OPERATOR_LOGIC_AND:
			return "&&";
		case Operator::ZS_OPERATOR_LOGIC_OR:
			return "||";
		case Operator::ZS_OPERATOR_EQUAL:
			return "==";
		case Operator::ZS_OPERATOR_NOT_EQUAL:
			return "!=";
		case Operator::ZS_OPERATOR_GT:
			return ">";
		case Operator::ZS_OPERATOR_LT:
			return "<";
		case Operator::ZS_OPERATOR_GTE:
			return ">=";
		case Operator::ZS_OPERATOR_LTE:
			return "<=";
		case Operator::ZS_OPERATOR_INSTANCEOF:
			return "instanceof";
		}

		return "unknow";
	}

}

