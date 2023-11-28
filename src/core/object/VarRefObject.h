/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs,stk) VarRefObject::newVarRefObject(zs,stk)

namespace zetscript{

	class  VarRefObject: public Object{

	public:
		static VarRefObject * newVarRefObject(ZetScript *zs, StackElement _stk_var_ref); // idx call
		VarRefObject(ZetScript *_zs);

		//void setIdxCall(int idx_call);
		//int getIdxCall();
		StackElement *getStackElementPtr();
		void setStackElement(StackElement stk);

		virtual String toString();

	private:
		StackElement stk_var_ref;
		//int idx_call;
		void setup();
	};

}
