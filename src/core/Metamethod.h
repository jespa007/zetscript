/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define METAMETHOD_SYMBOL_NAME_GETTER		"_get"

namespace zetscript{

	typedef enum: uint8_t {
		METAMETHOD_GET=254,  // ==
		METAMETHOD_INVALID=255,  // ==
		METAMETHOD_EQU=0,  // ==
		METAMETHOD_NEQU,  // !=
		METAMETHOD_LT,  // <
		METAMETHOD_LTE,  // <=
		METAMETHOD_GT,  // >
		METAMETHOD_GTE, // >=
		METAMETHOD_ADD, // +
		METAMETHOD_SUB, // -
		METAMETHOD_DIV, // /
		METAMETHOD_MUL, // *
		METAMETHOD_MOD,  // %
		METAMETHOD_AND, // & bitwise logic and
		METAMETHOD_OR, // | bitwise logic or
		METAMETHOD_XOR, // ^ logic xor
		METAMETHOD_SHL, // << shift left
		METAMETHOD_SHR, // >> shift right
		METAMETHOD_ADD_ASSIGN, // set '+='
		METAMETHOD_SUB_ASSIGN, // set '-='
		METAMETHOD_MUL_ASSIGN, // set '*='
		METAMETHOD_DIV_ASSIGN, // set '/='
		METAMETHOD_MOD_ASSIGN, // set '%='
		METAMETHOD_AND_ASSIGN, // set '&='
		METAMETHOD_OR_ASSIGN,  // set '|='
		METAMETHOD_XOR_ASSIGN, // set '^='
		METAMETHOD_SHL_ASSIGN, // set '<<'
		METAMETHOD_SHR_ASSIGN, // set '>>'
		METAMETHOD_BTWST, // set '~'

		//METAMETHOD_GET, // get '='
		METAMETHOD_NOT, // !
		METAMETHOD_NEG, // -a
		METAMETHOD_BWC, // ~a
		METAMETHOD_SET, // set '='
		METAMETHOD_TO_STRING, // toString
		METAMETHOD_POST_INC, // i++
		METAMETHOD_POST_DEC, // i--
		METAMETHOD_PRE_INC, // ++i
		METAMETHOD_PRE_DEC, // --i
		METAMETHOD_IN, // special metamethod in
		MAX_METAMETHOD_IDS,
	}Metamethod;
}
