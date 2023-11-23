/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const  Metamethod  MetamethodMembers::member_setter_list[]={
			METAMETHOD_SET
			,METAMETHOD_ADD_ASSIGN
			,METAMETHOD_SUB_ASSIGN
			,METAMETHOD_MUL_ASSIGN
			,METAMETHOD_DIV_ASSIGN
			,METAMETHOD_MOD_ASSIGN
			,METAMETHOD_AND_ASSIGN
			,METAMETHOD_OR_ASSIGN
			,METAMETHOD_XOR_ASSIGN
			,METAMETHOD_SHL_ASSIGN
			,METAMETHOD_SHR_ASSIGN
			,METAMETHOD_EQU //--> METAMETHOD_EQU is special mark defined as 0 (end of elements)
	};

	const Metamethod MetamethodMembers::member_list[]={

			METAMETHOD_SET
			,METAMETHOD_ADD_ASSIGN
			,METAMETHOD_SUB_ASSIGN
			,METAMETHOD_MUL_ASSIGN
			,METAMETHOD_DIV_ASSIGN
			,METAMETHOD_MOD_ASSIGN
			,METAMETHOD_AND_ASSIGN
			,METAMETHOD_OR_ASSIGN
			,METAMETHOD_XOR_ASSIGN
			,METAMETHOD_SHL_ASSIGN
			,METAMETHOD_SHR_ASSIGN
			,METAMETHOD_POST_INC
			,METAMETHOD_POST_DEC
			,METAMETHOD_PRE_INC
			,METAMETHOD_PRE_DEC
			,METAMETHOD_EQU //--> METAMETHOD_EQU is special mark defined as 0  (end of elements)
	};

	MetamethodMembers::MetamethodMembers(){
		getter= NULL;
		postinc=NULL;
		postdec=NULL;
		preinc=NULL;
		predec=NULL;
		neg=NULL;
		bwc=NULL;
	}


	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(Metamethod _metamethod){
		MetamethodMemberSetterInfo info;
		info.metamethod= _metamethod;
		info.metamethod_name=MetamethodHelper::getMetamethodSymbolName(_metamethod);
		switch(_metamethod){
			case METAMETHOD_SET:
				info.setters=&setters;
				break;
			case METAMETHOD_ADD_ASSIGN:
				info.setters=&add_setters;
				break;
			case METAMETHOD_SUB_ASSIGN:
				 info.setters=&sub_setters;
				break;
			case METAMETHOD_MUL_ASSIGN:
				info.setters=&mul_setters;
				break;
			case METAMETHOD_DIV_ASSIGN:
				info.setters=&div_setters;
				break;
			case METAMETHOD_MOD_ASSIGN:
				info.setters=&mod_setters;
				break;
			case METAMETHOD_AND_ASSIGN:
				info.setters=&and_setters;
				break;
			case METAMETHOD_OR_ASSIGN:
				info.setters=&or_setters;
				break;
			case METAMETHOD_XOR_ASSIGN:
				info.setters=&xor_setters;
				break;
			case METAMETHOD_SHL_ASSIGN:
				info.setters=&shl_setters;
				break;
			case METAMETHOD_SHR_ASSIGN:
				info.setters=&shr_setters;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberSetterInfo 		MetamethodMembers::getSetterInfo(const zs_string & _metamethod_name){
		const Metamethod *it=member_setter_list;
		Metamethod metamethod=METAMETHOD_INVALID;

		while(*it!=0){
			const char *_mt_name=MetamethodHelper::getMetamethodSymbolName(*it);
			if(_metamethod_name==_mt_name){
				metamethod=*it;
				break;
			}

			it++;
		}
		return getSetterInfo(metamethod);
	}


	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(Metamethod _metamethod){
		MetamethodMemberGetterInfo info;
		info.metamethod= _metamethod;
		info.metamethod_name=MetamethodHelper::getMetamethodSymbolName(_metamethod);
		switch(_metamethod){
			case METAMETHOD_POST_INC:
				info.getter=&postinc;
				break;
			case METAMETHOD_POST_DEC:
				 info.getter=&postdec;
				break;
			case METAMETHOD_PRE_INC:
				info.getter=&preinc;
				break;
			case METAMETHOD_PRE_DEC:
				info.getter=&predec;
				break;
			case METAMETHOD_NEG:
				info.getter=&neg;
				break;
			case METAMETHOD_BWC:
				info.getter=&bwc;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(const zs_string & _metamethod_name){

		// search setter
		Metamethod metamethod=METAMETHOD_INVALID;

		// particular case
		if(_metamethod_name == "_get"){
			return MetamethodMemberGetterInfo(
				METAMETHOD_GET
				,&getter
				,"_get"
			);
		}

		Metamethod getter_bytecodes[]={
			METAMETHOD_POST_INC
			,METAMETHOD_POST_DEC
			,METAMETHOD_PRE_INC
			,METAMETHOD_PRE_DEC
			,METAMETHOD_NEG
			,METAMETHOD_BWC
			,METAMETHOD_INVALID
		};

		Metamethod *it=getter_bytecodes;

		while(*it != METAMETHOD_INVALID){
			if(_metamethod_name==MetamethodHelper::getMetamethodSymbolName(*it)){
				metamethod=*it;
				break;
			}
			it++;
		}


		return getGetterInfo(metamethod);

	}

	void MetamethodMembers::addSetter(Metamethod _metamethod, Symbol *symbol_function){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)symbol_function;
		stk->properties=STACK_ELEMENT_PROPERTY_FUNCTION;
		switch(_metamethod){
			case METAMETHOD_SET:
				setters.push_back(stk);
				break;
			case METAMETHOD_ADD_ASSIGN:
				add_setters.push_back(stk);
				break;
			case METAMETHOD_SUB_ASSIGN:
				sub_setters.push_back(stk);
				break;
			case METAMETHOD_MUL_ASSIGN:
				mul_setters.push_back(stk);
				break;
			case METAMETHOD_DIV_ASSIGN:
				div_setters.push_back(stk);
				break;
			case METAMETHOD_MOD_ASSIGN:
				mod_setters.push_back(stk);
				break;
			case METAMETHOD_AND_ASSIGN:
				and_setters.push_back(stk);
				break;
			case METAMETHOD_OR_ASSIGN:
				or_setters.push_back(stk);
				break;
			case METAMETHOD_XOR_ASSIGN:
				xor_setters.push_back(stk);
				break;
			case METAMETHOD_SHL_ASSIGN:
				shl_setters.push_back(stk);
				break;
			case METAMETHOD_SHR_ASSIGN:
				shr_setters.push_back(stk);
				break;
			default:
				ZS_THROW_EXCEPTION("Unexpected to register '%s' setter"
					,MetamethodHelper::getMetamethodSymbolName(_metamethod)
				);
				break;
			}
	}

	bool MetamethodMembers::isSetter(Metamethod _metamethod){
		auto setter_info = getSetterInfo(_metamethod);

		return setter_info.metamethod != METAMETHOD_INVALID;
	}

	bool MetamethodMembers::isSetter(const zs_string & _str){
		auto setter_info = getSetterInfo(_str);

		return setter_info.metamethod != METAMETHOD_INVALID;

	}

	bool MetamethodMembers::isGetter(Metamethod _metamethod){
		auto getter_info = getGetterInfo(_metamethod);

		return getter_info.metamethod != METAMETHOD_INVALID;
	}

	bool MetamethodMembers::isGetter(const zs_string & _str){
		auto getter_info = getGetterInfo(_str);

		return getter_info.metamethod != METAMETHOD_INVALID;

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
				StackElement *stk_el=(StackElement *)(*it)->get(i);
				free(stk_el);
			}
			(*it)->clear();
			it++;
		}


		getter = NULL;
		postinc = NULL;
		postdec = NULL;
		preinc = NULL;
		predec = NULL;
		neg = NULL;
		bwc=NULL;
	}
}
