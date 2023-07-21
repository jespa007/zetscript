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
		METAMETHOD_BYTE_CODE_GET=254,  // ==
		METAMETHOD_BYTE_CODE_INVALID=255,  // ==
		METAMETHOD_BYTE_CODE_EQU=0,  // ==
		METAMETHOD_BYTE_CODE_NEQU,  // !=
		METAMETHOD_BYTE_CODE_LT,  // <
		METAMETHOD_BYTE_CODE_LTE,  // <=
		METAMETHOD_BYTE_CODE_GT,  // >
		METAMETHOD_BYTE_CODE_GTE, // >=
		METAMETHOD_BYTE_CODE_ADD, // +
		METAMETHOD_BYTE_CODE_SUB, // -
		METAMETHOD_BYTE_CODE_DIV, // /
		METAMETHOD_BYTE_CODE_MUL, // *
		METAMETHOD_BYTE_CODE_MOD,  // %
		METAMETHOD_BYTE_CODE_AND, // & bitwise logic and
		METAMETHOD_BYTE_CODE_OR, // | bitwise logic or
		METAMETHOD_BYTE_CODE_XOR, // ^ logic xor
		METAMETHOD_BYTE_CODE_SHL, // << shift left
		METAMETHOD_BYTE_CODE_SHR, // >> shift right
		METAMETHOD_BYTE_CODE_ADD_SET, // set '+='
		METAMETHOD_BYTE_CODE_SUB_SET, // set '-='
		METAMETHOD_BYTE_CODE_MUL_SET, // set '*='
		METAMETHOD_BYTE_CODE_DIV_SET, // set '/='
		METAMETHOD_BYTE_CODE_MOD_SET, // set '%='
		METAMETHOD_BYTE_CODE_AND_SET, // set '&='
		METAMETHOD_BYTE_CODE_OR_SET,  // set '|='
		METAMETHOD_BYTE_CODE_XOR_SET, // set '^='
		METAMETHOD_BYTE_CODE_SHL_SET, // set '<<'
		METAMETHOD_BYTE_CODE_SHR_SET, // set '>>'
		METAMETHOD_BYTE_CODE_BTW_SET, // set '~'

		//METAMETHOD_BYTE_CODE_GET, // get '='
		METAMETHOD_BYTE_CODE_NOT, // !
		METAMETHOD_BYTE_CODE_NEG, // -a
		METAMETHOD_BYTE_CODE_BWC, // ~a
		METAMETHOD_BYTE_CODE_SET, // set '='
		METAMETHOD_BYTE_CODE_TO_STRING, // toString
		METAMETHOD_BYTE_CODE_POST_INC, // i++
		METAMETHOD_BYTE_CODE_POST_DEC, // i--
		METAMETHOD_BYTE_CODE_PRE_INC, // ++i
		METAMETHOD_BYTE_CODE_PRE_DEC, // --i
		METAMETHOD_BYTE_CODE_IN, // special metamethod in
		MAX_METAMETHOD_BYTE_CODES,
	}MetamethodByteCode;

	const char * metamethod_byte_code_to_operator_str(MetamethodByteCode op);
	const char * metamethod_byte_code_to_symbol_str(MetamethodByteCode op);
	int			 metamethod_byte_code_get_num_arguments(MetamethodByteCode op);
	bool		 metamethod_byte_code_should_be_static(MetamethodByteCode op);

}
