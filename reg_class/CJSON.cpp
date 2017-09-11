#include "CZetScript.h"

namespace zetscript{

	void CJSON::registerScript(){
		register_C_Class<CJSON>("JSON");
		register_C_StaticFunctionMember<CJSON>("parse",&CJSON::parse);
	}

	CStructScriptVariable * CJSON::parse(const char *str){

		return CZetScript::eval<CStructScriptVariable *>(str);

	}

}
