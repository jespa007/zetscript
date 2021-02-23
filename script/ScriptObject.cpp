/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObject::ScriptObject(){
		idx_script_class=ZS_IDX_UNDEFINED;
		shared_pointer=NULL;
		zs=NULL;
	}

	void ScriptObject::setZetScript(ZetScript *_zs){
		zs=_zs;
	}

	size_t ScriptObject::count(){
		return 0;
	}

	const std::string & ScriptObject::getClassName(){
		return getScriptClass()->symbol_class.name;
	}

	const std::string & ScriptObjectClass::getNativePointerClassName(){
		return getScriptClass()->str_class_ptr_type;
	}

	bool ScriptObject::isNativeObject(){
		 return (getScriptClass()->symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0;
	}

	ScriptClass * ScriptObject::getScriptClass(){
		return this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
	}

	ZetScript      * ScriptObject::getZetScript() {
		return zs;
	}

	std::string ScriptObject::toString(){
		return "@"+getClassName();
	}

	ScriptObject::~ScriptObject(){

	}


}
