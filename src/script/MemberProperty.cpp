/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	MemberProperty::MemberProperty(ScriptType *_script_class,const zs_string & _property_name){
		property_name=_property_name;
		script_type=_script_class;
	}

	bool MemberProperty::symbolNameMatchStartSymbolNameMetamethod(const zs_string & _symbol_name){
		zs_string symbol="N/A";
		zs_string symbol_name_methametod;
		const Metamethod *it=MetamethodMembers::member_list;

		const char * at=strchr(_symbol_name.c_str(),'@');

		if(at == NULL){
			return false;
		}

		at++;

		// custom case get...
		if(zs_strutils::starts_with(at,METAMETHOD_SYMBOL_NAME_GETTER)){
			return true;
		}

		//
		while(*it!=0){
			if(zs_strutils::starts_with(at,MetamethodHelper::getSymbolName(*it))){
				return true;
			}
			it++;
		}

		return false;
	}

	MemberProperty::~MemberProperty(){

	}
}
