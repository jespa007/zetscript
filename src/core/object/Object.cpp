/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	Object::Object(ZetScript *_zs, TypeId _type_id){
		type_id=_type_id;
		shared_pointer=NULL;
		properties=0;

		map_builtin_fields=new MapString;
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
		stk_this.properties=STACK_ELEMENT_PROPERTY_OBJECT;

		ref_objects=NULL;//new List<RefObject *>();

		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(type_id >= TYPE_ID_OBJECT_STRING && type_id < TYPE_ID_OBJECT_CLASS){
			Type *type=getType();
			Vector<Symbol *> *symbol_vars=type->scope->symbol_variables;
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for(int i = 0; i < symbol_vars->length(); i++){
				Symbol * symbol = (Symbol *)symbol_vars->get(i);
				if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
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

	void Object::unrefStackElementContainer(StackElement *si){
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

				if( (var_type & STACK_ELEMENT_PROPERTY_OBJECT)
					&&(si->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (si->value != 0)
				 ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_object(vm,(Object *)si->value,NULL);
				}
				break;
		}

	}

	void Object::unrefAndFreeStackElementContainer(StackElement *si){
		unrefStackElementContainer(si);
		free(si);
	}

	StackElement *Object::newBuiltinSlot(){
		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk_builtin_elements.push(stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * Object::addBuiltinField(
			const String & symbol_value
			, StackElement stk
	){
		String key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		map_builtin_fields->set(key_value.toConstChar(),(zs_int)new_stk);

  	    return new_stk;
	}

	int Object::length(){
		return 0;
	}

	const char  * Object::getTypeName(){
		return getType()->name.toConstChar();
	}

	const char * ClassObject::getTypeNamePtr(){
		return getType()->native_name.toConstChar();
	}

	bool Object::isNativeObject(){
		 return (getType()->properties & ZS_TYPE_PROPERTY_C_OBJECT_REF) != 0;
	}

	Type * Object::getType(){
		return this->zs->getTypeFactory()->getType(type_id);
	}

	Symbol *Object::getGetter(){
		Type *type=this->zs->getTypeFactory()->getType(type_id);
		MetamethodMembers *metamethod_members=&type->metamethod_members;

		if(metamethod_members !=NULL){
			return metamethod_members->getter;
		}
		return NULL;
	}

	Vector<StackElement *> *Object::getSetterList(Metamethod _metamethod){
		Type *type=this->zs->getTypeFactory()->getType(type_id);
		MetamethodMembers *metamethod_members=&type->metamethod_members;

		if(metamethod_members !=NULL){
			MetamethodMemberSetterInfo info=metamethod_members->getSetterInfo(_metamethod);
			return info.setters;
		}
		return NULL;
	}

	ZetScript      * Object::getZetScript() {
		return zs;
	}

	StackElement * 			Object::getBuiltinField(const String & _key_name){
		bool exists=false;
		zs_int element=this->map_builtin_fields->get(_key_name.toConstChar(), &exists);
		if(exists){
			return (StackElement *)element;
		}
		return NULL;
	}

	StackElement 	* Object::getStackElementByKeyName(const String & _key_name){
		return getBuiltinField(_key_name);
	}

	Symbol 	* Object::getFunctionSymbol(const String & _function_member_name){
		Type *type=getType();
		Vector<Symbol *> *s=type->scope->symbol_functions;
		for(int i=((int)s->length())-1;i>=0;i--){
			Symbol *symbol=(Symbol *)s->get(i);
			if(symbol->name == _function_member_name){
				return symbol;
			}
		}
		return NULL;
	}

	StackElement *Object::getThis(){
		return &this->stk_this;
	}

	void	*Object::getNativeObject(){
		return this;
	}

	Vector<StackElement *> * Object::getStkBuiltinListElements(){
		return &stk_builtin_elements;
	}

	StackElement * Object::setStackElementByKeyName(
		const String & _key_value
		,StackElement * stk_element
	){
		ZS_UNUSUED_2PARAMS(_key_value, stk_element);
		ZS_VM_SET_USER_ERRORF(vm,"XXXXX::set() is not implemented");
		return NULL;
	}

	StackElement * Object::getBuiltinField(int idx){
		if(idx >= (int)stk_builtin_elements.length() || idx < 0){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}
		return (StackElement *)stk_builtin_elements.get(idx);
	}

	String Object::toString(){
		return "Object@"+String(getTypeName());
	}




	void Object::attachRefObjectNode(ListNode<RefObjectObject *> *_ref_object_node){
		if(ref_objects==NULL){
			ref_objects=new List<RefObjectObject *>;
		}
		ref_objects->insert(_ref_object_node);
	}


	void Object::deattachRefObjectNode(ListNode<RefObjectObject *> *_ref_object_node){
		if(ref_objects==NULL){
			ZS_VM_SET_USER_ERRORF(vm,"ref_objects==NULL");
			return;
		}
		ref_objects->remove(_ref_object_node);
	}

	TypesFactory		*	Object::getTypeFactory(){
		return zs->getTypeFactory();
	}

	void Object::onDettachRefObjectNode(ListNode<RefObjectObject *> *_current_node){
		_current_node->data->setRefObject(NULL);
	}

	MapString *Object::getMapStringBuiltinFields(){
		return map_builtin_fields;
	}

	Object::~Object(){
		// deallocate built-in function member objects
		for(int i=0; i< stk_builtin_elements.length(); i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.get(i);

			if(stk->properties & STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY){
				delete (StackElementMemberProperty *)stk->value;
			}else if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){ // is script object to be deferrenced
				if((stk->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (stk->value != 0)
				  ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_object(vm,(Object *)stk->value,NULL);
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
