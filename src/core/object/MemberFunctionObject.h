/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_MEMBER_FUNCTION(zs,so_object, so_function) (MemberFunctionObject::newObjectMemberFunction(zs,so_object,so_function))

namespace zetscript{
	class MemberFunctionObject:public RefObjectObject{
	public:
		Function *sf_ref;

		//----------------------------------------------
		//
		// Helpers
		//
		static MemberFunctionObject * newObjectMemberFunction(
				ZetScript *_zs
				, Object *_so_ref
				, Function *_sf_ref
		);
		//----------------------------------------------
		//
		// Members
		//
		MemberFunctionObject(
				ZetScript *zs
				,Object *_so_ref
				, Function *_sf_ref
		);

		~MemberFunctionObject();


	};
}
