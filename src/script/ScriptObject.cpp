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
					//if(((si->properties & STK_PROPERTY_IS_VAR_C) != STK_PROPERTY_IS_VAR_C)
					&&(si->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (si->value != 0)
				 ){ // deallocate but not if is c or this ref

						// remove property if not referenced anymore
						if(!vm_unref_shared_script_object_and_remove_if_zero(vm,(ScriptObject **)&si->value)){
							return false;
						}

					//}
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
		map_builtin_property_keys=new std::map<std::string,StackElement *>();
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
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
			for ( unsigned i = 0; i < script_class->symbol_member_variables->size(); i++){
				Symbol * symbol = (Symbol *)script_class->symbol_member_variables->at(i);
				if(symbol->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE){
					addBuiltinProperty(symbol->name.c_str(),{(zs_int)(new StackMemberAttribute(this,(MemberAttribute *)symbol->ref_ptr)),STK_PROPERTY_MEMBER_ATTRIBUTE});
				}/*else{
					addBuiltinProperty(symbol->name.c_str(),{(zs_int)(new StackMemberFunction(this,(ScriptFunction *)symbol->ref_ptr)),STK_PROPERTY_MEMBER_FUNCTION | STK_PROPERTY_FUNCTION});
				}*/
			}
		}

	}

	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_builtin_elements.push_back(stk);
		return stk;
	}


	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinProperty(const std::string & symbol_value, StackElement stk){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		(*map_builtin_property_keys)[key_value.c_str()]=new_stk;

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

	std::vector<StackElement *> *ScriptObject::getSetterList(){
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

	StackElement * 			ScriptObject::getBuiltinProperty(const std::string & property_name){

		bool exists=this->map_builtin_property_keys->count(property_name.c_str())>=1;

		if(exists){
			return (StackElement  *)this->map_builtin_property_keys->at(property_name.c_str());
		}

		return NULL;
	}

	StackElement 	* ScriptObject::getProperty(const std::string & property_name){
		return getBuiltinProperty(property_name);
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->stk_this;
	}

	void	*ScriptObject::getNativeObject(){
		return this;
	}

	std::vector<StackElement *> * ScriptObject::getStkBuiltinListElements(){
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
		if(idx >= (int)stk_builtin_elements.size() || idx < 0){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_builtin_elements.at(idx);
	}

	std::string ScriptObject::toString(){
		return "Object@"+getClassName();
	}

	ScriptObject::~ScriptObject(){

		// deallocate built-in function member objects
		for(unsigned i=0; i< stk_builtin_elements.size(); i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.at(i);

			/*if(stk->properties & STK_PROPERTY_MEMBER_FUNCTION){
				delete (StackMemberFunction *)stk->value;
			}else*/
			if(stk->properties & STK_PROPERTY_MEMBER_ATTRIBUTE){
				delete (StackMemberAttribute *)stk->value;
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
		//map_builtin_property_keys=NULL;
	}
}
