/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_SETTER		"_set@"
#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_GETTER		"_get@"
#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_POST_INC	"_post_inc@"
#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_POST_DEC	"_post_dec@"
#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_PRE_INC		"_pre_inc@"
#define ZS_MEMBER_PROPERTY_SYMBOL_NAME_PRE_DEC		"_pre_dec@"

namespace zetscript{

	class MemberProperty{
	public:
		ScriptClass *script_class;
		zs_vector setters; // setter that contains a list of script functions C++
		zs_vector setters_add;
		zs_vector setters_sub;
		zs_vector setters_div;
		zs_vector setters_mod;
		zs_vector setters_and;
		zs_vector setters_or;
		zs_vector setters_xor;
		zs_vector setters_shl;
		zs_vector setters_shr;
		 ScriptFunction *getter,*post_inc,*post_dec,*pre_inc,*pre_dec; // getter
		 zs_string attribute_name;

		 MemberProperty(ScriptClass *_script_class, const zs_string & _attribute_name);

		 void addSetter(ScriptFunction *f);

		~MemberProperty();
	};
}

