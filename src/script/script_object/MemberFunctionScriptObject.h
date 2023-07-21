/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (MemberFunctionScriptObject::newScriptObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class MemberFunctionScriptObject:public RefObjectScriptObject{
	public:
		ScriptFunction *sf_ref;

		//----------------------------------------------
		//
		// Helpers
		//
		static MemberFunctionScriptObject * newScriptObjectMemberFunction(
				ZetScript *_zs
				, ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);
		//----------------------------------------------
		//
		// Members
		//
		MemberFunctionScriptObject(
				ZetScript *zs
				,ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);

		~MemberFunctionScriptObject();


	};
}
