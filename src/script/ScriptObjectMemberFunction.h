/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs) (ScriptObjectMemberFunction::newScriptObjectMemberFunction(zs))

namespace zetscript{
	class ScriptObjectMemberFunction:public ScriptObject{
	public:
		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectMemberFunction * newScriptObjectMemberFunction(ZetScript *zs);

		ScriptFunction *so_function;
		ScriptObject *so_object;
	};
}
