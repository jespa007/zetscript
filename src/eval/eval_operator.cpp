/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	ByteCode eval_operator_to_byte_code(Operator op){
		switch(op){
		default:
			ZS_THROW_RUNTIME_ERROR("Convert %i to byte code not implemented",op);
			break;
		// assign and arithmetic with assign
		case Operator::ZS_OPERATOR_ASSIGN:
			return BYTE_CODE_STORE;
		case Operator::ZS_OPERATOR_ASSIGN_ADD:
			return BYTE_CODE_ADD_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SUB:
			return BYTE_CODE_SUB_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_MUL:
			return BYTE_CODE_MUL_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_DIV:
			return BYTE_CODE_DIV_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_MOD:
			return BYTE_CODE_MOD_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_XOR:
			return BYTE_CODE_BITWISE_XOR_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_AND:
			return BYTE_CODE_BITWISE_AND_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_OR:
			return BYTE_CODE_BITWISE_OR_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_LEFT:
			return BYTE_CODE_SHL_ASSIGN;
		case Operator::ZS_OPERATOR_ASSIGN_SHIFT_RIGHT:
			return BYTE_CODE_SHR_ASSIGN;

		// arithmetic ops
		case Operator::ZS_OPERATOR_ADD:
			return BYTE_CODE_ADD;
		case Operator::ZS_OPERATOR_SUB:
			return BYTE_CODE_SUB;
		case Operator::ZS_OPERATOR_MUL:
			return BYTE_CODE_MUL;
		case Operator::ZS_OPERATOR_DIV:
			return BYTE_CODE_DIV;
		case Operator::ZS_OPERATOR_MOD:
			return BYTE_CODE_MOD;
		case Operator::ZS_OPERATOR_XOR:
			return BYTE_CODE_BITWISE_XOR;
		case Operator::ZS_OPERATOR_AND:
			return BYTE_CODE_BITWISE_AND;
		case Operator::ZS_OPERATOR_OR:
			return BYTE_CODE_BITWISE_OR;
		case Operator::ZS_OPERATOR_SHIFT_LEFT:
			return BYTE_CODE_SHL;
		case Operator::ZS_OPERATOR_SHIFT_RIGHT:
			return BYTE_CODE_SHR;

		// logic
		case Operator::ZS_OPERATOR_LOGIC_AND:
			return BYTE_CODE_LOGIC_AND;
		case Operator::ZS_OPERATOR_LOGIC_OR:
			return BYTE_CODE_LOGIC_OR;
		case Operator::ZS_OPERATOR_EQUAL:
			return BYTE_CODE_EQU;
		case Operator::ZS_OPERATOR_NOT_EQUAL:
			return BYTE_CODE_NOT_EQU;
		case Operator::ZS_OPERATOR_GT:
			return BYTE_CODE_GT;
		case Operator::ZS_OPERATOR_LT:
			return BYTE_CODE_LT;
		case Operator::ZS_OPERATOR_GTE:
			return BYTE_CODE_GTE;
		case Operator::ZS_OPERATOR_LTE:
			return BYTE_CODE_LTE;
		case Operator::ZS_OPERATOR_INSTANCEOF:
			return BYTE_CODE_INSTANCEOF;
		case Operator::ZS_OPERATOR_IN:
			return BYTE_CODE_IN;
		}
		return BYTE_CODE_INVALID;
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

