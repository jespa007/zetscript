/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,_metamethod_name,_property_name) \
	sprintf(_str_out,"%s@%s,",_metamethod_name,_property_name)


#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_BYTE_CODE(_str_out,_metamethod_byte_code,_property_name) \
	ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(_str_out,metamethod_byte_code_to_symbol_str(_metamethod_byte_code),_property_name)


/*
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD(_str_out,_metamethod_byte_code,_property_name) \
	sprintf(_str_out,"%s@%s,",metamethod_byte_code_to_symbol_str(_metamethod_byte_code),_property_name)
*/

#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER		"_get@"
/*#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NEG			"_neg@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_BWC			"_bwc@"
*/


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
