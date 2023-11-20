/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_METAMETHOD_ID_SYMBOL_NAME_GETTER		"_get"

namespace zetscript{

	class Metamethod{
	public:

		typedef enum: uint8_t {
			// static
			METAMETHOD_ID_GET=254,  // ==
			METAMETHOD_ID_INVALID=255,  // ==
			METAMETHOD_ID_EQU=0,  // ==
			METAMETHOD_ID_NEQU,  // !=
			METAMETHOD_ID_LT,  // <
			METAMETHOD_ID_LTE,  // <=
			METAMETHOD_ID_GT,  // >
			METAMETHOD_ID_GTE, // >=
			METAMETHOD_ID_ADD, // +
			METAMETHOD_ID_SUB, // -
			METAMETHOD_ID_DIV, // /
			METAMETHOD_ID_MUL, // *
			METAMETHOD_ID_MOD,  // %
			METAMETHOD_ID_AND, // & bitwise logic and
			METAMETHOD_ID_OR, // | bitwise logic or
			METAMETHOD_ID_XOR, // ^ logic xor
			METAMETHOD_ID_SHL, // << shift left
			METAMETHOD_ID_SHR, // >> shift right
			METAMETHOD_ID_ADD_ASSIGN, // set '+='
			METAMETHOD_ID_SUB_ASSIGN, // set '-='
			METAMETHOD_ID_MUL_ASSIGN, // set '*='
			METAMETHOD_ID_DIV_ASSIGN, // set '/='
			METAMETHOD_ID_MOD_ASSIGN, // set '%='
			METAMETHOD_ID_AND_ASSIGN, // set '&='
			METAMETHOD_ID_OR_ASSIGN,  // set '|='
			METAMETHOD_ID_XOR_ASSIGN, // set '^='
			METAMETHOD_ID_SHL_ASSIGN, // set '<<'
			METAMETHOD_ID_SHR_ASSIGN, // set '>>'
			METAMETHOD_ID_BTWST, // set '~'

			//METAMETHOD_ID_GET, // get '='
			METAMETHOD_ID_NOT, // !
			METAMETHOD_ID_NEG, // -a
			METAMETHOD_ID_BWC, // ~a
			METAMETHOD_ID_SET, // set '='
			METAMETHOD_ID_TO_STRING, // toString
			METAMETHOD_ID_POST_INC, // i++
			METAMETHOD_ID_POST_DEC, // i--
			METAMETHOD_ID_PRE_INC, // ++i
			METAMETHOD_ID_PRE_DEC, // --i
			METAMETHOD_ID_IN, // special metamethod in
			MAX_METAMETHOD_IDS,
		}MetamethodId;

		static const char * toOperatorString(MetamethodId op);
		static const char * toSymbolString(MetamethodId op);
		static int			getNumberArguments(MetamethodId op);
		static bool		 	shouldBeStatic(MetamethodId op);
	};

}
