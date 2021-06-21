/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptObject::unrefAndFreeStackElementContainer(StackElement *si){
		ScriptFunction * ir_fun;
		unsigned short var_type = GET_STK_PROPERTY_TYPES(si->properties);

		switch(var_type){

			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_NULL:
			case STK_PROPERTY_ZS_FLOAT:
				break;
			case STK_PROPERTY_FUNCTION:
				 ir_fun  = (ScriptFunction *)(si->value);
				break;
			default: // properties ...

				if(var_type & STK_PROPERTY_SCRIPT_OBJECT){
					if(((si->properties & STK_PROPERTY_IS_VAR_C) != STK_PROPERTY_IS_VAR_C)
						&& (si->value != this) // ensure that property don't holds its same var.
						&& (si->value != 0)
					  ){ // deallocate but not if is c or this ref

						// remove property if not referenced anymore
						if(!vm_unref_shared_script_object_and_remove_if_zero(vm,(ScriptObject **)&si->value)){
							return false;
						}

					}
				}
				break;
		}

		free(si);

		return true;
	}

	ScriptObject::ScriptObject(){
		idx_script_class=ZS_IDX_UNDEFINED;
		shared_pointer=NULL;
		zs=NULL;
		map_builtin_property_keys=new zs_map();
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=this;
		stk_this.properties=STK_PROPERTY_SCRIPT_OBJECT;
		vm=NULL;
	}

	void ScriptObject::init(ZetScript *_zs){
		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(idx_script_class >= IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING && idx_script_class<IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS){
			ScriptClass *script_class=getScriptClass();
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for ( unsigned i = 0; i < script_class->symbol_members->count; i++){
				Symbol * symbol = (Symbol *)script_class->symbol_members->items[i];
				addBuiltinProperty(symbol->name.c_str(),{new StackMemberFunction(this,(ScriptFunction *)symbol->ref_ptr),STK_PROPERTY_MEMBER_FUNCTION | STK_PROPERTY_FUNCTION});
			}
		}

	}

	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_builtin_elements.push_back((zs_int)stk);
		return stk;
	}


	int  ScriptObject::getBuiltinPropertyIdx(const std::string & property_name){//,bool only_var_name){

		bool exists;
		int idx_stk_element=this->map_builtin_property_keys->get(property_name.c_str(),exists);
		if(exists){
			return idx_stk_element;
		}
		return ZS_IDX_UNDEFINED;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinProperty(const std::string & symbol_value, StackElement stk){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		map_builtin_property_keys->set(key_value.c_str(),stk_builtin_elements.count);

		StackElement *new_stk=newBuiltinSlot();
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

	ScriptFunction *ScriptObject::getGetter(){
		ScriptClass *script_class=this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
		MemberAttribute *member_attribute=script_class->setter_getter;

		if(member_attribute !=NULL){
			return member_attribute->getter;
		}

		return NULL;
	}

	zs_vector *ScriptObject::getSetterList(){
		ScriptClass *script_class=this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
		MemberAttribute *member_attribute=script_class->setter_getter;

		if(member_attribute !=NULL){
			return &member_attribute->setters;
		}

		return NULL;
	}

	ZetScript      * ScriptObject::getZetScript() {
		return zs;
	}

	StackElement * 			ScriptObject::getBuiltinProperty(const std::string & property_name, int * idx){
		/*if(property_name == "length"){
			stk_length={(void *)this->length(),STK_PROPERTY_ZS_INT};
			return &stk_length;
		}else {*/
			bool exists=false;
			zs_int idx_stk=this->map_builtin_property_keys->get(property_name.c_str(),exists);

			if(exists){
				return (StackElement *)this->stk_builtin_elements.items[idx_stk];
			}
		//}
		return NULL;
	}

	StackElement 	* ScriptObject::getProperty(const std::string & property_name, int * idx){
		return getBuiltinProperty(property_name,idx);
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->stk_this;
	}

	void	*ScriptObject::getNativeObject(){
		return this;
	}

	zs_vector * ScriptObject::getStkBuiltinListElements(){
		return &stk_builtin_elements;
	}

	StackElement * ScriptObject::addProperty(
			const std::string & symbol_value
			,std::string & error
			,StackElement * stk_element

	){
		VM_SET_USER_ERROR(vm,"addProperty is not implemented");
		return NULL;
	}

	StackElement * ScriptObject::getBuiltinElementAt(short idx){
		if(idx >= (int)stk_builtin_elements.count){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_builtin_elements.items[idx];
	}

	std::string ScriptObject::toString(){
		return "Object@"+getClassName();
	}

	ScriptObject::~ScriptObject(){

		// deallocate built-in function member objects
		for(unsigned i=0; i< stk_builtin_elements.count; i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.items[i];

			if(stk->properties & STK_PROPERTY_MEMBER_FUNCTION){
				delete (StackMemberFunction *)stk->value;
			}
			free(stk);
		}

		stk_builtin_elements.clear();
		delete map_builtin_property_keys;
		//map_builtin_property_keys=NULL;
	}
}
