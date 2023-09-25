/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const char * metamethod_byte_code_to_operator_str(MetamethodByteCode op){
		switch (op) {
			case	ZS_METAMETHOD_BYTE_CODE_EQU:		return  "==";  		// ==
			case	ZS_METAMETHOD_BYTE_CODE_NEQU: 	return  "!=";  		// !=,
			case	ZS_METAMETHOD_BYTE_CODE_LT:		return  "<";  		// <
			case	ZS_METAMETHOD_BYTE_CODE_LTE:		return  "<=";  		// <=
			case	ZS_METAMETHOD_BYTE_CODE_NOT:		return  "!"; 		// !
			case	ZS_METAMETHOD_BYTE_CODE_GT:		return  ">";  		// >
			case	ZS_METAMETHOD_BYTE_CODE_GTE:		return  ">="; 		// >=
			case	ZS_METAMETHOD_BYTE_CODE_NEG:		return  "-"; 		// -a
			case	ZS_METAMETHOD_BYTE_CODE_BWC:		return  "~"; 		// ~a
			case	ZS_METAMETHOD_BYTE_CODE_ADD:		return  "+"; 		// +
			case	ZS_METAMETHOD_BYTE_CODE_SUB:		return  "-"; 		// -
			case	ZS_METAMETHOD_BYTE_CODE_DIV:		return  "/"; 		// /
			case	ZS_METAMETHOD_BYTE_CODE_MUL:		return  "*"; 		// *
			case	ZS_METAMETHOD_BYTE_CODE_MOD:		return  "%";  		// %
			case	ZS_METAMETHOD_BYTE_CODE_AND:		return  "&"; 		// bitwise and
			case	ZS_METAMETHOD_BYTE_CODE_OR:		return  "|";		// bitwise or
			case	ZS_METAMETHOD_BYTE_CODE_XOR:		return  "^"; 		// bitwise xor
			case	ZS_METAMETHOD_BYTE_CODE_SHL:		return  "<<"; 		// bitwise shift left
			case	ZS_METAMETHOD_BYTE_CODE_SHR:		return  ">>"; 		// bitwise shift right
			case	ZS_METAMETHOD_BYTE_CODE_SET:		return  "=";		// _set
			case	ZS_METAMETHOD_BYTE_CODE_ADD_ST:	return  "+=";		// _set_add
			case	ZS_METAMETHOD_BYTE_CODE_SUB_ST:	return  "-=";		// _set_sub
			case 	ZS_METAMETHOD_BYTE_CODE_MUL_ST:   return  "*=";		// _mulst
			case	ZS_METAMETHOD_BYTE_CODE_DIV_ST:	return  "/=";		// _set_div
			case	ZS_METAMETHOD_BYTE_CODE_MOD_ST:	return  "%=";		// _set_mod
			case	ZS_METAMETHOD_BYTE_CODE_AND_ST:	return  "&=";		// _set_and
			case	ZS_METAMETHOD_BYTE_CODE_OR_ST:	return  "|=";		// _set_or
			case	ZS_METAMETHOD_BYTE_CODE_XOR_ST:	return  "^=";		// _set_xor
			case	ZS_METAMETHOD_BYTE_CODE_SHL_ST:	return  "<<=";		// _set_shl
			case	ZS_METAMETHOD_BYTE_CODE_SHR_ST:	return  ">>=";		// _set_shr
			case	ZS_METAMETHOD_BYTE_CODE_BTWST:	return  "~=";		// _set_btw
			//case	ZS_METAMETHOD_BYTE_CODE_GET:		return  "_get";		// _get
			case	ZS_METAMETHOD_BYTE_CODE_TO_STRING: return  "_tostring";// _tostring
			case 	ZS_METAMETHOD_BYTE_CODE_POST_INC: 	return  "++";		// i++
			case 	ZS_METAMETHOD_BYTE_CODE_POST_DEC: 	return  "--";		//  i--
			case 	ZS_METAMETHOD_BYTE_CODE_PRE_INC: 	return  "++";		// ++i
			case 	ZS_METAMETHOD_BYTE_CODE_PRE_DEC: 	return  "--";		// --i
			case 	ZS_METAMETHOD_BYTE_CODE_IN: 		return  "in";		// in
			default:
				return "none";
		}
	}

	const char * metamethod_byte_code_to_symbol_str(MetamethodByteCode op){

		switch (op) {
		default:
			break;
			case	ZS_METAMETHOD_BYTE_CODE_EQU:		return  "_equ"; 	// ==
			case	ZS_METAMETHOD_BYTE_CODE_NEQU: 		return  "_nequ";  	// !=,
			case	ZS_METAMETHOD_BYTE_CODE_LT:			return  "_lt";  	// <
			case	ZS_METAMETHOD_BYTE_CODE_LTE:		return  "_lte";  	// <=
			case	ZS_METAMETHOD_BYTE_CODE_NOT:		return  "_not"; 	// !
			case	ZS_METAMETHOD_BYTE_CODE_GT:			return  "_gt";  	// >
			case	ZS_METAMETHOD_BYTE_CODE_GTE:		return  "_gte"; 	// >=
			case	ZS_METAMETHOD_BYTE_CODE_NEG:		return  "_neg"; 	// -a, !a
			case	ZS_METAMETHOD_BYTE_CODE_BWC:		return  "_bwc"; 	// ~a bitwise complement operator
			case	ZS_METAMETHOD_BYTE_CODE_ADD:		return  "_add"; 	// +
			case	ZS_METAMETHOD_BYTE_CODE_SUB:		return  "_sub"; 	// -
			case	ZS_METAMETHOD_BYTE_CODE_DIV:		return  "_div"; 	// /
			case	ZS_METAMETHOD_BYTE_CODE_MUL:		return  "_mul"; 	// *
			case	ZS_METAMETHOD_BYTE_CODE_MOD:		return  "_mod";  	// %
			case	ZS_METAMETHOD_BYTE_CODE_AND:		return  "_and"; 	// bitwise and
			case	ZS_METAMETHOD_BYTE_CODE_OR:			return  "_or"; 		// bitwise or
			case	ZS_METAMETHOD_BYTE_CODE_XOR:		return  "_xor"; 	// bitwise xor
			case	ZS_METAMETHOD_BYTE_CODE_SHL:		return  "_shl"; 	// bitwise shift left
			case	ZS_METAMETHOD_BYTE_CODE_SHR:		return  "_shr"; 	// bitwise shift right
			case	ZS_METAMETHOD_BYTE_CODE_SET:		return  "_set"; 	// _set
			case	ZS_METAMETHOD_BYTE_CODE_ADD_ST:		return  "_addst";	// _set_add
			case	ZS_METAMETHOD_BYTE_CODE_SUB_ST:		return  "_subst";	// _set_sub
			case	ZS_METAMETHOD_BYTE_CODE_MUL_ST:		return  "_mulst";	// _mulst
			case	ZS_METAMETHOD_BYTE_CODE_DIV_ST:		return  "_divst";	// _set_div
			case	ZS_METAMETHOD_BYTE_CODE_MOD_ST:		return  "_modst";	// _set_mod
			case	ZS_METAMETHOD_BYTE_CODE_AND_ST:		return  "_andst";	// _set_and
			case	ZS_METAMETHOD_BYTE_CODE_OR_ST:		return  "_orst"; 	// _set_or
			case	ZS_METAMETHOD_BYTE_CODE_XOR_ST:		return  "_xorst";	// _set_xor
			case	ZS_METAMETHOD_BYTE_CODE_SHL_ST:		return  "_shlst";	// _set_shl
			case	ZS_METAMETHOD_BYTE_CODE_SHR_ST:		return  "_shrst";	// _set_shr
			case	ZS_METAMETHOD_BYTE_CODE_BTWST:		return  "_btwst";	// _set_btw

			//case	ZS_METAMETHOD_BYTE_CODE_GET:		return  "_get"; 	// _get
			case	ZS_METAMETHOD_BYTE_CODE_TO_STRING:	return  "_tostring";// _tostring
			case 	ZS_METAMETHOD_BYTE_CODE_POST_INC: 	return  "_postinc";// i++
			case 	ZS_METAMETHOD_BYTE_CODE_POST_DEC:	return  "_postdec";// i--
			case 	ZS_METAMETHOD_BYTE_CODE_PRE_INC: 	return  "_preinc";	// ++i
			case 	ZS_METAMETHOD_BYTE_CODE_PRE_DEC:	return  "_predec";	// --i
			case 	ZS_METAMETHOD_BYTE_CODE_IN: 		return  "_in";		// 	_in
		}

		return "none";
	}

	int			 metamethod_byte_code_get_num_arguments(MetamethodByteCode op){
		switch(op){
		default:
			break;
			case ZS_METAMETHOD_BYTE_CODE_TO_STRING:
			case ZS_METAMETHOD_BYTE_CODE_POST_INC:
			case ZS_METAMETHOD_BYTE_CODE_POST_DEC:
			case ZS_METAMETHOD_BYTE_CODE_PRE_INC:
			case ZS_METAMETHOD_BYTE_CODE_PRE_DEC:
			case ZS_METAMETHOD_BYTE_CODE_NOT:
			case ZS_METAMETHOD_BYTE_CODE_NEG:
			case ZS_METAMETHOD_BYTE_CODE_BWC:
				return 0;
			case ZS_METAMETHOD_BYTE_CODE_ADD_ST:
			case ZS_METAMETHOD_BYTE_CODE_SUB_ST:
			case ZS_METAMETHOD_BYTE_CODE_MUL_ST:
			case ZS_METAMETHOD_BYTE_CODE_DIV_ST:
			case ZS_METAMETHOD_BYTE_CODE_MOD_ST:
			case ZS_METAMETHOD_BYTE_CODE_AND_ST:
			case ZS_METAMETHOD_BYTE_CODE_OR_ST:
			case ZS_METAMETHOD_BYTE_CODE_XOR_ST:
			case ZS_METAMETHOD_BYTE_CODE_SHL_ST:
			case ZS_METAMETHOD_BYTE_CODE_SHR_ST:
			case ZS_METAMETHOD_BYTE_CODE_SET:
			case ZS_METAMETHOD_BYTE_CODE_IN:
				return 1;
		}

		return 2;
	}

	bool		 metamethod_byte_code_should_be_static(MetamethodByteCode op){

		switch(op){
		case ZS_METAMETHOD_BYTE_CODE_EQU:
		case ZS_METAMETHOD_BYTE_CODE_NEQU:
		case ZS_METAMETHOD_BYTE_CODE_LT:
		case ZS_METAMETHOD_BYTE_CODE_LTE:
		case ZS_METAMETHOD_BYTE_CODE_GT:
		case ZS_METAMETHOD_BYTE_CODE_GTE:
		case ZS_METAMETHOD_BYTE_CODE_ADD:
		case ZS_METAMETHOD_BYTE_CODE_SUB:
		case ZS_METAMETHOD_BYTE_CODE_DIV:
		case ZS_METAMETHOD_BYTE_CODE_MUL:
		case ZS_METAMETHOD_BYTE_CODE_MOD:
		case ZS_METAMETHOD_BYTE_CODE_AND:
		case ZS_METAMETHOD_BYTE_CODE_OR:
		case ZS_METAMETHOD_BYTE_CODE_XOR:
		case ZS_METAMETHOD_BYTE_CODE_SHL:
		case ZS_METAMETHOD_BYTE_CODE_SHR:
			return true;
			break;
		default:
			break;
		}
		return false;
	}
}
