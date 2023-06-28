/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const ByteCodeMetamethod MetamethodMembers::byte_code_metamethod_member_setter_list[]={
			BYTE_CODE_METAMETHOD_SET
			,BYTE_CODE_METAMETHOD_ADD_SET
			,BYTE_CODE_METAMETHOD_SUB_SET
			,BYTE_CODE_METAMETHOD_MUL_SET
			,BYTE_CODE_METAMETHOD_DIV_SET
			,BYTE_CODE_METAMETHOD_MOD_SET
			,BYTE_CODE_METAMETHOD_AND_SET
			,BYTE_CODE_METAMETHOD_OR_SET
			,BYTE_CODE_METAMETHOD_XOR_SET
			,BYTE_CODE_METAMETHOD_SHL_SET
			,BYTE_CODE_METAMETHOD_SHR_SET
			,BYTE_CODE_METAMETHOD_EQU //--> BYTE_CODE_METAMETHOD_EQU is special mark defined as 0 (end of elements)
	};

	const ByteCodeMetamethod MetamethodMembers::byte_code_metamethod_member_list[]={

			BYTE_CODE_METAMETHOD_SET
			,BYTE_CODE_METAMETHOD_ADD_SET
			,BYTE_CODE_METAMETHOD_SUB_SET
			,BYTE_CODE_METAMETHOD_MUL_SET
			,BYTE_CODE_METAMETHOD_DIV_SET
			,BYTE_CODE_METAMETHOD_MOD_SET
			,BYTE_CODE_METAMETHOD_AND_SET
			,BYTE_CODE_METAMETHOD_OR_SET
			,BYTE_CODE_METAMETHOD_XOR_SET
			,BYTE_CODE_METAMETHOD_SHL_SET
			,BYTE_CODE_METAMETHOD_SHR_SET
			,BYTE_CODE_METAMETHOD_POST_INC
			,BYTE_CODE_METAMETHOD_POST_DEC
			,BYTE_CODE_METAMETHOD_PRE_INC
			,BYTE_CODE_METAMETHOD_PRE_DEC
			,BYTE_CODE_METAMETHOD_EQU //--> BYTE_CODE_METAMETHOD_EQU is special mark defined as 0  (end of elements)
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
	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(ByteCodeMetamethod _byte_code_metamethod){
		MetamethodMemberSetterInfo info;
		info.byte_code_metamethod= _byte_code_metamethod;
		info.str_byte_code_metamethod=byte_code_metamethod_to_symbol_str(_byte_code_metamethod);
		switch(_byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_SET:
				info.setters=&setters;
				break;
			case BYTE_CODE_METAMETHOD_ADD_SET:
				info.setters=&add_setters;
				break;
			case BYTE_CODE_METAMETHOD_SUB_SET:
				 info.setters=&sub_setters;
				break;
			case BYTE_CODE_METAMETHOD_MUL_SET:
				info.setters=&mul_setters;
				break;
			case BYTE_CODE_METAMETHOD_DIV_SET:
				info.setters=&div_setters;
				break;
			case BYTE_CODE_METAMETHOD_MOD_SET:
				info.setters=&mod_setters;
				break;
			case BYTE_CODE_METAMETHOD_AND_SET:
				info.setters=&and_setters;
				break;
			case BYTE_CODE_METAMETHOD_OR_SET:
				info.setters=&or_setters;
				break;
			case BYTE_CODE_METAMETHOD_XOR_SET:
				info.setters=&xor_setters;
				break;
			case BYTE_CODE_METAMETHOD_SHL_SET:
				info.setters=&shl_setters;
				break;
			case BYTE_CODE_METAMETHOD_SHR_SET:
				info.setters=&shr_setters;
				break;
			default:
				break;
			}
		return info;
	}


	Symbol *getter,*post_inc,*post_dec,*pre_inc,*pre_dec,*neg,*bwc; // getter

	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(ByteCodeMetamethod _byte_code_metamethod){
		MetamethodMemberGetterInfo info;
		info.byte_code_metamethod= _byte_code_metamethod;
		info.str_byte_code_metamethod=byte_code_metamethod_to_symbol_str(_byte_code_metamethod);
		switch(_byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_POST_INC:
				info.getter=post_inc;
				break;
			case BYTE_CODE_METAMETHOD_POST_DEC:
				 info.getter=post_dec;
				break;
			case BYTE_CODE_METAMETHOD_PRE_INC:
				info.getter=pre_inc;
				break;
			case BYTE_CODE_METAMETHOD_PRE_DEC:
				info.getter=pre_dec;
				break;
			case BYTE_CODE_METAMETHOD_NEG:
				info.getter=neg;
				break;
			case BYTE_CODE_METAMETHOD_BWC:
				info.getter=bwc;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberGetterInfo MetamethodMembers::getSetterInfo(const char *_symbol_name){

		// search setter
		const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
		ByteCodeMetamethod _byte_code_metamethod=BYTE_CODE_METAMETHOD_INVALID;

		// particular case
		if(strcmp(_symbol_name,"_get")==0){
			return MetamethodMemberGetterInfo(
				BYTE_CODE_METAMETHOD_GET
				,get
				,"_get"
			);
		}else{

			while(*it!=0){
				const char *_mt_name=byte_code_metamethod_to_symbol_str(*it);
				if(strcmp(_symbol_name,_mt_name)==0){
					_byte_code_metamethod=*it;
					break;
				}

				it++;
			}

			return getSetterInfo(_byte_code_metamethod);
		}

	}

	void MetamethodMembers::addSetter(ByteCodeMetamethod _byte_code_metamethod, Symbol *symbol_function){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)symbol_function;
		stk->properties=STK_PROPERTY_FUNCTION;
		switch(_byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_SET:
				setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_ADD_SET:
				add_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_SUB_SET:
				sub_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_MUL_SET:
				mul_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_DIV_SET:
				div_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_MOD_SET:
				mod_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_AND_SET:
				and_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_OR_SET:
				or_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_XOR_SET:
				xor_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_SHL_SET:
				shl_setters.push_back(stk);
				break;
			case BYTE_CODE_METAMETHOD_SHR_SET:
				shr_setters.push_back(stk);
				break;
			default:
				ZS_THROW_EXCEPTION("Unexpected to register '%s' setter"
					,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
				);
				break;
			}
	}

	bool MetamethodMembers::isSetter(ByteCodeMetamethod _byte_code_metamethod){
		const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
		while(*it!=0){
			if(*it == _byte_code_metamethod){
				return true;
			}
			it++;
		}

		return false;
	}

	static bool MetamethodMembers::isSetter(const char * _byte_code_metamethod_str){
		// search setter
		const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
		ByteCodeMetamethod _byte_code_metamethod=BYTE_CODE_METAMETHOD_INVALID;
		while(*it!=0){
			const char *_mt_name=byte_code_metamethod_to_symbol_str(*it);
			if(strcmp(_byte_code_metamethod_str,_mt_name)==0){
				return true;
				break;
			}

			it++;
		}

		return false;
	}

	bool MetamethodMembers::isMetamethodMember(ByteCodeMetamethod _byte_code){
		zs_string symbol="N/A";
		const ByteCodeMetamethod *it=byte_code_metamethod_member_list;

		while(*it!=0){
			if(_byte_code==*it){
				return true;
			}
			it++;
		}
		return false;
	}

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
