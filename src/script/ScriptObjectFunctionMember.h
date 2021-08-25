/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#define ZS_NEW_OBJECT_FUNCTION_MEMBER(zs) (ScriptObjectVector::newScriptObjectFunctionMember(zs))

namespace zetscript{
	class ScriptObjectFunctionMember:public ScriptObject{
	public:
		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectFunctionMember * newScriptObjectFunctionMember(ZetScript *zs);

		ScriptFunction *so_function;
		ScriptObject *so_object;
	};
}
