/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectMemberFunction * ScriptObjectMemberFunction::newScriptObjectMemberFunction(ZetScript *zs){
		ScriptObjectMemberFunction *sofm=new ScriptObjectMemberFunction();
		sofm->init(zs);

		return sofm;

	}

	ScriptObjectMemberFunction::ScriptObjectMemberFunction(){
		this->so_function=NULL;
		this->so_object=NULL;

	}

}
