/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const static ByteCodeMetamethod *MemberProperty::byte_code_metamethod_list={
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
			,0
	};

	MemberProperty::MemberProperty(ScriptClass *_script_class,const zs_string & _property_name){
		getter= NULL;
		post_inc=NULL;
		post_dec=NULL;
		pre_inc=NULL;
		pre_dec=NULL;
		property_name=_property_name;
		script_class=_script_class;
	}
	MemberPropertyInfo MemberProperty::getInfo(ByteCodeMetamethod _byte_code_metamethod){
		MemberPropertyInfo info={_byte_code_metamethod,NULL,NULL};
		info.str_byte_code_metamethod=byte_code_metamethod_to_symbol_str(_byte_code_metamethod);
		switch(_byte_code_metamethod){
			case BYTE_CODE_METAMETHOD_SET:
				return info.setters=&setters;
				break;
			case BYTE_CODE_METAMETHOD_ADD_SET:
				return info.setters=&add_setters;
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

	MemberPropertyInfo MemberProperty::getInfo(const char *_symbol_name){

		ByteCodeMetamethod _byte_code_metamethod=byte_code_symbol_to_setter_metamethod(_symbol_name);
		return getInfo(_byte_code_metamethod);


	}

	void MemberProperty::addSetter(ByteCodeMetamethod _byte_code_metamethod, ScriptFunction *f){

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
				THROW_SCRIPT_ERROR("Unexpected to register '%s' setter"
					,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
				);
				break;
			}
	}




	MemberProperty::~MemberProperty(){

		zs_vector *ptr_vector={
				&setters
				&add_setters
				&sub_setters
				&div_setters
				&mod_setters
				&and_setters
				&or_setters
				&xor_setters
				&shl_setters
				&shr_setters
				,0
		};

		zs_vector *it=ptr_vector;
		while(*it++!=NULL){

			for(int i=0;i < it->count; i++){
				StackElement *stk_el=(StackElement *)it->items[i];
				free(stk_el);
			}
			it->clear();
		}


		getter = NULL;
		post_inc = NULL;
		post_dec = NULL;
		pre_inc = NULL;
		pre_dec = NULL;


	}
};
