/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

//#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER		"_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(_byte_code_metamethod,_property_name) zs_string(byte_code_metamethod_to_symbol_str(_byte_code_metamethod))+"@"+_property_name
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER		"_get@"
/*#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_INC		"_post_inc@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_POST_DEC		"_post_dec@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_INC		"_pre_inc@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_PRE_DEC		"_pre_dec@"

#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_ADD_SET		"_add_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SUB_SET		"_sub_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_DIV_SET		"_div_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_MOD_SET		"_mod_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_AND_SET		"_and_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_OR_SET		"_or_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_XOR_SET		"_xor_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SHL_SET		"_shl_set@"
#define ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SHR_SET		"_shr_set@"
*/

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		ByteCodeMetamethod byte_code_metamethod;
		zs_vector *setters;
		const char *str_byte_code_metamethod;

		MetamethodMemberSetterInfo() {
			byte_code_metamethod = ByteCodeMetamethod::BYTE_CODE_METAMETHOD_INVALID;
			setters = NULL;
			str_byte_code_metamethod = NULL;

		}
	};

	class MetamethodMembers{
	public:
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
		Symbol *getter,*post_inc,*post_dec,*pre_inc,*pre_dec,*neg; // getter

		const static ByteCodeMetamethod 	byte_code_metamethod_member_setter_list[];
		const static ByteCodeMetamethod 	byte_code_metamethod_member_list[];

		//const static zs_string & 			byte_code_metamethod_to_symbol_str(ByteCodeMetamethod _byte_code, const zs_string & _property_name);
		bool static 						isMetamethodMember(ByteCodeMetamethod _byte_code);
		static 	ByteCodeMetamethod symbolNameToSetterMetamethod(const char *_symbol_name);
		static bool isSetter(ByteCodeMetamethod _byte_code_metamethod);

		MetamethodMembers();
		MetamethodMemberSetterInfo 	getSetterInfo(ByteCodeMetamethod _byte_code);
		MetamethodMemberSetterInfo 	getSetterInfo(const char *_symbol_name);
		void						addSetter(ByteCodeMetamethod _byte_code_metamethod,Symbol *f);
		zs_vector * 				getVectorSetter(ByteCodeMetamethod _byte_code_metamethod);



		~MetamethodMembers();

	};
}

