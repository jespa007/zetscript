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

				if( (var_type & STK_PROPERTY_SCRIPT_OBJECT)
					&&(si->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (si->value != 0)
				 ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					if(!vm_unref_shared_script_object_and_remove_if_zero(vm,(ScriptObject **)&si->value)){
						return false;
					}
				}
				break;
		}
		free(si);
		return true;
	}

	ScriptObject::ScriptObject(){
		idx_script_class=IDX_ZS_UNDEFINED;
		shared_pointer=NULL;
		zs=NULL;
		map_builtin_property_keys=new zs_map();
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
		stk_this.properties=STK_PROPERTY_SCRIPT_OBJECT;
		vm=NULL;
	}

	void ScriptObject::init(ZetScript *_zs){
		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(idx_script_class >= IDX_TYPE_SCRIPT_OBJECT_STRING && idx_script_class<IDX_TYPE_SCRIPT_OBJECT_CLASS){
			ScriptClass *script_class=getScriptClass();
			zs_vector *symbol_vars=script_class->class_scope->symbol_variables;
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for ( unsigned i = 0; i < symbol_vars->count; i++){
				Symbol * symbol = (Symbol *)symbol_vars->items[i];
				if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
					addBuiltinProperty(symbol->name.c_str(),{(zs_int)(new StackMemberProperty(this,(MemberProperty *)symbol->ref_ptr)),STK_PROPERTY_MEMBER_PROPERTY});
				}
			}
		}
	}

	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk_builtin_elements.push_back((zs_int)stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinProperty(const zs_string & symbol_value, StackElement stk){
		zs_string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		map_builtin_property_keys->set(key_value.c_str(),(zs_int)new_stk);

  	    return new_stk;
	}

	size_t ScriptObject::length(){
		return 0;
	}

	const zs_string & ScriptObject::getClassName(){
		return getScriptClass()->class_name;
	}

	const char * ScriptObjectClass::getNativePointerClassName(){
		return getScriptClass()->str_class_ptr_type;
	}

	bool ScriptObject::isNativeObject(){
		 return (getScriptClass()->properties & SCRIPT_CLASS_PROPERTY_C_OBJECT_REF) != 0;
	}

	ScriptClass * ScriptObject::getScriptClass(){
		return this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
	}

	ScriptFunction *ScriptObject::getGetter(){
		ScriptClass *script_class=this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
		MetamethodMembers *metamethod_members=&script_class->metamethod_members;

		if(metamethod_members !=NULL){
			return metamethod_members->getter;
		}
		return NULL;
	}

	zs_vector *ScriptObject::getSetterList(ByteCodeMetamethod _byte_code_metamethod){
		ScriptClass *script_class=this->zs->getScriptClassFactory()->getScriptClass(idx_script_class);
		MetamethodMembers *metamethod_members=&script_class->metamethod_members;

		if(metamethod_members !=NULL){
			MetamethodMemberSetterInfo info=metamethod_members->getSetterInfo(_byte_code_metamethod);
			return info.setters;
		}
		return NULL;
	}

	ZetScript      * ScriptObject::getZetScript() {
		return zs;
	}

	StackElement * 			ScriptObject::getBuiltinProperty(const zs_string & property_name){
		bool exists=false;
		StackElement  *stk_item=(StackElement  *)this->map_builtin_property_keys->get(property_name.c_str(),&exists);

		if(exists){
			return stk_item;
		}
		return NULL;
	}

	StackElement 	* ScriptObject::getProperty(const zs_string & property_name){
		return getBuiltinProperty(property_name);
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
			const zs_string & symbol_value
			,zs_string & error
			,StackElement * stk_element

	){
		VM_SET_USER_ERROR(vm,"addProperty is not implemented");
		return NULL;
	}

	StackElement * ScriptObject::getBuiltinElementAt(short idx){
		if(idx >= (int)stk_builtin_elements.count || idx < 0){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}
		return (StackElement *)stk_builtin_elements.items[idx];
	}

	zs_string ScriptObject::toString(){
		return "Object@"+getClassName();
	}

	ScriptObject::~ScriptObject(){
		// deallocate built-in function member objects
		for(unsigned i=0; i< stk_builtin_elements.count; i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.items[i];

			if(stk->properties & STK_PROPERTY_MEMBER_PROPERTY){
				delete (StackMemberProperty *)stk->value;
			}else if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){ // is script object to be deferrenced
				if((stk->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (stk->value != 0)
				  ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_script_object_and_remove_if_zero(vm,(ScriptObject **)&stk->value);
				}
			}
			free(stk);
		}
		stk_builtin_elements.clear();
		delete map_builtin_property_keys;
	}
}
