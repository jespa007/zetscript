/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		ByteCodeMetamethod byte_code_metamethod;
		zs_vector<StackElement *> *setters;
		const char *str_byte_code_metamethod;

		MetamethodMemberSetterInfo() {
			byte_code_metamethod = ByteCodeMetamethod::BYTE_CODE_METAMETHOD_INVALID;
			setters = NULL;
			str_byte_code_metamethod = NULL;

		}
	};

	class MetamethodMembers{
	public:
		zs_vector<StackElement *> setters; // =
		zs_vector<StackElement *> add_setters; // +=
		zs_vector<StackElement *> sub_setters; // -=
		zs_vector<StackElement *> mul_setters; // *=
		zs_vector<StackElement *> div_setters; // /=
		zs_vector<StackElement *> mod_setters; // %=
		zs_vector<StackElement *> and_setters; // &=
		zs_vector<StackElement *> or_setters; // |=
		zs_vector<StackElement *> xor_setters; // ^=
		zs_vector<StackElement *> shl_setters; // <<=
		zs_vector<StackElement *> shr_setters; // >>=
		Symbol *getter,*post_inc,*post_dec,*pre_inc,*pre_dec,*neg,*bwc; // getter

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
		zs_vector<StackElement *> * 				getArraySetter(ByteCodeMetamethod _byte_code_metamethod);



		~MetamethodMembers();

	};
}

