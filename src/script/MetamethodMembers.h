/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		MetamethodByteCode metamethod_byte_code;
		zs_vector<StackElement *> *setters;
		const char *metamethod_name;

		MetamethodMemberSetterInfo() {
			metamethod_byte_code = MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;
			setters = NULL;
			metamethod_name = NULL;

		}
	};

	struct MetamethodMemberGetterInfo {
		MetamethodByteCode metamethod_byte_code;
		Symbol **getter;
		const char *metamethod_name;

		MetamethodMemberGetterInfo() {
			metamethod_byte_code = MetamethodByteCode::METAMETHOD_BYTE_CODE_INVALID;
			getter = NULL;
			metamethod_name = NULL;

		}

		MetamethodMemberGetterInfo(
				MetamethodByteCode _metamethod_byte_code
				,Symbol **_getter
				,const char *_metamethod_name
		) {
			metamethod_byte_code = _metamethod_byte_code;
			getter = _getter;
			metamethod_name = _metamethod_name;

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

		const static MetamethodByteCode 	metamethod_byte_code_member_setter_list[];
		const static MetamethodByteCode 	metamethod_byte_code_member_list[];

		//const static zs_string & 			metamethod_byte_code_to_symbol_str(MetamethodByteCode _byte_code, const zs_string & _property_name);
		//bool static 						isMetamethodMember(MetamethodByteCode _metamethod_byte_code);
	//	bool static 						isMetamethodMember(const zs_string & _metamethod_byte_code);
		static 	MetamethodByteCode 			symbolNameToSetterMetamethod(const zs_string & _metamethod_name);
		bool 								isSetter(MetamethodByteCode _metamethod_byte_code);
		bool 								isSetter(const zs_string & _metamethod_name);
		bool 								isGetter(MetamethodByteCode _metamethod_byte_code);
		bool 								isGetter(const zs_string & _metamethod_name);


		MetamethodMembers();
		MetamethodMemberSetterInfo 			getSetterInfo(MetamethodByteCode _metamethod_byte_code);
		MetamethodMemberSetterInfo 			getSetterInfo(const zs_string & _metamethod_name);

		MetamethodMemberGetterInfo 			getGetterInfo(MetamethodByteCode _metamethod_byte_code);
		MetamethodMemberGetterInfo 			getGetterInfo(const zs_string & _metamethod_name);

		void								addSetter(MetamethodByteCode _metamethod_byte_code,Symbol *f);
		zs_vector<StackElement *> * 		getArraySetter(MetamethodByteCode _metamethod_byte_code);



		~MetamethodMembers();

	};
}

