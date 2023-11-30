/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class MemberProperty;
	class ScriptObject;
	struct StackElementMemberProperty{
	public:
		MemberProperty 		*member_property;
		ScriptObject		*so_object;

		StackElementMemberProperty(
				ScriptObject		*_so_object
				,MemberProperty 	*_member_property){
			so_object = _so_object;
			member_property= _member_property;
		}
	};

}
