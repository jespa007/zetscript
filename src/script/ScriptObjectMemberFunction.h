/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs) (ScriptObjectMemberFunction::newScriptObjectMemberFunction(zs))

namespace zetscript{
	class ScriptObjectMemberFunction:public ScriptObject{
	public:
		ScriptFunction *so_function;
		ScriptObject *so_object;

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectMemberFunction * newScriptObjectMemberFunction(ZetScript *zs);
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
