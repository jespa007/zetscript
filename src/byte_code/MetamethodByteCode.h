/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_METAMETHOD_BYTE_CODE_SYMBOL_NAME_GETTER		"_get"
#define ZS_IS_METAMETHOD_BYTE_CODE_OPERATOR_WITH_TWO_ARGS()

namespace zetscript{

	typedef enum: uint8_t {
		// static
		ZS_METAMETHOD_BYTE_CODE_GET=254,  // ==
		ZS_METAMETHOD_BYTE_CODE_INVALID=255,  // ==
		ZS_METAMETHOD_BYTE_CODE_EQU=0,  // ==
		ZS_METAMETHOD_BYTE_CODE_NEQU,  // !=
		ZS_METAMETHOD_BYTE_CODE_LT,  // <
		ZS_METAMETHOD_BYTE_CODE_LTE,  // <=
		ZS_METAMETHOD_BYTE_CODE_GT,  // >
		ZS_METAMETHOD_BYTE_CODE_GTE, // >=
		ZS_METAMETHOD_BYTE_CODE_ADD, // +
		ZS_METAMETHOD_BYTE_CODE_SUB, // -
		ZS_METAMETHOD_BYTE_CODE_DIV, // /
		ZS_METAMETHOD_BYTE_CODE_MUL, // *
		ZS_METAMETHOD_BYTE_CODE_MOD,  // %
		ZS_METAMETHOD_BYTE_CODE_AND, // & bitwise logic and
		ZS_METAMETHOD_BYTE_CODE_OR, // | bitwise logic or
		ZS_METAMETHOD_BYTE_CODE_XOR, // ^ logic xor
		ZS_METAMETHOD_BYTE_CODE_SHL, // << shift left
		ZS_METAMETHOD_BYTE_CODE_SHR, // >> shift right
		ZS_METAMETHOD_BYTE_CODE_ADD_ST, // set '+='
		ZS_METAMETHOD_BYTE_CODE_SUB_ST, // set '-='
		ZS_METAMETHOD_BYTE_CODE_MUL_ST, // set '*='
		ZS_METAMETHOD_BYTE_CODE_DIV_ST, // set '/='
		ZS_METAMETHOD_BYTE_CODE_MOD_ST, // set '%='
		ZS_METAMETHOD_BYTE_CODE_AND_ST, // set '&='
		ZS_METAMETHOD_BYTE_CODE_OR_ST,  // set '|='
		ZS_METAMETHOD_BYTE_CODE_XOR_ST, // set '^='
		ZS_METAMETHOD_BYTE_CODE_SHL_ST, // set '<<'
		ZS_METAMETHOD_BYTE_CODE_SHR_ST, // set '>>'
		ZS_METAMETHOD_BYTE_CODE_BTWST, // set '~'

		//ZS_METAMETHOD_BYTE_CODE_GET, // get '='
		ZS_METAMETHOD_BYTE_CODE_NOT, // !
		ZS_METAMETHOD_BYTE_CODE_NEG, // -a
		ZS_METAMETHOD_BYTE_CODE_BWC, // ~a
		ZS_METAMETHOD_BYTE_CODE_SET, // set '='
		ZS_METAMETHOD_BYTE_CODE_TO_STRING, // toString
		ZS_METAMETHOD_BYTE_CODE_POST_INC, // i++
		ZS_METAMETHOD_BYTE_CODE_POST_DEC, // i--
		ZS_METAMETHOD_BYTE_CODE_PRE_INC, // ++i
		ZS_METAMETHOD_BYTE_CODE_PRE_DEC, // --i
		ZS_METAMETHOD_BYTE_CODE_IN, // special metamethod in
		ZS_MAX_METAMETHOD_BYTE_CODES,
	}MetamethodByteCode;

	const char * metamethod_byte_code_to_operator_str(MetamethodByteCode op);
	const char * metamethod_byte_code_to_symbol_str(MetamethodByteCode op);
	int			 metamethod_byte_code_get_num_arguments(MetamethodByteCode op);
	bool		 metamethod_byte_code_should_be_static(MetamethodByteCode op);

}
