/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define PREFERENCE_OPERATOR_GROUP_0(_operator) \
(\
  (_operator) == OPERATOR_LOGIC_OR \
)

#define PREFERENCE_OPERATOR_GROUP_1(_operator) \
(\
  (_operator) == OPERATOR_LOGIC_AND \
)


#define PREFERENCE_OPERATOR_GROUP_2(_operator) \
(\
    (_operator) == OPERATOR_EQUAL \
|| 	(_operator) == OPERATOR_NOT_EQUAL \
|| 	(_operator) == OPERATOR_GTE \
|| 	(_operator) == OPERATOR_LTE \
|| 	(_operator) == OPERATOR_GT \
|| 	(_operator) == OPERATOR_LT \
)

#define PREFERENCE_OPERATOR_GROUP_3(_operator) \
( \
(_operator) == OPERATOR_ADD \
|| 	(_operator) == OPERATOR_SUB \
||	(_operator) == OPERATOR_IN \
|| 	(_operator) == OPERATOR_INSTANCEOF \
)

#define PREFERENCE_OPERATOR_GROUP_4(_operator) \
( \
(_operator) == OPERATOR_MUL \
|| 	(_operator) == OPERATOR_DIV \
||  (_operator) == OPERATOR_MOD \
)

#define PREFERENCE_OPERATOR_GROUP_5(_operator) \
( \
 	(_operator) == OPERATOR_OR \
|| 	(_operator) == OPERATOR_XOR \
)

#define PREFERENCE_OPERATOR_GROUP_6(_operator) \
( \
  (_operator) == OPERATOR_AND \
)


#define	PREFERENCE_OPERATOR_GROUP_7(_operator) \
(\
(_operator) == OPERATOR_SHIFT_LEFT \
|| 	(_operator) == OPERATOR_SHIFT_RIGHT \
)

#define PREFERENCE_OPERATOR_GROUP_MAX	7

namespace zetscript{

	// operator enumerated as its precedence
	typedef enum :uint8_t {

		OPERATOR_UNKNOWN = 0,

		// ASSIGN
		OPERATOR_ASSIGN, 										// =
		OPERATOR_ASSIGN_ADD,									// += (first)
		OPERATOR_ASSIGN_SUB, 									// -=
		OPERATOR_ASSIGN_MUL, 									// *=
		OPERATOR_ASSIGN_DIV, 									// /=
		OPERATOR_ASSIGN_MOD, 									// %=
		OPERATOR_ASSIGN_XOR,									// ^=
		OPERATOR_ASSIGN_AND,									// &=
		OPERATOR_ASSIGN_OR, 		 							// |=
		OPERATOR_ASSIGN_SHIFT_LEFT, 							// <<=
		OPERATOR_ASSIGN_SHIFT_RIGHT, 							// >>= (last)

		// TERNARY...
		OPERATOR_TERNARY_IF,	// ?
		OPERATOR_TERNARY_ELSE, 									// :

		// LOGIC...
		OPERATOR_LOGIC_AND,										// &&
		OPERATOR_LOGIC_OR, 										// ||

		// RELATIONAL
		OPERATOR_EQUAL, 										// ==
		OPERATOR_NOT_EQUAL, 									// !=
		OPERATOR_GTE, 											// >=
		OPERATOR_LTE, 											// <=
		OPERATOR_GT, 											// >
		OPERATOR_LT, 											// <

		// ARITHMETIC
		OPERATOR_ADD, 											// +
		OPERATOR_SUB, 											// -
		OPERATOR_XOR, 											// ^
		OPERATOR_MUL, 											// *
		OPERATOR_AND,		 									// &
		OPERATOR_DIV, 											// /
		OPERATOR_MOD, 											// %
		OPERATOR_OR, 											// |
		OPERATOR_SHIFT_LEFT, 									// <<
		OPERATOR_SHIFT_RIGHT, 									// >>

		OPERATOR_INSTANCEOF, 									// instanceof
		OPERATOR_IN,		 									// in

		OPERATOR_MAX
	}Operator;

}
