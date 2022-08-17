/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		ByteCodeMetamethod byte_code_metamethod;
		std::vector<StackElement *> *setters;
		const char *str_byte_code_metamethod;

		MetamethodMemberSetterInfo() {
			byte_code_metamethod = ByteCodeMetamethod::BYTE_CODE_METAMETHOD_INVALID;
			setters = NULL;
			str_byte_code_metamethod = NULL;

		}
	};

	class MetamethodMembers{
	public:
		std::vector<StackElement *> setters; // =
		std::vector<StackElement *> add_setters; // +=
		std::vector<StackElement *> sub_setters; // -=
		std::vector<StackElement *> mul_setters; // *=
		std::vector<StackElement *> div_setters; // /=
		std::vector<StackElement *> mod_setters; // %=
		std::vector<StackElement *> and_setters; // &=
		std::vector<StackElement *> or_setters; // |=
		std::vector<StackElement *> xor_setters; // ^=
		std::vector<StackElement *> shl_setters; // <<=
		std::vector<StackElement *> shr_setters; // >>=
		Symbol *getter,*post_inc,*post_dec,*pre_inc,*pre_dec,*neg; // getter

		const static ByteCodeMetamethod 	byte_code_metamethod_member_setter_list[];
		const static ByteCodeMetamethod 	byte_code_metamethod_member_list[];

		//const static std::string & 			byte_code_metamethod_to_symbol_str(ByteCodeMetamethod _byte_code, const std::string & _property_name);
		bool static 						isMetamethodMember(ByteCodeMetamethod _byte_code);
		static 	ByteCodeMetamethod symbolNameToSetterMetamethod(const char *_symbol_name);
		static bool isSetter(ByteCodeMetamethod _byte_code_metamethod);

		MetamethodMembers();
		MetamethodMemberSetterInfo 	getSetterInfo(ByteCodeMetamethod _byte_code);
		MetamethodMemberSetterInfo 	getSetterInfo(const char *_symbol_name);
		void						addSetter(ByteCodeMetamethod _byte_code_metamethod,Symbol *f);
		std::vector<StackElement *> * 				getVectorSetter(ByteCodeMetamethod _byte_code_metamethod);



		~MetamethodMembers();

	};
}

