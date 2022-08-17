/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs,stk) ScriptObjectVarRef::newScriptObjectVarRef(zs,stk)

namespace zetscript{

	class  ScriptObjectVarRef: public ScriptObject{

	public:
		static ScriptObjectVarRef * newScriptObjectVarRef(ZetScript *zs, StackElement _stk_var_ref); // idx call
		ScriptObjectVarRef();

		//void setIdxCall(int idx_call);
		//int getIdxCall();
		StackElement *getStackElementPtr();
		void setStackElement(StackElement stk);

		virtual std::string toString();

	private:
		StackElement stk_var_ref;
		//int idx_call;
		void setup();
	};

}
