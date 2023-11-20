/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		Metamethod::MetamethodId metamethod_id;
		zs_vector<StackElement *> *setters;
		const char *metamethod_name;

		MetamethodMemberSetterInfo() {
			metamethod_id = Metamethod::MetamethodId::METAMETHOD_ID_INVALID;
			setters = NULL;
			metamethod_name = NULL;

		}
	};

	struct MetamethodMemberGetterInfo {
		Metamethod::MetamethodId metamethod_id;
		Symbol **getter;
		const char *metamethod_name;

		MetamethodMemberGetterInfo() {
			metamethod_id = Metamethod::MetamethodId::METAMETHOD_ID_INVALID;
			getter = NULL;
			metamethod_name = NULL;

		}

		MetamethodMemberGetterInfo(
				Metamethod::MetamethodId _metamethod_id
				,Symbol **_getter
				,const char *_metamethod_name
		) {
			metamethod_id = _metamethod_id;
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

		const static Metamethod::MetamethodId 	member_setter_list[];
		const static Metamethod::MetamethodId 	member_list[];

		//const static zs_string & 			Metamethod::toSymbolString(Metamethod _byte_code, const zs_string & _property_name);
		//bool static 						isMetamethodMember(Metamethod _metamethod_id);
	//	bool static 						isMetamethodMember(const zs_string & _metamethod_id);
		static 	Metamethod 			symbolNameToSetterMetamethod(const zs_string & _metamethod_name);
		bool 								isSetter(Metamethod::MetamethodId _metamethod_id);
		bool 								isSetter(const zs_string & _metamethod_name);
		bool 								isGetter(Metamethod::MetamethodId _metamethod_id);
		bool 								isGetter(const zs_string & _metamethod_name);


		MetamethodMembers();
		MetamethodMemberSetterInfo 			getSetterInfo(Metamethod::MetamethodId _metamethod_id);
		MetamethodMemberSetterInfo 			getSetterInfo(const zs_string & _metamethod_name);

		MetamethodMemberGetterInfo 			getGetterInfo(Metamethod::MetamethodId _metamethod_id);
		MetamethodMemberGetterInfo 			getGetterInfo(const zs_string & _metamethod_name);

		void								addSetter(Metamethod::MetamethodId _metamethod_id,Symbol *f);
		zs_vector<StackElement *> * 		getArraySetter(Metamethod _metamethod_id);



		~MetamethodMembers();

	};
}

