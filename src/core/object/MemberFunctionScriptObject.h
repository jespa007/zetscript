/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (MemberFunctionObject::newObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class MemberFunctionObject:public RefScriptObject{
	public:
		ScriptFunction *sf_ref;

		//----------------------------------------------
		//
		// Helpers
		//
		static MemberFunctionObject * newObjectMemberFunction(
				ZetScript *_zs
				, ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);
		//----------------------------------------------
		//
		// Members
		//
		MemberFunctionObject(
				ZetScript *zs
				,ScriptObject *_so_ref
				, ScriptFunction *_sf_ref
		);

		~MemberFunctionObject();


	};
}
