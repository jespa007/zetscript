#pragma once

#define OPERATOR_GROUP_0(operator_type) \
(\
(operator_type) == OPERATOR_LOGIC_AND \
|| 	(operator_type) == OPERATOR_LOGIC_OR \
)



#define OPERATOR_GROUP_1(operator_type) \
(\
(operator_type) == OPERATOR_EQUAL \
|| 	(operator_type) == OPERATOR_NOT_EQUAL \
|| 	(operator_type) == OPERATOR_GTE \
|| 	(operator_type) == OPERATOR_LTE \
|| 	(operator_type) == OPERATOR_GT \
|| 	(operator_type) == OPERATOR_LT \
|| 	(operator_type) == OPERATOR_IN \
|| 	(operator_type) == OPERATOR_INSTANCEOF \
)

#define OPERATOR_GROUP_2(operator_type) \
( \
(operator_type) == OPERATOR_ADD \
|| 	(operator_type) == OPERATOR_SUB \
)

#define OPERATOR_GROUP_3(operator_type) \
( \
(operator_type) == OPERATOR_MUL \
|| 	(operator_type) == OPERATOR_DIV \
||  (operator_type) == OPERATOR_MOD \
)

#define OPERATOR_GROUP_4(operator_type) \
( \
(operator_type) == OPERATOR_AND \
|| 	(operator_type) == OPERATOR_OR \
|| 	(operator_type) == OPERATOR_XOR \
)

#define	OPERATOR_GROUP_5(operator_type) \
(\
(operator_type) == OPERATOR_SHIFT_LEFT \
|| 	(operator_type) == OPERATOR_SHIFT_RIGHT \
)

#define OPERATOR_GROUP_MAX	5

namespace zetscript{

	// operator enumerated as its precedence
	typedef enum :unsigned char {

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
		OPERATOR_EQUAL, 									// ==
		OPERATOR_NOT_EQUAL, 								// !=
		OPERATOR_GTE, 									// >=
		OPERATOR_LTE, 									// <=
		OPERATOR_GT, 										// >
		OPERATOR_LT, 										// <

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

};
