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
		const ByteCodeMetamethod *it=MetamethodMembers::byte_code_metamethod_member_list;

		// custom case get...
		if(zs_strutils::starts_with(_symbol_name,ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER)){
			return true;
		}

		//
		while(*it!=0){
			symbol_name_methametod=byte_code_metamethod_to_symbol_str(*it);
			symbol_name_methametod.append('@');
			if(zs_strutils::starts_with(_symbol_name,symbol_name_methametod)){
				return true;
			}
			it++;
		}


		return false;
	}

	MemberProperty::~MemberProperty(){

	}


}
