/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	MemberProperty::MemberProperty(ScriptType *_script_class,const String & _property_name){
		property_name=_property_name;
		script_type=_script_class;
	}

	bool MemberProperty::symbolNameMatchStartSymbolNameMetamethod(const String & _symbol_name){
		String symbol="N/A";
		String symbol_name_methametod;
		const Metamethod *it=MetamethodMembers::member_list;

		const char * at=strchr(_symbol_name.toConstChar(),'@');

		if(at == NULL){
			return false;
		}

		at++;

		// custom case get...
		if(String(at).startsWith(METAMETHOD_SYMBOL_NAME_GETTER)){
			return true;
		}

		//
		while(*it!=0){
			if(String(at).startsWith(MetamethodHelper::getMetamethodSymbolName(*it))){
				return true;
			}
			it++;
		}

		return false;
	}

	MemberProperty::~MemberProperty(){

	}
}
