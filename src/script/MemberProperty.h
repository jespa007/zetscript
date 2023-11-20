/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,_property_name,_metamethod_name) \
	sprintf(_str_out,"%s@%s",_property_name,_metamethod_name)


#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD(_str_out,_property_name,_metamethod_id) \
	ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,_property_name,Metamethod::toSymbolString(_metamethod_id))


namespace zetscript{
	class MemberProperty{
	public:
		ScriptType *script_type;
		 zs_string property_name;
		 MetamethodMembers metamethod_members;

		 static bool 						symbolNameMatchStartSymbolNameMetamethod(const zs_string & _symbol_name);

		 MemberProperty(ScriptType *_script_class, const zs_string & _property_name);
		~MemberProperty();
	};
}
