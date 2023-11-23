/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObject::ScriptObject(ZetScript *_zs, ScriptTypeId _script_type_id){
		script_type_id=_script_type_id;
		shared_pointer=NULL;
		properties=0;

		map_builtin_fields=new zs_map;
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
		stk_this.properties=STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT;

		ref_objects=NULL;//new zs_list<RefObject *>();

		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(script_type_id >= SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING && script_type_id < SCRIPT_TYPE_ID_SCRIPT_OBJECT_CLASS){
			ScriptType *script_type=getScriptType();
			zs_vector<Symbol *> *symbol_vars=script_type->scope->symbol_variables;
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for(int i = 0; i < symbol_vars->size(); i++){
				Symbol * symbol = (Symbol *)symbol_vars->get(i);
				if(symbol->properties & ZS_SYMBOL_PROPERTY_MEMBER_PROPERTY){
					addBuiltinField(
							symbol->name
							,{(zs_int)(new StackElementMemberProperty(
									this
									,(MemberProperty *)symbol->ref_ptr)
							)
							,STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY}
					);
				}
			}
		}
	}

	void ScriptObject::unrefStackElementContainer(StackElement *si){
		unsigned short var_type = STACK_ELEMENT_TYPE_PROPERTIES(si->properties);

		switch(var_type){

			case STACK_ELEMENT_PROPERTY_BOOL:
			case STACK_ELEMENT_PROPERTY_INT:
			case STACK_ELEMENT_PROPERTY_UNDEFINED:
			case STACK_ELEMENT_PROPERTY_NULL:
			case STACK_ELEMENT_PROPERTY_FLOAT:
				break;
			case STACK_ELEMENT_PROPERTY_CONTAINER_SLOT:
				ContainerSlot::deleteContainerSlot((ContainerSlot *)si->value);
				break;
			case STACK_ELEMENT_PROPERTY_FUNCTION:
				break;
			default: // properties ...

				if( (var_type & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT)
					&&(si->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (si->value != 0)
				 ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_script_object(vm,(ScriptObject *)si->value,NULL);
				}
				break;
		}

	}

	void ScriptObject::unrefAndFreeStackElementContainer(StackElement *si){
		unrefStackElementContainer(si);
		free(si);
	}

	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk_builtin_elements.push_back(stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinField(
			const zs_string & symbol_value
			, StackElement stk
	){
		zs_string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		map_builtin_fields->set(key_value.c_str(),(zs_int)new_stk);

  	    return new_stk;
	}

	int ScriptObject::length(){
		return 0;
	}

	const char  * ScriptObject::getTypeName(){
		return getScriptType()->name.c_str();
	}

	const char * ClassScriptObject::getTypeNamePtr(){
		return getScriptType()->native_name.c_str();
	}

	bool ScriptObject::isNativeObject(){
		 return (getScriptType()->properties & ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF) != 0;
	}

	ScriptType * ScriptObject::getScriptType(){
		return this->zs->getScriptTypeFactory()->getScriptType(script_type_id);
	}

	Symbol *ScriptObject::getGetter(){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(script_type_id);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

		if(metamethod_members !=NULL){
			return metamethod_members->getter;
		}
		return NULL;
	}

	zs_vector<StackElement *> *ScriptObject::getSetterList(Metamethod _metamethod){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(script_type_id);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

		if(metamethod_members !=NULL){
			MetamethodMemberSetterInfo info=metamethod_members->getSetterInfo(_metamethod);
			return info.setters;
		}
		return NULL;
	}

	ZetScript      * ScriptObject::getZetScript() {
		return zs;
	}

	StackElement * 			ScriptObject::getBuiltinField(const zs_string & _key_name){
		bool exists=false;
		zs_int element=this->map_builtin_fields->get(_key_name.c_str(), &exists);
		if(exists){
			return (StackElement *)element;
		}
		return NULL;
	}

	StackElement 	* ScriptObject::getStackElementByKeyName(const zs_string & _key_name){
		return getBuiltinField(_key_name);
	}

	Symbol 	* ScriptObject::getScriptFunctionSymbol(const zs_string & _function_member_name){
		ScriptType *script_type=getScriptType();
		zs_vector<Symbol *> *s=script_type->scope->symbol_functions;
		for(int i=((int)s->size())-1;i>=0;i--){
			Symbol *symbol=(Symbol *)s->get(i);
			if(symbol->name == _function_member_name){
				return symbol;
			}
		}
		return NULL;
	}

	StackElement *ScriptObject::getThis(){
		return &this->stk_this;
	}

	void	*ScriptObject::getNativeObject(){
		return this;
	}

	zs_vector<StackElement *> * ScriptObject::getStkBuiltinListElements(){
		return &stk_builtin_elements;
	}

	StackElement * ScriptObject::setStackElementByKeyName(
		const zs_string & _key_value
		,StackElement * stk_element
	){
		ZS_UNUSUED_2PARAMS(_key_value, stk_element);
		ZS_VM_SET_USER_ERRORF(vm,"XXXXX::set() is not implemented");
		return NULL;
	}

	StackElement * ScriptObject::getBuiltinField(int idx){
		if(idx >= (int)stk_builtin_elements.size() || idx < 0){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}
		return (StackElement *)stk_builtin_elements.get(idx);
	}

	zs_string ScriptObject::toString(){
		return "Object@"+zs_string(getTypeName());
	}




	void ScriptObject::attachRefObjectNode(zs_list_node<RefObjectScriptObject *> *_ref_object_node){
		if(ref_objects==NULL){
			ref_objects=new zs_list<RefObjectScriptObject *>;
		}
		ref_objects->insert(_ref_object_node);
	}


	void ScriptObject::deattachRefObjectNode(zs_list_node<RefObjectScriptObject *> *_ref_object_node){
		if(ref_objects==NULL){
			ZS_VM_SET_USER_ERRORF(vm,"ref_objects==NULL");
			return;
		}
		ref_objects->remove(_ref_object_node);
	}

	ScriptTypeFactory		*	ScriptObject::getScriptTypeFactory(){
		return zs->getScriptTypeFactory();
	}

	void ScriptObject::onDettachRefObjectNode(zs_list_node<RefObjectScriptObject *> *_current_node){
		_current_node->data->setRefObject(NULL);
	}

	zs_map *ScriptObject::getMapBuiltinFields(){
		return map_builtin_fields;
	}

	ScriptObject::~ScriptObject(){
		// deallocate built-in function member objects
		for(int i=0; i< stk_builtin_elements.size(); i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.get(i);

			if(stk->properties & STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY){
				delete (StackElementMemberProperty *)stk->value;
			}else if(stk->properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){ // is script object to be deferrenced
				if((stk->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (stk->value != 0)
				  ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_script_object(vm,(ScriptObject *)stk->value,NULL);
				}
			}
			free(stk);
		}
		stk_builtin_elements.clear();
		delete map_builtin_fields;

		if(ref_objects!=NULL){
			ref_objects->dettachAllNodes(onDettachRefObjectNode);

			delete ref_objects;
		}

	}
}
