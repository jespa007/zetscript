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
			,BYTE_CODE_METAMETHOD_EQU //--> is 0 (end of elements)
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
			,BYTE_CODE_METAMETHOD_EQU //--> is 0 (end of elements)
	};



	MetamethodMembers::MetamethodMembers(){
		getter= NULL;
		post_inc=NULL;
		post_dec=NULL;
		pre_inc=NULL;
		pre_dec=NULL;
		neg=NULL;
	}
	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(ByteCodeMetamethod _byte_code_metamethod){
		MetamethodMemberSetterInfo info={_byte_code_metamethod,NULL,NULL};
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

	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(const char *_symbol_name){

		// search setter
		const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
		bool found=false;
		ByteCodeMetamethod _byte_code_metamethod=BYTE_CODE_METAMETHOD_INVALID;
		while(*it!=0){
			const char *_mt_name=byte_code_metamethod_to_symbol_str(*it);
			if(_symbol_name == _mt_name){
				_byte_code_metamethod=*it;
				break;
			}

			it++;
		}

		return getSetterInfo(_byte_code_metamethod);
	}

	void MetamethodMembers::addSetter(ByteCodeMetamethod _byte_code_metamethod, ScriptFunction *f){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)f;
		stk->properties=STK_PROPERTY_FUNCTION;
		switch(_byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_SET:
				setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_ADD_SET:
				add_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_SUB_SET:
				sub_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_MUL_SET:
				mul_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_DIV_SET:
				div_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_MOD_SET:
				mod_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_AND_SET:
				and_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_OR_SET:
				or_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_XOR_SET:
				xor_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_SHL_SET:
				shl_setters.push_back((zs_int)stk);
				break;
			case BYTE_CODE_METAMETHOD_SHR_SET:
				shr_setters.push_back((zs_int)stk);
				break;
			default:
				THROW_EXCEPTION(zs_strutils::format("Unexpected to register '%s' setter"
						,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
					)
				);
				break;
			}
	}


	/*const zs_string & MetamethodMembers::byte_code_metamethod_to_symbol_str(ByteCodeMetamethod _byte_code, const zs_string & _property_name){
		zs_string symbol="N/A";
		const ByteCodeMetamethod *it=byte_code_metamethod_member_list;

		while(*it!=0){
			if(_byte_code==*it){
				symbol=zs_string(ByteCode::byte_code_metamethod_to_symbol_str(_byte_code))+"@"+_property_name;//ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(_byte_code,_property_name);
				break;
			}
			it++;
		}
		return symbol;
	}*/
	bool MetamethodMembers::isSetter(ByteCodeMetamethod _byte_code_metamethod){
		const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
		bool found=false;
		while(*it!=0){
			const char *_mt_name=byte_code_metamethod_to_symbol_str(*it);
			if(*it == _byte_code_metamethod){
				return true;
			}
			it++;
		}

		return false;
	}

	/*ByteCodeMetamethod MetamethodMembers::toByteCodeMetamethod(const char *_symbol_name)
	{

			const ByteCodeMetamethod *it=byte_code_metamethod_member_setter_list;
			bool found=false;
			while(*it!=0){
				const char *_mt_name=byte_code_metamethod_to_symbol_str(*it);
				if(_symbol_name == _mt_name){
					return *it;
				}

				it++;
			}

			return BYTE_CODE_METAMETHOD_INVALID;
	}*/


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

		zs_vector *ptr_vector[]={
				&setters
				,&add_setters
				,&sub_setters
				,&div_setters
				,&mod_setters
				,&and_setters
				,&or_setters
				,&xor_setters
				,&shl_setters
				,&shr_setters
				,NULL
		};

		zs_vector **it=ptr_vector;
		while(*it!=NULL){

			for(int i=0;i < (*it)->count; i++){
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
	}
}
