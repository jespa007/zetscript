/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs,stk) VarRefObject::newVarRefObject(zs,stk)

namespace zetscript{

	class  VarRefObject: public ScriptObject{

	public:
		static VarRefObject * newVarRefObject(ScriptEngine *zs, StackElement _stk_var_ref); // idx call
		VarRefObject(ScriptEngine *_script_engine);

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
