/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (ScriptObjectMemberFunction::newScriptObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class ScriptObjectMemberFunction:public ScriptObject{
	public:
		ScriptFunction *so_function;
		ScriptObject *so_object;

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectMemberFunction * newScriptObjectMemberFunction(ZetScript *zs, ScriptObject *_so_object, ScriptFunction *_so_function);
		//----------------------------------------------
		//
		// Members
		//
		ScriptObjectMemberFunction();
		~ScriptObjectMemberFunction();
	private:

		void setup();

	};
}
