/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		Metamethod metamethod;
		zs_vector<StackElement *> *setters;
		const char *metamethod_name;

		MetamethodMemberSetterInfo() {
			metamethod = METAMETHOD_INVALID;
			setters = NULL;
			metamethod_name = NULL;

		}
	};

	struct MetamethodMemberGetterInfo {
		Metamethod metamethod;
		Symbol **getter;
		const char *metamethod_name;

		MetamethodMemberGetterInfo() {
			metamethod = METAMETHOD_INVALID;
			getter = NULL;
			metamethod_name = NULL;

		}

		MetamethodMemberGetterInfo(
				Metamethod _metamethod
				,Symbol **_getter
				,const char *_metamethod_name
		) {
			metamethod = _metamethod;
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
		Symbol *getter,*postinc,*postdec,*preinc,*predec,*neg,*bwc; // getter

		const static Metamethod 	member_setter_list[];
		const static Metamethod 	member_list[];

		//const static zs_string & 			MetamethodHelper::getSymbolName(MetamethodHelper _byte_code, const zs_string & _property_name);
		//bool static 						isMetamethodMember(MetamethodHelper _metamethod);
	//	bool static 						isMetamethodMember(const zs_string & _metamethod);
		static 	MetamethodHelper 			symbolNameToSetterMetamethod(const zs_string & _metamethod_name);
		bool 								isSetter(Metamethod _metamethod);
		bool 								isSetter(const zs_string & _metamethod_name);
		bool 								isGetter(Metamethod _metamethod);
		bool 								isGetter(const zs_string & _metamethod_name);


		MetamethodMembers();
		MetamethodMemberSetterInfo 			getSetterInfo(Metamethod _metamethod);
		MetamethodMemberSetterInfo 			getSetterInfo(const zs_string & _metamethod_name);

		MetamethodMemberGetterInfo 			getGetterInfo(Metamethod _metamethod);
		MetamethodMemberGetterInfo 			getGetterInfo(const zs_string & _metamethod_name);

		void								addSetter(Metamethod _metamethod,Symbol *f);
		zs_vector<StackElement *> * 		getArraySetter(MetamethodHelper _metamethod);



		~MetamethodMembers();

	};
}

