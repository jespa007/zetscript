/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObject::ScriptObject(ZetScript * _zs){
		idx_script_class=ZS_IDX_UNDEFINED;
		zs=_zs;
		shared_pointer=NULL;

	}

	zs_int ScriptObject::count(){
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

	bool ScriptObject::initSharedPtr(){
		// is assigned means that when is created is assigned immediately ?

		if(shared_pointer == NULL){
			if((shared_pointer = this->zs->getVirtualMachine()->newSharedPointer(this))==NULL){
				return false;
			}
		}
		else{
			VM_SET_USER_ERROR(this->zs->getVirtualMachine()," shared ptr already registered");
			return false;
		}

		return true;
	}

	bool ScriptObject::unrefSharedPtr(int _idx_current_call){
		if(shared_pointer!=NULL){
			if(!this->zs->getVirtualMachine()->unrefSharedScriptObject(shared_pointer,_idx_current_call)){
				return false;
			}
		}
		else{
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"shared ptr not registered");
			return false;
		}
		return true;
	}

	std::string ScriptObject::toString(){
		return "@Object:"+getClassName();
	}

	ScriptObject::~ScriptObject(){

	}


}
