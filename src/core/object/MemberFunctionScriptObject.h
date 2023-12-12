/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (MemberFunctionScriptObject::newObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class MemberFunctionScriptObject:public RefScriptObject{
	public:
		ScriptFunction *sf_ref;

		//----------------------------------------------
		//
		// Helpers
		//
		static MemberFunctionScriptObject * newObjectMemberFunction(
				ScriptEngine *_script_engine
				, ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);
		//----------------------------------------------
		//
		// Members
		//
		MemberFunctionScriptObject(
				ScriptEngine *zs
				,ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);

		~MemberFunctionScriptObject();


	};
}
