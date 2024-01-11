/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,_property_name,_metamethod_name) \
	sprintf(_str_out,"%s@%s",_property_name,_metamethod_name)


#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD(_str_out,_property_name,_metamethod) \
	ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,_property_name,MetamethodHelper::getMetamethodSymbolName(_metamethod))


namespace zetscript{
	class MemberProperty{
	public:
		ScriptType *script_type;
		 String property_name;
		 MetamethodMembers metamethod_members;

		 static bool 						symbolNameMatchStartSymbolNameMetamethod(const String & _symbol_name);

		 MemberProperty(ScriptType *_script_class, const String & _property_name);
		~MemberProperty();
	};
}
