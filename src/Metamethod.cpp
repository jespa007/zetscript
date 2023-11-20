/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const char * Metamethod::toOperatorString(MetamethodId op){
		switch (op) {
			case	Metamethod::MetamethodId::METAMETHOD_ID_EQU:		return  "==";  		// ==
			case	Metamethod::MetamethodId::METAMETHOD_ID_NEQU: 	return  "!=";  		// !=,
			case	Metamethod::MetamethodId::METAMETHOD_ID_LT:		return  "<";  		// <
			case	Metamethod::MetamethodId::METAMETHOD_ID_LTE:		return  "<=";  		// <=
			case	Metamethod::MetamethodId::METAMETHOD_ID_NOT:		return  "!"; 		// !
			case	Metamethod::MetamethodId::METAMETHOD_ID_GT:		return  ">";  		// >
			case	Metamethod::MetamethodId::METAMETHOD_ID_GTE:		return  ">="; 		// >=
			case	Metamethod::MetamethodId::METAMETHOD_ID_NEG:		return  "-"; 		// -a
			case	Metamethod::MetamethodId::METAMETHOD_ID_BWC:		return  "~"; 		// ~a
			case	Metamethod::MetamethodId::METAMETHOD_ID_ADD:		return  "+"; 		// +
			case	Metamethod::MetamethodId::METAMETHOD_ID_SUB:		return  "-"; 		// -
			case	Metamethod::MetamethodId::METAMETHOD_ID_DIV:		return  "/"; 		// /
			case	Metamethod::MetamethodId::METAMETHOD_ID_MUL:		return  "*"; 		// *
			case	Metamethod::MetamethodId::METAMETHOD_ID_MOD:		return  "%";  		// %
			case	Metamethod::MetamethodId::METAMETHOD_ID_AND:		return  "&"; 		// bitwise and
			case	Metamethod::MetamethodId::METAMETHOD_ID_OR:		return  "|";		// bitwise or
			case	Metamethod::MetamethodId::METAMETHOD_ID_XOR:		return  "^"; 		// bitwise xor
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHL:		return  "<<"; 		// bitwise shift left
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHR:		return  ">>"; 		// bitwise shift right
			case	Metamethod::MetamethodId::METAMETHOD_ID_SET:		return  "=";		// _set
			case	Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN:	return  "+=";		// _set_add
			case	Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN:	return  "-=";		// _set_sub
			case 	Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN:   return  "*=";		// _mulassign
			case	Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN:	return  "/=";		// _set_div
			case	Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN:	return  "%=";		// _set_mod
			case	Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN:	return  "&=";		// _set_and
			case	Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN:	return  "|=";		// _set_or
			case	Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN:	return  "^=";		// _set_xor
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN:	return  "<<=";		// _set_shl
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN:	return  ">>=";		// _set_shr
			case	Metamethod::MetamethodId::METAMETHOD_ID_BTWST:	return  "~=";		// _set_btw
			//case	Metamethod::MetamethodId::METAMETHOD_ID_GET:		return  "_get";		// _get
			case	Metamethod::MetamethodId::METAMETHOD_ID_TO_STRING: return  "_tostring";// _tostring
			case 	Metamethod::MetamethodId::METAMETHOD_ID_POST_INC: 	return  "++";		// i++
			case 	Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC: 	return  "--";		//  i--
			case 	Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC: 	return  "++";		// ++i
			case 	Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC: 	return  "--";		// --i
			case 	Metamethod::MetamethodId::METAMETHOD_ID_IN: 		return  "in";		// in
			default:
				return "none";
		}
	}

	const char * Metamethod::toSymbolString(MetamethodId op){

		switch (op) {
		default:
			break;
			case	Metamethod::MetamethodId::METAMETHOD_ID_EQU:		return  "_equ"; 	// ==
			case	Metamethod::MetamethodId::METAMETHOD_ID_NEQU: 		return  "_nequ";  	// !=,
			case	Metamethod::MetamethodId::METAMETHOD_ID_LT:			return  "_lt";  	// <
			case	Metamethod::MetamethodId::METAMETHOD_ID_LTE:		return  "_lte";  	// <=
			case	Metamethod::MetamethodId::METAMETHOD_ID_NOT:		return  "_not"; 	// !
			case	Metamethod::MetamethodId::METAMETHOD_ID_GT:			return  "_gt";  	// >
			case	Metamethod::MetamethodId::METAMETHOD_ID_GTE:		return  "_gte"; 	// >=
			case	Metamethod::MetamethodId::METAMETHOD_ID_NEG:		return  "_neg"; 	// -a, !a
			case	Metamethod::MetamethodId::METAMETHOD_ID_BWC:		return  "_bwc"; 	// ~a bitwise complement operator
			case	Metamethod::MetamethodId::METAMETHOD_ID_ADD:		return  "_add"; 	// +
			case	Metamethod::MetamethodId::METAMETHOD_ID_SUB:		return  "_sub"; 	// -
			case	Metamethod::MetamethodId::METAMETHOD_ID_DIV:		return  "_div"; 	// /
			case	Metamethod::MetamethodId::METAMETHOD_ID_MUL:		return  "_mul"; 	// *
			case	Metamethod::MetamethodId::METAMETHOD_ID_MOD:		return  "_mod";  	// %
			case	Metamethod::MetamethodId::METAMETHOD_ID_AND:		return  "_and"; 	// bitwise and
			case	Metamethod::MetamethodId::METAMETHOD_ID_OR:			return  "_or"; 		// bitwise or
			case	Metamethod::MetamethodId::METAMETHOD_ID_XOR:		return  "_xor"; 	// bitwise xor
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHL:		return  "_shl"; 	// bitwise shift left
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHR:		return  "_shr"; 	// bitwise shift right
			case	Metamethod::MetamethodId::METAMETHOD_ID_SET:		return  "_set"; 	// _set
			case	Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN:		return  "_addassign";	// _set_add
			case	Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN:		return  "_subassign";	// _set_sub
			case	Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN:		return  "_mulassign";	// _mulassign
			case	Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN:		return  "_divassign";	// _set_div
			case	Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN:		return  "_modassign";	// _set_mod
			case	Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN:		return  "_andassign";	// _set_and
			case	Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN:		return  "_orassign"; 	// _set_or
			case	Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN:		return  "_xorassign";	// _set_xor
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN:		return  "_shlassign";	// _set_shl
			case	Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN:		return  "_shrassign";	// _set_shr
			case	Metamethod::MetamethodId::METAMETHOD_ID_BTWST:		return  "_btwassign";	// _set_btw

			//case	Metamethod::MetamethodId::METAMETHOD_ID_GET:		return  "_get"; 	// _get
			case	Metamethod::MetamethodId::METAMETHOD_ID_TO_STRING:	return  "_tostring";// _tostring
			case 	Metamethod::MetamethodId::METAMETHOD_ID_POST_INC: 	return  "_postinc";// i++
			case 	Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC:	return  "_postdec";// i--
			case 	Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC: 	return  "_preinc";	// ++i
			case 	Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC:	return  "_predec";	// --i
			case 	Metamethod::MetamethodId::METAMETHOD_ID_IN: 		return  "_in";		// 	_in
		}

		return "none";
	}

	int			 Metamethod::getNumberArguments(MetamethodId op){
		switch(op){
		default:
			break;
			case Metamethod::MetamethodId::METAMETHOD_ID_TO_STRING:
			case Metamethod::MetamethodId::METAMETHOD_ID_POST_INC:
			case Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC:
			case Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC:
			case Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC:
			case Metamethod::MetamethodId::METAMETHOD_ID_NOT:
			case Metamethod::MetamethodId::METAMETHOD_ID_NEG:
			case Metamethod::MetamethodId::METAMETHOD_ID_BWC:
				return 0;
			case Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN:
			case Metamethod::MetamethodId::METAMETHOD_ID_SET:
			case Metamethod::MetamethodId::METAMETHOD_ID_IN:
				return 1;
		}

		return 2;
	}

	bool		 Metamethod::shouldBeStatic(Metamethod::MetamethodId _metamethod_id){

		switch(_metamethod_id){
		case Metamethod::MetamethodId::METAMETHOD_ID_EQU:
		case Metamethod::MetamethodId::METAMETHOD_ID_NEQU:
		case Metamethod::MetamethodId::METAMETHOD_ID_LT:
		case Metamethod::MetamethodId::METAMETHOD_ID_LTE:
		case Metamethod::MetamethodId::METAMETHOD_ID_GT:
		case Metamethod::MetamethodId::METAMETHOD_ID_GTE:
		case Metamethod::MetamethodId::METAMETHOD_ID_ADD:
		case Metamethod::MetamethodId::METAMETHOD_ID_SUB:
		case Metamethod::MetamethodId::METAMETHOD_ID_DIV:
		case Metamethod::MetamethodId::METAMETHOD_ID_MUL:
		case Metamethod::MetamethodId::METAMETHOD_ID_MOD:
		case Metamethod::MetamethodId::METAMETHOD_ID_AND:
		case Metamethod::MetamethodId::METAMETHOD_ID_OR:
		case Metamethod::MetamethodId::METAMETHOD_ID_XOR:
		case Metamethod::MetamethodId::METAMETHOD_ID_SHL:
		case Metamethod::MetamethodId::METAMETHOD_ID_SHR:
			return true;
			break;
		default:
			break;
		}
		return false;
	}
}
