/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

//#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER		"_set@"
//#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(_byte_code_metamethod,_property_name) byte_code_metamethod_to_symbol_str(_byte_code_metamethod)+"@"+_property_name
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER		"_get@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_INC		"_post_inc@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_DEC		"_post_dec@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_INC		"_pre_inc@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_DEC		"_pre_dec@"



namespace zetscript{

	typedef struct{
		ByteCodeMetamethod byte_code_metamethod;
		zs_vector *setters;
		const char *str_byte_code_metamethod;
	}MemberPropertyInfo;

	class MemberProperty{
	public:
		ScriptClass *script_class;
		zs_vector setters; // =
		zs_vector add_setters; // +=
		zs_vector sub_setters; // -=
		zs_vector mul_setters; // *=
		zs_vector div_setters; // /=
		zs_vector mod_setters; // %=
		zs_vector and_setters; // &=
		zs_vector or_setters; // |=
		zs_vector xor_setters; // ^=
		zs_vector shl_setters; // <<=
		zs_vector shr_setters; // >>=
		 ScriptFunction *getter,*post_inc,*post_dec,*pre_inc,*pre_dec; // getter
		 zs_string property_name;

		 const static ByteCodeMetamethod *byte_code_metamethod_list;

		 MemberProperty(ScriptClass *_script_class, const zs_string & _property_name);
		 MemberPropertyInfo getInfo(ByteCodeMetamethod _symbol_name);
		 MemberPropertyInfo getInfo(const char *_symbol_name);
		 void addSetter(ByteCodeMetamethod _byte_code_metamethod,ScriptFunction *f);
		 zs_vector * getVectorSetter(const char *_symbol_name);

		~MemberProperty();

	};
}

