/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectFunctionMember * ScriptObjectFunctionMember::newScriptObjectFunctionMember(ZetScript *zs){
		ScriptObjectFunctionMember *sofm=new ScriptObjectFunctionMember();
		sofm->init(zs);

		return sofm;

	}

	ScriptObjectFunctionMember::ScriptObjectFunctionMember(){
		this->so_function=NULL;
		this->so_object=NULL;

	}

}
