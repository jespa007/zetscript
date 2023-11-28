/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	MemberFunctionObject * MemberFunctionObject::newObjectMemberFunction(ZetScript *_zs, Object *_so_object_ref, Function *_sf_ref){

		return new MemberFunctionObject(_zs,_so_object_ref,_sf_ref);
	}

	MemberFunctionObject::MemberFunctionObject(
			ZetScript *_zs
			, Object *_so_ref
			, Function *_sf_ref
	):RefObjectObject(_zs, _so_ref){
		type_id=TYPE_ID_OBJECT_FUNCTION_MEMBER;
		this->sf_ref=_sf_ref;
	}



	MemberFunctionObject::~MemberFunctionObject(){
	}

}
