/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (MemberFunctionScriptObject::newScriptObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class MemberFunctionScriptObject:public RefObjectScriptObject{
	public:
		ScriptFunction *so_function;

		//----------------------------------------------
		//
		// Helpers
		//
		static MemberFunctionScriptObject * newScriptObjectMemberFunction(ZetScript *zs, ScriptObject *_so_object, ScriptFunction *_so_function);
		//----------------------------------------------
		//
		// Members
		//
		MemberFunctionScriptObject();
		~MemberFunctionScriptObject();
	private:

		void setup();

	};
}
