/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs,stk) VarRefScriptObject::newVarRefScriptObject(zs,stk)

namespace zetscript{

	class  VarRefScriptObject: public ScriptObject{

	public:
		static VarRefScriptObject * newVarRefScriptObject(ZetScript *zs, StackElement _stk_var_ref); // idx call
		VarRefScriptObject(ZetScript *_zs);

		//void setIdxCall(int idx_call);
		//int getIdxCall();
		StackElement *getStackElementPtr();
		void setStackElement(StackElement stk);

		virtual zs_string toString();

	private:
		StackElement stk_var_ref;
		//int idx_call;
		void setup();
	};

}
