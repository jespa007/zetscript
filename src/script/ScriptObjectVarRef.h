/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs,stk,idx_call) ScriptObjectVarRef::newScriptObjectVarRef(zs,stk,idx_call)

namespace zetscript{

	class  ScriptObjectVarRef: public ScriptObject{

	public:
		static ScriptObjectVarRef * newScriptObjectVarRef(ZetScript *zs, StackElement _stk_var_ref, int idx_call); // idx call
		ScriptObjectVarRef();

		void setIdxCall(int idx_call);
		int getIdxCall();
		StackElement *getStackElementPtr();
		void setStackElement(StackElement stk);

		virtual std::string toString(const std::string _format="");

	private:
		StackElement stk_var_ref;
		int idx_call;
		void setup();
	};

}
