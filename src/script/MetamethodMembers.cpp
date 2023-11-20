/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	const  Metamethod::MetamethodId  MetamethodMembers::member_setter_list[]={
			Metamethod::MetamethodId::METAMETHOD_ID_SET
			,Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_EQU //--> Metamethod::MetamethodId::METAMETHOD_ID_EQU is special mark defined as 0 (end of elements)
	};

	const Metamethod::MetamethodId MetamethodMembers::member_list[]={

			Metamethod::MetamethodId::METAMETHOD_ID_SET
			,Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN
			,Metamethod::MetamethodId::METAMETHOD_ID_POST_INC
			,Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC
			,Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC
			,Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC
			,Metamethod::MetamethodId::METAMETHOD_ID_EQU //--> Metamethod::MetamethodId::METAMETHOD_ID_EQU is special mark defined as 0  (end of elements)
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


	MetamethodMemberSetterInfo MetamethodMembers::getSetterInfo(Metamethod::MetamethodId _metamethod_id){
		MetamethodMemberSetterInfo info;
		info.metamethod_id= _metamethod_id;
		info.metamethod_name=Metamethod::toSymbolString(_metamethod_id);
		switch(_metamethod_id){
			case Metamethod::MetamethodId::METAMETHOD_ID_SET:
				info.setters=&setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN:
				info.setters=&add_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN:
				 info.setters=&sub_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN:
				info.setters=&mul_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN:
				info.setters=&div_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN:
				info.setters=&mod_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN:
				info.setters=&and_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN:
				info.setters=&or_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN:
				info.setters=&xor_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN:
				info.setters=&shl_setters;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN:
				info.setters=&shr_setters;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberSetterInfo 		MetamethodMembers::getSetterInfo(const zs_string & _metamethod_name){
		const Metamethod::MetamethodId *it=member_setter_list;
		Metamethod::MetamethodId metamethod_id=Metamethod::MetamethodId::METAMETHOD_ID_INVALID;

		while(*it!=0){
			const char *_mt_name=Metamethod::toSymbolString(*it);
			if(_metamethod_name==_mt_name){
				metamethod_id=*it;
				break;
			}

			it++;
		}
		return getSetterInfo(metamethod_id);
	}


	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(Metamethod::MetamethodId _metamethod_id){
		MetamethodMemberGetterInfo info;
		info.metamethod_id= _metamethod_id;
		info.metamethod_name=Metamethod::toSymbolString(_metamethod_id);
		switch(_metamethod_id){
			case Metamethod::MetamethodId::METAMETHOD_ID_POST_INC:
				info.getter=&postinc;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC:
				 info.getter=&postdec;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC:
				info.getter=&preinc;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC:
				info.getter=&predec;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_NEG:
				info.getter=&neg;
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_BWC:
				info.getter=&bwc;
				break;
			default:
				break;
			}
		return info;
	}

	MetamethodMemberGetterInfo MetamethodMembers::getGetterInfo(const zs_string & _metamethod_name){

		// search setter
		Metamethod::MetamethodId metamethod_id=Metamethod::MetamethodId::METAMETHOD_ID_INVALID;

		// particular case
		if(_metamethod_name == "_get"){
			return MetamethodMemberGetterInfo(
				Metamethod::MetamethodId::METAMETHOD_ID_GET
				,&getter
				,"_get"
			);
		}

		Metamethod::MetamethodId getter_bytecodes[]={
			Metamethod::MetamethodId::METAMETHOD_ID_POST_INC
			,Metamethod::MetamethodId::METAMETHOD_ID_POST_DEC
			,Metamethod::MetamethodId::METAMETHOD_ID_PRE_INC
			,Metamethod::MetamethodId::METAMETHOD_ID_PRE_DEC
			,Metamethod::MetamethodId::METAMETHOD_ID_NEG
			,Metamethod::MetamethodId::METAMETHOD_ID_BWC
			,Metamethod::MetamethodId::METAMETHOD_ID_INVALID
		};

		Metamethod::MetamethodId *it=getter_bytecodes;

		while(*it != Metamethod::MetamethodId::METAMETHOD_ID_INVALID){
			if(_metamethod_name==Metamethod::toSymbolString(*it)){
				metamethod_id=*it;
				break;
			}
			it++;
		}


		return getGetterInfo(metamethod_id);

	}

	void MetamethodMembers::addSetter(Metamethod::MetamethodId _metamethod_id, Symbol *symbol_function){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)symbol_function;
		stk->properties=ZS_STK_PROPERTY_FUNCTION;
		switch(_metamethod_id){
			case Metamethod::MetamethodId::METAMETHOD_ID_SET:
				setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_ADD_ASSIGN:
				add_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SUB_ASSIGN:
				sub_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_MUL_ASSIGN:
				mul_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_DIV_ASSIGN:
				div_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_MOD_ASSIGN:
				mod_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_AND_ASSIGN:
				and_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_OR_ASSIGN:
				or_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_XOR_ASSIGN:
				xor_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SHL_ASSIGN:
				shl_setters.push_back(stk);
				break;
			case Metamethod::MetamethodId::METAMETHOD_ID_SHR_ASSIGN:
				shr_setters.push_back(stk);
				break;
			default:
				ZS_THROW_EXCEPTION("Unexpected to register '%s' setter"
					,Metamethod::toSymbolString(_metamethod_id)
				);
				break;
			}
	}

	bool MetamethodMembers::isSetter(Metamethod::MetamethodId _metamethod_id){
		auto setter_info = getSetterInfo(_metamethod_id);

		return setter_info.metamethod_id != Metamethod::MetamethodId::METAMETHOD_ID_INVALID;
	}

	bool MetamethodMembers::isSetter(const zs_string & _str){
		auto setter_info = getSetterInfo(_str);

		return setter_info.metamethod_id != Metamethod::MetamethodId::METAMETHOD_ID_INVALID;

	}

	bool MetamethodMembers::isGetter(Metamethod::MetamethodId _metamethod_id){
		auto getter_info = getGetterInfo(_metamethod_id);

		return getter_info.metamethod_id != Metamethod::MetamethodId::METAMETHOD_ID_INVALID;
	}

	bool MetamethodMembers::isGetter(const zs_string & _str){
		auto getter_info = getGetterInfo(_str);

		return getter_info.metamethod_id != Metamethod::MetamethodId::METAMETHOD_ID_INVALID;

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
