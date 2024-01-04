/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

// group 0 is the most candidate split it will do in the compiler expression (less priority)
#define ZS_PREFERENCE_OPERATOR_GROUP_0(_operator) \
(\
  (_operator) == ZS_OPERATOR_LOGIC_OR \
)

#define ZS_PREFERENCE_OPERATOR_GROUP_1(_operator) \
(\
  (_operator) == ZS_OPERATOR_LOGIC_AND \
)


#define ZS_PREFERENCE_OPERATOR_GROUP_2(_operator) \
(\
    (_operator) == ZS_OPERATOR_EQUAL \
|| 	(_operator) == ZS_OPERATOR_NOT_EQUAL \
|| 	(_operator) == ZS_OPERATOR_GTE \
|| 	(_operator) == ZS_OPERATOR_LTE \
|| 	(_operator) == ZS_OPERATOR_GT \
|| 	(_operator) == ZS_OPERATOR_LT \
)

#define ZS_PREFERENCE_OPERATOR_GROUP_3(_operator) \
( \
    (_operator) == ZS_OPERATOR_ADD \
|| 	(_operator) == ZS_OPERATOR_SUB \
)

#define ZS_PREFERENCE_OPERATOR_GROUP_4(_operator) \
( \
(_operator) == ZS_OPERATOR_MUL \
|| 	(_operator) == ZS_OPERATOR_DIV \
||  (_operator) == ZS_OPERATOR_MOD \
)

#define ZS_PREFERENCE_OPERATOR_GROUP_5(_operator) \
( \
 	(_operator) == ZS_OPERATOR_OR \
|| 	(_operator) == ZS_OPERATOR_XOR \
)

#define ZS_PREFERENCE_OPERATOR_GROUP_6(_operator) \
( \
  (_operator) == ZS_OPERATOR_AND \
)


#define	ZS_PREFERENCE_OPERATOR_GROUP_7(_operator) \
(\
(_operator) == ZS_OPERATOR_SHIFT_LEFT \
|| 	(_operator) == ZS_OPERATOR_SHIFT_RIGHT \
)


#define	ZS_PREFERENCE_OPERATOR_GROUP_8(_operator) \
(\
	(_operator) == ZS_OPERATOR_IN \
|| 	(_operator) == ZS_OPERATOR_INSTANCEOF \
)
// group 8 is the less candidate split it will do in the compiler expression (more priority)

#define ZS_PREFERENCE_OPERATOR_GROUP_MAX	8

namespace zetscript{

	// operator enumerated as its precedence
	typedef enum :uint8_t {

		ZS_OPERATOR_UNKNOWN = 0,

		// ASSIGN
		ZS_OPERATOR_ASSIGN, 										// =
		ZS_OPERATOR_ASSIGN_ADD,									// += (first)
		ZS_OPERATOR_ASSIGN_SUB, 									// -=
		ZS_OPERATOR_ASSIGN_MUL, 									// *=
		ZS_OPERATOR_ASSIGN_DIV, 									// /=
		ZS_OPERATOR_ASSIGN_MOD, 									// %=
		ZS_OPERATOR_ASSIGN_XOR,									// ^=
		ZS_OPERATOR_ASSIGN_AND,									// &=
		ZS_OPERATOR_ASSIGN_OR, 		 							// |=
		ZS_OPERATOR_ASSIGN_SHIFT_LEFT, 							// <<=
		ZS_OPERATOR_ASSIGN_SHIFT_RIGHT, 							// >>= (last)

		// TERNARY...
		ZS_OPERATOR_TERNARY_IF,	// ?
		ZS_OPERATOR_TERNARY_ELSE, 									// :

		// LOGIC...
		ZS_OPERATOR_LOGIC_AND,										// &&
		ZS_OPERATOR_LOGIC_OR, 										// ||

		// RELATIONAL
		ZS_OPERATOR_EQUAL, 										// ==
		ZS_OPERATOR_NOT_EQUAL, 									// !=
		ZS_OPERATOR_GTE, 											// >=
		ZS_OPERATOR_LTE, 											// <=
		ZS_OPERATOR_GT, 											// >
		ZS_OPERATOR_LT, 											// <

		// ARITHMETIC
		ZS_OPERATOR_ADD, 											// +
		ZS_OPERATOR_SUB, 											// -
		ZS_OPERATOR_XOR, 											// ^
		ZS_OPERATOR_MUL, 											// *
		ZS_OPERATOR_AND,		 									// &
		ZS_OPERATOR_DIV, 											// /
		ZS_OPERATOR_MOD, 											// %
		ZS_OPERATOR_OR, 											// |
		ZS_OPERATOR_SHIFT_LEFT, 									// <<
		ZS_OPERATOR_SHIFT_RIGHT, 									// >>

		ZS_OPERATOR_INSTANCEOF, 									// instanceof
		ZS_OPERATOR_IN,		 									// in

		ZS_OPERATOR_MAX
	}Operator;

}
