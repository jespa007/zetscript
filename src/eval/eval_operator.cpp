/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "eval.h"

namespace zetscript{

	ByteCode eval_operator_to_byte_code(Operator op){
		switch(op){
		default:
			THROW_RUNTIME_ERROR("Convert %i to byte code not implemented",op);
			break;
		// assign and arithmetic with assign
		case Operator::OPERATOR_ASSIGN:
			return ByteCode::BYTE_CODE_STORE;
		case Operator::OPERATOR_ASSIGN_ADD:
			return ByteCode::BYTE_CODE_STORE_ADD;
		case Operator::OPERATOR_ASSIGN_SUB:
			return ByteCode::BYTE_CODE_STORE_SUB;
		case Operator::OPERATOR_ASSIGN_MUL:
			return ByteCode::BYTE_CODE_STORE_MUL;
		case Operator::OPERATOR_ASSIGN_DIV:
			return ByteCode::BYTE_CODE_STORE_DIV;
		case Operator::OPERATOR_ASSIGN_MOD:
			return ByteCode::BYTE_CODE_STORE_MOD;
		case Operator::OPERATOR_ASSIGN_XOR:
			return ByteCode::BYTE_CODE_STORE_BITWISE_XOR;
		case Operator::OPERATOR_ASSIGN_AND:
			return ByteCode::BYTE_CODE_STORE_BITWISE_AND;
		case Operator::OPERATOR_ASSIGN_OR:
			return ByteCode::BYTE_CODE_STORE_BITWISE_OR;
		case Operator::OPERATOR_ASSIGN_SHIFT_LEFT:
			return ByteCode::BYTE_CODE_STORE_SHL;
		case Operator::OPERATOR_ASSIGN_SHIFT_RIGHT:
			return ByteCode::BYTE_CODE_STORE_SHR;


		// arithmetic ops
		case Operator::OPERATOR_ADD:
			return ByteCode::BYTE_CODE_ADD;
		case Operator::OPERATOR_SUB:
			return ByteCode::BYTE_CODE_SUB;
		case Operator::OPERATOR_MUL:
			return ByteCode::BYTE_CODE_MUL;
		case Operator::OPERATOR_DIV:
			return ByteCode::BYTE_CODE_DIV;
		case Operator::OPERATOR_MOD:
			return ByteCode::BYTE_CODE_MOD;
		case Operator::OPERATOR_XOR:
			return ByteCode::BYTE_CODE_BITWISE_XOR;
		case Operator::OPERATOR_AND:
			return ByteCode::BYTE_CODE_BITWISE_AND;
		case Operator::OPERATOR_OR:
			return ByteCode::BYTE_CODE_BITWISE_OR;
		case Operator::OPERATOR_SHIFT_LEFT:
			return ByteCode::BYTE_CODE_SHL;
		case Operator::OPERATOR_SHIFT_RIGHT:
			return ByteCode::BYTE_CODE_SHR;

		// logic
		case Operator::OPERATOR_LOGIC_AND:
			return ByteCode::BYTE_CODE_LOGIC_AND;
		case Operator::OPERATOR_LOGIC_OR:
			return ByteCode::BYTE_CODE_LOGIC_OR;
		case Operator::OPERATOR_EQUAL:
			return ByteCode::BYTE_CODE_EQU;
		case Operator::OPERATOR_NOT_EQUAL:
			return ByteCode::BYTE_CODE_NOT_EQU;
		case Operator::OPERATOR_GT:
			return ByteCode::BYTE_CODE_GT;
		case Operator::OPERATOR_LT:
			return ByteCode::BYTE_CODE_LT;
		case Operator::OPERATOR_GTE:
			return ByteCode::BYTE_CODE_GTE;
		case Operator::OPERATOR_LTE:
			return ByteCode::BYTE_CODE_LTE;
		case Operator::OPERATOR_INSTANCEOF:
			return ByteCode::BYTE_CODE_INSTANCEOF;
		case Operator::OPERATOR_IN:
			return ByteCode::BYTE_CODE_IN;

		}


		return ByteCode::BYTE_CODE_INVALID;
	}



	const char * eval_operator_to_str(Operator op){
		switch(op){
		default:
			break;
		// assign and arithmetic with assign
		case Operator::OPERATOR_ASSIGN:
			return "=";
		case Operator::OPERATOR_ASSIGN_ADD:
			return "+=";
		case Operator::OPERATOR_ASSIGN_SUB:
			return "-=";
		case Operator::OPERATOR_ASSIGN_MUL:
			return "*=";
		case Operator::OPERATOR_ASSIGN_DIV:
			return "/=";
		case Operator::OPERATOR_ASSIGN_MOD:
			return "%=";
		case Operator::OPERATOR_ASSIGN_XOR:
			return "^=";
		case Operator::OPERATOR_ASSIGN_AND:
			return "&=";
		case Operator::OPERATOR_ASSIGN_OR:
			return "|=";
		case Operator::OPERATOR_ASSIGN_SHIFT_LEFT:
			return "<<=";
		case Operator::OPERATOR_ASSIGN_SHIFT_RIGHT:
			return ">>=";

		// arithmetic ops
		case Operator::OPERATOR_ADD:
			return "+";
		case Operator::OPERATOR_SUB:
			return "-";
		case Operator::OPERATOR_MUL:
			return "*";
		case Operator::OPERATOR_DIV:
			return "/";
		case Operator::OPERATOR_MOD:
			return "%";
		case Operator::OPERATOR_XOR:
			return "^";
		case Operator::OPERATOR_AND:
			return "&";
		case Operator::OPERATOR_OR:
			return "|";
		case Operator::OPERATOR_SHIFT_LEFT:
			return ">>";
		case Operator::OPERATOR_SHIFT_RIGHT:
			return "<<";

		// logic
		case Operator::OPERATOR_LOGIC_AND:
			return "&&";
		case Operator::OPERATOR_LOGIC_OR:
			return "||";
		case Operator::OPERATOR_EQUAL:
			return "==";
		case Operator::OPERATOR_NOT_EQUAL:
			return "!=";
		case Operator::OPERATOR_GT:
			return ">";
		case Operator::OPERATOR_LT:
			return "<";
		case Operator::OPERATOR_GTE:
			return ">=";
		case Operator::OPERATOR_LTE:
			return "<=";
		case Operator::OPERATOR_INSTANCEOF:
			return "instanceof";
		}

		return "unknow";
	}

}

