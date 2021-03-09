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
		map_property_keys_builtin=new zs_map();
		memset(&stk_this,0,sizeof(stk_this));
	}

	void ScriptObject::setZetScript(ZetScript *_zs){
		zs=_zs;
	}

	StackElement *ScriptObject::newSlotBuiltin(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=stk_undefined;
		stk_elements_builtin.push_back((zs_int)stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addPropertyBuiltIn(const std::string & symbol_value, StackElement stk){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		map_property_keys_builtin->set(key_value.c_str(),stk_elements_builtin.count);

		StackElement *new_stk=newSlotBuiltin();
		*new_stk=stk;

  	    return new_stk;
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
			stk_length={(void *)this->length(),MSK_STK_PROPERTY_ZS_INT};
			return &stk_length;
		}else {
			bool exists=false;
			zs_int idx_stk=this->map_property_keys_builtin->get(property_name.c_str(),exists);

			if(exists){
				return (StackElement *)this->stk_elements_builtin.items[idx_stk];
			}
		}
		return NULL;
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->stk_this;
	}

	void	*ScriptObject::getNativeObject(){
		return this;
	}

	zs_vector * ScriptObject::getAllElements(){
		return &stk_elements_builtin;
	}

	StackElement * ScriptObject::addProperty(
			const std::string & symbol_value
			//, const ScriptFunction *info_function
			//, Instruction *src_instruction
			,std::string & error
			,StackElement * stk_element
			,int * idx_stk_element

	){
		return NULL;
	}

	StackElement * ScriptObject::getElementAt(short idx){
		if(idx >= (int)stk_elements_builtin.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_elements_builtin.items[idx];
	}

	std::string ScriptObject::toString(){
		return "@"+getClassName();
	}

	ScriptObject::~ScriptObject(){
		// deallocate built-in function member objects
		for(unsigned i=0; i< stk_elements_builtin.count; i++){
			StackElement *stk=(StackElement *)stk_elements_builtin.items[i];
			if(stk->properties & MSK_STK_PROPERTY_FUNCTION_MEMBER){
				delete (FunctionMember *)stk->stk_value;
			}
			free(stk);
		}

		stk_elements_builtin.clear();
		delete map_property_keys_builtin;
	}
}
