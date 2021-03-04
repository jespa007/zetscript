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

		stk_length.stk_value=0;
		stk_length.properties=MSK_STK_PROPERTY_ZS_INT;

	}

	void ScriptObject::setZetScript(ZetScript *_zs){
		zs=_zs;
	}

	size_t ScriptObject::length(){
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

	StackElement * 			ScriptObject::getProperty(const std::string & property_name, int * idx){
		if(property_name == "length"){
			stk_length.stk_value=(void *)this->length();
			return &stk_length;
		}
		return NULL;
	}

	std::string ScriptObject::toString(){
		return "@"+getClassName();
	}

	ScriptObject::~ScriptObject(){

	}


}
