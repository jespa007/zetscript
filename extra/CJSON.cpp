/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

namespace zetscript{

	void CJSON::registerScript(){
		register_C_Class<CJSON>("JSON");
		register_C_StaticFunctionMember<CJSON>("parse",&CJSON::parse);
	}

	CStructScriptVariable * CJSON::parse(const char *str){

		return NULL;//CZetScript::eval<CStructScriptVariable *>(str);

	}

}
