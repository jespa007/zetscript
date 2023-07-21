/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const char * metamethod_byte_code_to_operator_str(MetamethodByteCode op){
		switch (op) {
			case	METAMETHOD_BYTE_CODE_EQU:		return  "==";  		// ==
			case	METAMETHOD_BYTE_CODE_NEQU: 	return  "!=";  		// !=,
			case	METAMETHOD_BYTE_CODE_LT:		return  "<";  		// <
			case	METAMETHOD_BYTE_CODE_LTE:		return  "<=";  		// <=
			case	METAMETHOD_BYTE_CODE_NOT:		return  "!"; 		// !
			case	METAMETHOD_BYTE_CODE_GT:		return  ">";  		// >
			case	METAMETHOD_BYTE_CODE_GTE:		return  ">="; 		// >=
			case	METAMETHOD_BYTE_CODE_NEG:		return  "-"; 		// -a
			case	METAMETHOD_BYTE_CODE_BWC:		return  "~"; 		// ~a
			case	METAMETHOD_BYTE_CODE_ADD:		return  "+"; 		// +
			case	METAMETHOD_BYTE_CODE_SUB:		return  "-"; 		// -
			case	METAMETHOD_BYTE_CODE_DIV:		return  "/"; 		// /
			case	METAMETHOD_BYTE_CODE_MUL:		return  "*"; 		// *
			case	METAMETHOD_BYTE_CODE_MOD:		return  "%";  		// %
			case	METAMETHOD_BYTE_CODE_AND:		return  "&"; 		// bitwise and
			case	METAMETHOD_BYTE_CODE_OR:		return  "|";		// bitwise or
			case	METAMETHOD_BYTE_CODE_XOR:		return  "^"; 		// bitwise xor
			case	METAMETHOD_BYTE_CODE_SHL:		return  "<<"; 		// bitwise shift left
			case	METAMETHOD_BYTE_CODE_SHR:		return  ">>"; 		// bitwise shift right
			case	METAMETHOD_BYTE_CODE_SET:		return  "=";		// _set
			case	METAMETHOD_BYTE_CODE_ADD_SET:	return  "+=";		// _set_add
			case	METAMETHOD_BYTE_CODE_SUB_SET:	return  "-=";		// _set_sub
			case 	METAMETHOD_BYTE_CODE_MUL_SET:   return  "*=";		// _mulset
			case	METAMETHOD_BYTE_CODE_DIV_SET:	return  "/=";		// _set_div
			case	METAMETHOD_BYTE_CODE_MOD_SET:	return  "%=";		// _set_mod
			case	METAMETHOD_BYTE_CODE_AND_SET:	return  "&=";		// _set_and
			case	METAMETHOD_BYTE_CODE_OR_SET:	return  "|=";		// _set_or
			case	METAMETHOD_BYTE_CODE_XOR_SET:	return  "^=";		// _set_xor
			case	METAMETHOD_BYTE_CODE_SHL_SET:	return  "<<=";		// _set_shl
			case	METAMETHOD_BYTE_CODE_SHR_SET:	return  ">>=";		// _set_shr
			case	METAMETHOD_BYTE_CODE_BTW_SET:	return  "~=";		// _set_btw
			//case	METAMETHOD_BYTE_CODE_GET:		return  "_get";		// _get
			case	METAMETHOD_BYTE_CODE_TO_STRING: return  "_tostring";// _tostring
			case 	METAMETHOD_BYTE_CODE_POST_INC: 	return  "++";		// i++
			case 	METAMETHOD_BYTE_CODE_POST_DEC: 	return  "--";		//  i--
			case 	METAMETHOD_BYTE_CODE_PRE_INC: 	return  "++";		// ++i
			case 	METAMETHOD_BYTE_CODE_PRE_DEC: 	return  "--";		// --i
			case 	METAMETHOD_BYTE_CODE_IN: 		return  "in";		// in
			default:
				return "none";
		}
	}

	const char * metamethod_byte_code_to_symbol_str(MetamethodByteCode op){

		switch (op) {
		default:
			break;
			case	METAMETHOD_BYTE_CODE_EQU:		return  "_equ"; 	// ==
			case	METAMETHOD_BYTE_CODE_NEQU: 	return  "_nequ";  	// !=,
			case	METAMETHOD_BYTE_CODE_LT:		return  "_lt";  	// <
			case	METAMETHOD_BYTE_CODE_LTE:		return  "_lte";  	// <=
			case	METAMETHOD_BYTE_CODE_NOT:		return  "_not"; 	// !
			case	METAMETHOD_BYTE_CODE_GT:		return  "_gt";  	// >
			case	METAMETHOD_BYTE_CODE_GTE:		return  "_gte"; 	// >=
			case	METAMETHOD_BYTE_CODE_NEG:		return  "_neg"; 	// -a, !a
			case	METAMETHOD_BYTE_CODE_BWC:		return  "_bwc"; 	// ~a bitwise complement operator
			case	METAMETHOD_BYTE_CODE_ADD:		return  "_add"; 	// +
			case	METAMETHOD_BYTE_CODE_SUB:		return  "_sub"; 	// -
			case	METAMETHOD_BYTE_CODE_DIV:		return  "_div"; 	// /
			case	METAMETHOD_BYTE_CODE_MUL:		return  "_mul"; 	// *
			case	METAMETHOD_BYTE_CODE_MOD:		return  "_mod";  	// %
			case	METAMETHOD_BYTE_CODE_AND:		return  "_and"; 	// bitwise and
			case	METAMETHOD_BYTE_CODE_OR:		return  "_or"; 		// bitwise or
			case	METAMETHOD_BYTE_CODE_XOR:		return  "_xor"; 	// bitwise xor
			case	METAMETHOD_BYTE_CODE_SHL:		return  "_shl"; 	// bitwise shift left
			case	METAMETHOD_BYTE_CODE_SHR:		return  "_shr"; 	// bitwise shift right
			case	METAMETHOD_BYTE_CODE_SET:		return  "_set"; 	// _set
			case	METAMETHOD_BYTE_CODE_ADD_SET:	return  "_addset";	// _set_add
			case	METAMETHOD_BYTE_CODE_SUB_SET:	return  "_subset";	// _set_sub
			case	METAMETHOD_BYTE_CODE_MUL_SET:	return  "_mulset";	// _mulset
			case	METAMETHOD_BYTE_CODE_DIV_SET:	return  "_divset";	// _set_div
			case	METAMETHOD_BYTE_CODE_MOD_SET:	return  "_modset";	// _set_mod
			case	METAMETHOD_BYTE_CODE_AND_SET:	return  "_andset";	// _set_and
			case	METAMETHOD_BYTE_CODE_OR_SET:	return  "_orset"; 	// _set_or
			case	METAMETHOD_BYTE_CODE_XOR_SET:	return  "_xorset";	// _set_xor
			case	METAMETHOD_BYTE_CODE_SHL_SET:	return  "_shlset";	// _set_shl
			case	METAMETHOD_BYTE_CODE_SHR_SET:	return  "_shrset";	// _set_shr
			case	METAMETHOD_BYTE_CODE_BTW_SET:	return  "_btw_set";	// _set_btw

			//case	METAMETHOD_BYTE_CODE_GET:		return  "_get"; 	// _get
			case	METAMETHOD_BYTE_CODE_TO_STRING:	return  "_tostring";// _tostring
			case 	METAMETHOD_BYTE_CODE_POST_INC: 	return  "_postinc";// i++
			case 	METAMETHOD_BYTE_CODE_POST_DEC:	return  "_postdec";// i--
			case 	METAMETHOD_BYTE_CODE_PRE_INC: 	return  "_preinc";	// ++i
			case 	METAMETHOD_BYTE_CODE_PRE_DEC:	return  "_predec";	// --i
			case 	METAMETHOD_BYTE_CODE_IN: 		return  "_in";		// 	_in
		}

		return "none";
	}

	int			 metamethod_byte_code_get_num_arguments(MetamethodByteCode op){
		switch(op){
		default:
			break;
			case METAMETHOD_BYTE_CODE_TO_STRING:
			case METAMETHOD_BYTE_CODE_POST_INC:
			case METAMETHOD_BYTE_CODE_POST_DEC:
			case METAMETHOD_BYTE_CODE_PRE_INC:
			case METAMETHOD_BYTE_CODE_PRE_DEC:
			case METAMETHOD_BYTE_CODE_NOT:
			case METAMETHOD_BYTE_CODE_NEG:
			case METAMETHOD_BYTE_CODE_BWC:
				return 0;
			case METAMETHOD_BYTE_CODE_ADD_SET:
			case METAMETHOD_BYTE_CODE_SUB_SET:
			case METAMETHOD_BYTE_CODE_MUL_SET:
			case METAMETHOD_BYTE_CODE_DIV_SET:
			case METAMETHOD_BYTE_CODE_MOD_SET:
			case METAMETHOD_BYTE_CODE_AND_SET:
			case METAMETHOD_BYTE_CODE_OR_SET:
			case METAMETHOD_BYTE_CODE_XOR_SET:
			case METAMETHOD_BYTE_CODE_SHL_SET:
			case METAMETHOD_BYTE_CODE_SHR_SET:
			case METAMETHOD_BYTE_CODE_SET:
			case METAMETHOD_BYTE_CODE_IN:
				return 1;
		}

		return 2;
	}

	bool		 metamethod_byte_code_should_be_static(MetamethodByteCode op){

		switch(op){
		case METAMETHOD_BYTE_CODE_EQU:
		case METAMETHOD_BYTE_CODE_NEQU:
		case METAMETHOD_BYTE_CODE_LT:
		case METAMETHOD_BYTE_CODE_LTE:
		case METAMETHOD_BYTE_CODE_GT:
		case METAMETHOD_BYTE_CODE_GTE:
		case METAMETHOD_BYTE_CODE_ADD:
		case METAMETHOD_BYTE_CODE_SUB:
		case METAMETHOD_BYTE_CODE_DIV:
		case METAMETHOD_BYTE_CODE_MUL:
		case METAMETHOD_BYTE_CODE_MOD:
		case METAMETHOD_BYTE_CODE_AND:
		case METAMETHOD_BYTE_CODE_OR:
		case METAMETHOD_BYTE_CODE_XOR:
		case METAMETHOD_BYTE_CODE_SHL:
		case METAMETHOD_BYTE_CODE_SHR:
			return true;
			break;
		default:
			break;
		}
		return false;
	}
}
