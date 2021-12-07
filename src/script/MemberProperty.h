/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

//#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER		"_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(_byte_code_metamethod,_property_name) zs_string(byte_code_metamethod_to_symbol_str(_byte_code_metamethod))+"@"+_property_name
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER		"_get@"


namespace zetscript{
	class MemberProperty{
	public:
		ScriptClass *script_class;
		 zs_string property_name;
		 MetamethodMembers metamethod_members;

		 static bool 						symbolNameMatchStartSymbolNameMetamethod(const zs_string & _symbol_name);

		 MemberProperty(ScriptClass *_script_class, const zs_string & _property_name);

		~MemberProperty();


	};
}