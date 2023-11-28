/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const char * MetamethodHelper::getMetamethodOperatorName(Metamethod _metamethod){
		switch (_metamethod) {
			case	METAMETHOD_EQU:		return  "==";  		// ==
			case	METAMETHOD_NEQU: 	return  "!=";  		// !=,
			case	METAMETHOD_LT:		return  "<";  		// <
			case	METAMETHOD_LTE:		return  "<=";  		// <=
			case	METAMETHOD_NOT:		return  "!"; 		// !
			case	METAMETHOD_GT:		return  ">";  		// >
			case	METAMETHOD_GTE:		return  ">="; 		// >=
			case	METAMETHOD_NEG:		return  "-"; 		// -a
			case	METAMETHOD_BWC:		return  "~"; 		// ~a
			case	METAMETHOD_ADD:		return  "+"; 		// +
			case	METAMETHOD_SUB:		return  "-"; 		// -
			case	METAMETHOD_DIV:		return  "/"; 		// /
			case	METAMETHOD_MUL:		return  "*"; 		// *
			case	METAMETHOD_MOD:		return  "%";  		// %
			case	METAMETHOD_AND:		return  "&"; 		// bitwise and
			case	METAMETHOD_OR:		return  "|";		// bitwise or
			case	METAMETHOD_XOR:		return  "^"; 		// bitwise xor
			case	METAMETHOD_SHL:		return  "<<"; 		// bitwise shift left
			case	METAMETHOD_SHR:		return  ">>"; 		// bitwise shift right
			case	METAMETHOD_SET:		return  "=";		// _set
			case	METAMETHOD_ADD_ASSIGN:	return  "+=";		// _set_add
			case	METAMETHOD_SUB_ASSIGN:	return  "-=";		// _set_sub
			case 	METAMETHOD_MUL_ASSIGN:   return  "*=";		// _mulassign
			case	METAMETHOD_DIV_ASSIGN:	return  "/=";		// _set_div
			case	METAMETHOD_MOD_ASSIGN:	return  "%=";		// _set_mod
			case	METAMETHOD_AND_ASSIGN:	return  "&=";		// _set_and
			case	METAMETHOD_OR_ASSIGN:	return  "|=";		// _set_or
			case	METAMETHOD_XOR_ASSIGN:	return  "^=";		// _set_xor
			case	METAMETHOD_SHL_ASSIGN:	return  "<<=";		// _set_shl
			case	METAMETHOD_SHR_ASSIGN:	return  ">>=";		// _set_shr
			case	METAMETHOD_BTWST:	return  "~=";		// _set_btw
			//case	METAMETHOD_GET:		return  "_get";		// _get
			case	METAMETHOD_TO_STRING: return  "_tostring";// _tostring
			case 	METAMETHOD_POST_INC: 	return  "++";		// i++
			case 	METAMETHOD_POST_DEC: 	return  "--";		//  i--
			case 	METAMETHOD_PRE_INC: 	return  "++";		// ++i
			case 	METAMETHOD_PRE_DEC: 	return  "--";		// --i
			case 	METAMETHOD_IN: 		return  "in";		// in
			default:
				return "none";
		}
	}

	const char * MetamethodHelper::getMetamethodSymbolName(Metamethod _metamethod){

		switch (_metamethod) {
		default:
			break;
			case	METAMETHOD_EQU:				return  "_equ"; 	// ==
			case	METAMETHOD_NEQU: 			return  "_nequ";  	// !=,
			case	METAMETHOD_LT:				return  "_lt";  	// <
			case	METAMETHOD_LTE:				return  "_lte";  	// <=
			case	METAMETHOD_NOT:				return  "_not"; 	// !
			case	METAMETHOD_GT:				return  "_gt";  	// >
			case	METAMETHOD_GTE:				return  "_gte"; 	// >=
			case	METAMETHOD_NEG:				return  "_neg"; 	// -a, !a
			case	METAMETHOD_BWC:				return  "_bwc"; 	// ~a bitwise complement operator
			case	METAMETHOD_ADD:				return  "_add"; 	// +
			case	METAMETHOD_SUB:				return  "_sub"; 	// -
			case	METAMETHOD_DIV:				return  "_div"; 	// /
			case	METAMETHOD_MUL:				return  "_mul"; 	// *
			case	METAMETHOD_MOD:				return  "_mod";  	// %
			case	METAMETHOD_AND:				return  "_and"; 	// bitwise and
			case	METAMETHOD_OR:				return  "_or"; 		// bitwise or
			case	METAMETHOD_XOR:				return  "_xor"; 	// bitwise xor
			case	METAMETHOD_SHL:				return  "_shl"; 	// bitwise shift left
			case	METAMETHOD_SHR:				return  "_shr"; 	// bitwise shift right
			case	METAMETHOD_SET:				return  "_set"; 	// _set
			case	METAMETHOD_ADD_ASSIGN:		return  "_addassign";	// _set_add
			case	METAMETHOD_SUB_ASSIGN:		return  "_subassign";	// _set_sub
			case	METAMETHOD_MUL_ASSIGN:		return  "_mulassign";	// _mulassign
			case	METAMETHOD_DIV_ASSIGN:		return  "_divassign";	// _set_div
			case	METAMETHOD_MOD_ASSIGN:		return  "_modassign";	// _set_mod
			case	METAMETHOD_AND_ASSIGN:		return  "_andassign";	// _set_and
			case	METAMETHOD_OR_ASSIGN:		return  "_orassign"; 	// _set_or
			case	METAMETHOD_XOR_ASSIGN:		return  "_xorassign";	// _set_xor
			case	METAMETHOD_SHL_ASSIGN:		return  "_shlassign";	// _set_shl
			case	METAMETHOD_SHR_ASSIGN:		return  "_shrassign";	// _set_shr
			case	METAMETHOD_BTWST:			return  "_btwassign";	// _set_btw

			//case	METAMETHOD_GET:		return  "_get"; 	// _get
			case	METAMETHOD_TO_STRING:		return  "_tostring";// _tostring
			case 	METAMETHOD_POST_INC: 		return  "_postinc";// i++
			case 	METAMETHOD_POST_DEC:		return  "_postdec";// i--
			case 	METAMETHOD_PRE_INC: 		return  "_preinc";	// ++i
			case 	METAMETHOD_PRE_DEC:			return  "_predec";	// --i
			case 	METAMETHOD_IN: 				return  "_in";		// 	_in
		}

		return "none";
	}

	int	MetamethodHelper::getMetamethodNumberArguments(Metamethod _metamethod){
		switch(_metamethod){
		default:
			break;
			case METAMETHOD_TO_STRING:
			case METAMETHOD_POST_INC:
			case METAMETHOD_POST_DEC:
			case METAMETHOD_PRE_INC:
			case METAMETHOD_PRE_DEC:
			case METAMETHOD_NOT:
			case METAMETHOD_NEG:
			case METAMETHOD_BWC:
				return 0;
			case METAMETHOD_ADD_ASSIGN:
			case METAMETHOD_SUB_ASSIGN:
			case METAMETHOD_MUL_ASSIGN:
			case METAMETHOD_DIV_ASSIGN:
			case METAMETHOD_MOD_ASSIGN:
			case METAMETHOD_AND_ASSIGN:
			case METAMETHOD_OR_ASSIGN:
			case METAMETHOD_XOR_ASSIGN:
			case METAMETHOD_SHL_ASSIGN:
			case METAMETHOD_SHR_ASSIGN:
			case METAMETHOD_SET:
			case METAMETHOD_IN:
				return 1;
		}

		return 2;
	}

	bool		 MetamethodHelper::isMetamethodStatic(Metamethod _metamethod){

		switch(_metamethod){
		case METAMETHOD_EQU:
		case METAMETHOD_NEQU:
		case METAMETHOD_LT:
		case METAMETHOD_LTE:
		case METAMETHOD_GT:
		case METAMETHOD_GTE:
		case METAMETHOD_ADD:
		case METAMETHOD_SUB:
		case METAMETHOD_DIV:
		case METAMETHOD_MUL:
		case METAMETHOD_MOD:
		case METAMETHOD_AND:
		case METAMETHOD_OR:
		case METAMETHOD_XOR:
		case METAMETHOD_SHL:
		case METAMETHOD_SHR:
			return true;
			break;
		default:
			break;
		}
		return false;
	}
}
