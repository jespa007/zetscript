/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptObject::unrefAndFreeStackElementContainer(StackElement *si){
		ScriptFunction 	* ir_fun;
		ContainerSlot 	* container_slot=NULL;
		ContainerScriptObject *so_container_ref=NULL;
		unsigned short var_type = GET_STK_PROPERTY_TYPES(si->properties);

		switch(var_type){

			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_UNDEFINED:
			case STK_PROPERTY_NULL:
			case STK_PROPERTY_ZS_FLOAT:
				break;
			case STK_PROPERTY_CONTAINER_SLOT:
				container_slot=(ContainerSlot *)si->value;
				so_container_ref=container_slot->getSrcContainerRef();
				if(so_container_ref!=NULL){
					so_container_ref->removeSlot(container_slot,NULL);
				}
				delete container_slot;
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
					if(!vm_unref_shared_script_object(vm,(ScriptObject *)si->value,NULL)){
						return false;
					}
				}
				break;
		}
		free(si);
		return true;
	}

	ScriptObject::ScriptObject(ZetScript *_zs){
		idx_script_type=ZS_IDX_UNDEFINED;
		shared_pointer=NULL;

		map_builtin_properties=new zs_map;
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
		stk_this.properties=STK_PROPERTY_SCRIPT_OBJECT;

		ref_objects=NULL;//new zs_list<RefObject *>();

		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(idx_script_type >= IDX_TYPE_SCRIPT_OBJECT_STRING && idx_script_type<IDX_TYPE_SCRIPT_OBJECT_CLASS){
			ScriptType *script_type=getScriptType();
			zs_vector<Symbol *> *symbol_vars=script_type->scope_script_type->symbol_variables;
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for(int i = 0; i < symbol_vars->size(); i++){
				Symbol * symbol = (Symbol *)symbol_vars->items[i];
				if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
					addBuiltinProperty(
							symbol->name
							,{(zs_int)(new StackMemberProperty(
									this
									,(MemberProperty *)symbol->ref_ptr)
							)
							,STK_PROPERTY_MEMBER_PROPERTY}
					);
				}
			}
		}
	}


	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk_builtin_elements.push_back(stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinProperty(
			const zs_string & symbol_value
			, StackElement stk
	){
		zs_string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		map_builtin_properties->set(key_value.c_str(),(zs_int)new_stk);

  	    return new_stk;
	}

	int ScriptObject::length(){
		return 0;
	}

	const char  * ScriptObject::getTypeName(){
		return getScriptType()->str_script_type.c_str();
	}

	const char * ClassScriptObject::getTypeNamePtr(){
		return getScriptType()->str_script_type_ptr.c_str();
	}

	bool ScriptObject::isNativeObject(){
		 return (getScriptType()->properties & SCRIPT_TYPE_PROPERTY_C_OBJECT_REF) != 0;
	}

	ScriptType * ScriptObject::getScriptType(){
		return this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
	}

	Symbol *ScriptObject::getGetter(){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

		if(metamethod_members !=NULL){
			return metamethod_members->getter;
		}
		return NULL;
	}

	zs_vector<StackElement *> *ScriptObject::getSetterList(ByteCodeMetamethod _byte_code_metamethod){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

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
		zs_int element=this->map_builtin_properties->get(property_name.c_str(), &exists);
		if(exists){
			return (StackElement *)element;
		}
		return NULL;
	}

	StackElement 	* ScriptObject::getProperty(const zs_string & property_name){
		return getBuiltinProperty(property_name);
	}

	Symbol 	* ScriptObject::getScriptFunctionSymbol(const zs_string & _function_member_name){
		ScriptType *script_type=getScriptType();
		zs_vector<Symbol *> *s=script_type->scope_script_type->symbol_functions;
		for(int i=((int)s->size())-1;i>=0;i--){
			Symbol *symbol=(Symbol *)s->items[i];
			if(symbol->name == _function_member_name){
				return symbol;
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

	zs_vector<StackElement *> * ScriptObject::getStkBuiltinListElements(){
		return &stk_builtin_elements;
	}

	StackElement * ScriptObject::addProperty(
		const zs_string & symbol_value
		,zs_string & error
		,StackElement * stk_element
	){
		ZS_UNUSUED_3PARAMS(symbol_value, error, stk_element);
		VM_SET_USER_ERRORF(vm,"addProperty is not implemented");
		return NULL;
	}

	StackElement * ScriptObject::getBuiltinElementAt(int idx){
		if(idx >= (int)stk_builtin_elements.size() || idx < 0){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}
		return (StackElement *)stk_builtin_elements.items[idx];
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
			VM_SET_USER_ERRORF(vm,"ref_objects==NULL");
			return;
		}
		ref_objects->remove(_ref_object_node);
	}

	ScriptTypeFactory		*	ScriptObject::getScriptTypeFactory(){
		return zs->getScriptTypeFactory();
	}

	ScriptObject::~ScriptObject(){
		// deallocate built-in function member objects
		for(int i=0; i< stk_builtin_elements.size(); i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.items[i];

			if(stk->properties & STK_PROPERTY_MEMBER_PROPERTY){
				delete (StackMemberProperty *)stk->value;
			}else if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){ // is script object to be deferrenced
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
		delete map_builtin_properties;

		if(ref_objects!=NULL){
			auto first_node=ref_objects->first;
			auto current_node=first_node;
			if(current_node != NULL){
				do{
					current_node=current_node->next;

					current_node->data->setRefObject(NULL); //deref script object reference

				}while(current_node!=first_node);
			}

			delete ref_objects;
		}

	}
}
