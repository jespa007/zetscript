/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const MetamethodByteCode MetamethodMembers::metamethod_byte_code_member_setter_list[]={
			METAMETHOD_BYTE_CODE_SET
			,METAMETHOD_BYTE_CODE_ADD_SET
			,METAMETHOD_BYTE_CODE_SUB_SET
			,METAMETHOD_BYTE_CODE_MUL_SET
			,METAMETHOD_BYTE_CODE_DIV_SET
			,METAMETHOD_BYTE_CODE_MOD_SET
			,METAMETHOD_BYTE_CODE_AND_SET
			,METAMETHOD_BYTE_CODE_OR_SET
			,METAMETHOD_BYTE_CODE_XOR_SET
			,METAMETHOD_BYTE_CODE_SHL_SET
			,METAMETHOD_BYTE_CODE_SHR_SET
			,METAMETHOD_BYTE_CODE_EQU //--> METAMETHOD_BYTE_CODE_EQU is special mark defined as 0 (end of elements)
	};

	const MetamethodByteCode MetamethodMembers::metamethod_byte_code_member_list[]={

			METAMETHOD_BYTE_CODE_SET
			,METAMETHOD_BYTE_CODE_ADD_SET
			,METAMETHOD_BYTE_CODE_SUB_SET
			,METAMETHOD_BYTE_CODE_MUL_SET
			,METAMETHOD_BYTE_CODE_DIV_SET
			,METAMETHOD_BYTE_CODE_MOD_SET
			,METAMETHOD_BYTE_CODE_AND_SET
			,METAMETHOD_BYTE_CODE_OR_SET
			,METAMETHOD_BYTE_CODE_XOR_SET
			,METAMETHOD_BYTE_CODE_SHL_SET
			,METAMETHOD_BYTE_CODE_SHR_SET
			,METAMETHOD_BYTE_CODE_POST_INC
			,METAMETHOD_BYTE_CODE_POST_DEC
			,METAMETHOD_BYTE_CODE_PRE_INC
			,METAMETHOD_BYTE_CODE_PRE_DEC
			,METAMETHOD_BYTE_CODE_EQU //--> METAMETHOD_BYTE_CODE_EQU is special mark defined as 0  (end of elements)
	};

	MetamethodMembers::MetamethodMembers(){
		getter= NULL;
		post_inc=NULL;
		post_dec=NULL;
		pre_inc=NULL;
		pre_dec=NULL;
		neg=NULL;
		bwc=NULL;
	}


	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(MetamethodByteCode _metamethod_byte_code){
		MetamethodMemberSetterInfo info;
		info.metamethod_byte_code= _metamethod_byte_code;
		info.metamethod_name=metamethod_byte_code_to_symbol_str(_metamethod_byte_code);
		switch(_metamethod_byte_code){
			case METAMETHOD_BYTE_CODE_SET:
				info.setters=&setters;
				break;
			case METAMETHOD_BYTE_CODE_ADD_SET:
				info.setters=&add_setters;
				break;
			case METAMETHOD_BYTE_CODE_SUB_SET:
				 info.setters=&sub_setters;
				break;
			case METAMETHOD_BYTE_CODE_MUL_SET:
				info.setters=&mul_setters;
				break;
			case METAMETHOD_BYTE_CODE_DIV_SET:
				info.setters=&div_setters;
				break;
			case METAMETHOD_BYTE_CODE_MOD_SET:
				info.setters=&mod_setters;
				break;
			case METAMETHOD_BYTE_CODE_AND_SET:
				info.setters=&and_setters;
				break;
			case METAMETHOD_BYTE_CODE_OR_SET:
				info.setters=&or_setters;
				break;
			case METAMETHOD_BYTE_CODE_XOR_SET:
				info.setters=&xor_setters;
				break;
			case METAMETHOD_BYTE_CODE_SHL_SET:
				info.setters=&shl_setters;
				break;
			case METAMETHOD_BYTE_CODE_SHR_SET:
				info.setters=&shr_setters;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberSetterInfo 		MetamethodMembers::getSetterInfo(const zs_string & _metamethod_name){
		const MetamethodByteCode *it=metamethod_byte_code_member_setter_list;
		MetamethodByteCode metamethod_byte_code=MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;

		while(*it!=0){
			const char *_mt_name=metamethod_byte_code_to_symbol_str(*it);
			if(_metamethod_name==_mt_name){
				metamethod_byte_code=*it;
				break;
			}

			it++;
		}

		return getSetterInfo(metamethod_byte_code);
	}


	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(MetamethodByteCode _metamethod_byte_code){
		MetamethodMemberGetterInfo info;
		info.metamethod_byte_code= _metamethod_byte_code;
		info.metamethod_name=metamethod_byte_code_to_symbol_str(_metamethod_byte_code);
		switch(_metamethod_byte_code){
			case METAMETHOD_BYTE_CODE_POST_INC:
				info.getter=&post_inc;
				break;
			case METAMETHOD_BYTE_CODE_POST_DEC:
				 info.getter=&post_dec;
				break;
			case METAMETHOD_BYTE_CODE_PRE_INC:
				info.getter=&pre_inc;
				break;
			case METAMETHOD_BYTE_CODE_PRE_DEC:
				info.getter=&pre_dec;
				break;
			case METAMETHOD_BYTE_CODE_NEG:
				info.getter=&neg;
				break;
			case METAMETHOD_BYTE_CODE_BWC:
				info.getter=&bwc;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(const zs_string & _metamethod_name){

		// search setter
		MetamethodByteCode metamethod_byte_code=METAMETHOD_BYTE_CODE_INVALID;

		// particular case
		if(_metamethod_name == "_get"){
			return MetamethodMemberGetterInfo(
				METAMETHOD_BYTE_CODE_GET
				,&getter
				,"_get"
			);
		}

		MetamethodByteCode getter_bytecodes[]={
			METAMETHOD_BYTE_CODE_POST_INC
			,METAMETHOD_BYTE_CODE_POST_DEC
			,METAMETHOD_BYTE_CODE_PRE_INC
			,METAMETHOD_BYTE_CODE_PRE_DEC
			,METAMETHOD_BYTE_CODE_NEG
			,METAMETHOD_BYTE_CODE_BWC
			,METAMETHOD_BYTE_CODE_INVALID
		};

		MetamethodByteCode *it=getter_bytecodes;

		while(*it != METAMETHOD_BYTE_CODE_INVALID){
			if(_metamethod_name==metamethod_byte_code_to_operator_str(*it)){
				metamethod_byte_code=*it;
				break;
			}
			it++;
		}


		return getGetterInfo(metamethod_byte_code);

	}

	void MetamethodMembers::addSetter(MetamethodByteCode _metamethod_byte_code, Symbol *symbol_function){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)symbol_function;
		stk->properties=STK_PROPERTY_FUNCTION;
		switch(_metamethod_byte_code){
			case METAMETHOD_BYTE_CODE_SET:
				setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_ADD_SET:
				add_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_SUB_SET:
				sub_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_MUL_SET:
				mul_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_DIV_SET:
				div_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_MOD_SET:
				mod_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_AND_SET:
				and_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_OR_SET:
				or_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_XOR_SET:
				xor_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_SHL_SET:
				shl_setters.push_back(stk);
				break;
			case METAMETHOD_BYTE_CODE_SHR_SET:
				shr_setters.push_back(stk);
				break;
			default:
				ZS_THROW_EXCEPTION("Unexpected to register '%s' setter"
					,metamethod_byte_code_to_symbol_str(_metamethod_byte_code)
				);
				break;
			}
	}

	bool MetamethodMembers::isSetter(MetamethodByteCode _metamethod_byte_code){
		auto setter_info = getSetterInfo(_metamethod_byte_code);

		return setter_info.metamethod_byte_code != MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;
	}

	bool MetamethodMembers::isSetter(const zs_string & _metamethod_byte_code_str){
		auto setter_info = getSetterInfo(_metamethod_byte_code_str);

		return setter_info.metamethod_byte_code != MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;

	}

	bool MetamethodMembers::isGetter(MetamethodByteCode _metamethod_byte_code){
		auto getter_info = getGetterInfo(_metamethod_byte_code);

		return getter_info.metamethod_byte_code != MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;
	}

	bool MetamethodMembers::isGetter(const zs_string & _metamethod_byte_code_str){
		auto getter_info = getGetterInfo(_metamethod_byte_code_str);

		return getter_info.metamethod_byte_code != MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;

	}

	/*bool MetamethodMembers::isMetamethodMember(MetamethodByteCode _byte_code){
		if(isSetter(_byte_code)){
			return true;
		}

		return isGetter(_byte_code);
		zs_string symbol="N/A";
		const MetamethodByteCode *it=metamethod_byte_code_member_list;

		while(*it!=0){
			if(_byte_code==*it){
				return true;
			}
			it++;
		}
		return false;
	}*/

	/*bool MetamethodMembers::isMetamethodMember(const zs_string & _metamethod_name){

		if(isSetter(_metamethod_name)){
			return true;
		}

		return isGetter(_metamethod_name);
		zs_string symbol="N/A";
		const MetamethodByteCode *it=metamethod_byte_code_member_list;

		while(*it!=0){
			if(_byte_code==*it){
				return true;
			}
			it++;
		}
		return false;
	}*/

	MetamethodMembers::~MetamethodMembers(){

		zs_vector<StackElement *> *ptr_vector[]={
			&setters
			,&add_setters
			,&sub_setters
			,&mul_setters
			,&div_setters
			,&mod_setters
			,&and_setters
			,&or_setters
			,&xor_setters
			,&shl_setters
			,&shr_setters
			,NULL
		};

		zs_vector<StackElement *> **it=ptr_vector;
		while(*it!=NULL){

			for(int i=0;i < (*it)->size(); i++){
				StackElement *stk_el=(StackElement *)(*it)->items[i];
				free(stk_el);
			}
			(*it)->clear();
			it++;
		}


		getter = NULL;
		post_inc = NULL;
		post_dec = NULL;
		pre_inc = NULL;
		pre_dec = NULL;
		neg = NULL;
		bwc=NULL;
	}
}
