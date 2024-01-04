/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	struct MetamethodMemberSetterInfo {
		Metamethod metamethod;
		Vector<StackElement *> *setters;
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
		Vector<StackElement *> setters; // =
		Vector<StackElement *> add_setters; // +=
		Vector<StackElement *> sub_setters; // -=
		Vector<StackElement *> mul_setters; // *=
		Vector<StackElement *> div_setters; // /=
		Vector<StackElement *> mod_setters; // %=
		Vector<StackElement *> and_setters; // &=
		Vector<StackElement *> or_setters; // |=
		Vector<StackElement *> xor_setters; // ^=
		Vector<StackElement *> shl_setters; // <<=
		Vector<StackElement *> shr_setters; // >>=
		Symbol *getter,*postinc,*postdec,*preinc,*predec,*neg,*bwc; // getter

		const static Metamethod 	member_setter_list[];
		const static Metamethod 	member_list[];

		//const static String & 			MetamethodHelper::getMetamethodSymbolName(MetamethodHelper _byte_code, const String & _property_name);
		//bool static 						isMetamethodMember(MetamethodHelper _metamethod);
	//	bool static 						isMetamethodMember(const String & _metamethod);
		static 	MetamethodHelper 			symbolNameToSetterMetamethod(const String & _metamethod_name);
		bool 								isSetter(Metamethod _metamethod);
		bool 								isSetter(const String & _metamethod_name);
		bool 								isGetter(Metamethod _metamethod);
		bool 								isGetter(const String & _metamethod_name);


		MetamethodMembers();
		MetamethodMemberSetterInfo 			getSetterInfo(Metamethod _metamethod);
		MetamethodMemberSetterInfo 			getSetterInfo(const String & _metamethod_name);

		MetamethodMemberGetterInfo 			getGetterInfo(Metamethod _metamethod);
		MetamethodMemberGetterInfo 			getGetterInfo(const String & _metamethod_name);

		void								addSetter(Metamethod _metamethod,Symbol *f);
		Vector<StackElement *> * 		getArraySetter(MetamethodHelper _metamethod);



		~MetamethodMembers();

	};
}

