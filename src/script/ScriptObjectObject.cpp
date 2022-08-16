/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----------------------------------------------
	//
	// Helpers
	//
	ScriptObjectObject * ScriptObjectObject::newScriptObjectObject(ZetScript	*_zs){
		ScriptObjectObject *ac= new ScriptObjectObject();
		ac->init(_zs);
		return ac;
	}

	ScriptObjectObject *ScriptObjectObject::newShareableScriptObjectObject(ZetScript	*_zs){
		VirtualMachine *virtual_machine = _zs->getVirtualMachine();

		ScriptObjectObject *so= new ScriptObjectObject();
		so->init(_zs);
		// share this variable++
		vm_create_shared_script_object(virtual_machine,(ScriptObject *)so);
		vm_share_script_object(virtual_machine,(ScriptObject *)so);
		return so;
	}


	ScriptObjectObject * ScriptObjectObject::concat(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		zs_string error;
		ScriptObjectObject *obj = ZS_NEW_OBJECT_OBJECT(zs);

		// get properties from object o1
		zs_map *map=o1->getMapUserProperties();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->addUserProperty(it.key,error,(StackElement *)it.value);
		}

		// get properties from object o2
		map=o2->getMapUserProperties();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->addUserProperty(it.key,error,(StackElement *)it.value);
		}
		return obj;
	}

	void  ScriptObjectObject::append(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		ZS_UNUSUED_PARAM(zs);
		zs_string error;

		// get properties from object o2
		zs_map *map=o2->getMapUserProperties();
		for(auto it=map->begin(); !it.end();it.next()){
			o1->addUserProperty(it.key,error,(StackElement *)it.value);
		}
	}
	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_properties=new zs_map();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const char * symbol_value
			,zs_string & error
			,StackElement * sv
		){
		StackElement si;

		if(map_user_properties->exist(symbol_value)){
			error=zs_strutils::format("'%s' symbol already exists",symbol_value);
			return NULL;
		}

		if(sv != NULL){
			si = *sv;
			// update n_refs +1
			if(sv->properties&STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(vm,(ScriptObjectObject *)(sv->value)) == false){
					return NULL;
				}
			}
		}else{

			si=k_stk_undefined;
		}

		zs_string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		map_user_properties->set(key_value.c_str(),(zs_int)new_stk);

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const char * property_name){

		bool exists;
		StackElement *stk_element=(StackElement *)this->map_user_properties->get(property_name,&exists);
		if(exists){
			return stk_element;
		}
		return NULL;
	}

	StackElement * ScriptObjectObject::addProperty(
			const char * symbol_value
			,zs_string & _error
			,StackElement * stk_element
	){

		return addUserProperty(symbol_value,_error,stk_element);
	}

	StackElement 	* ScriptObjectObject::getProperty(const char  * property_name){
		StackElement *stk=getBuiltinProperty(property_name);
		if(stk==NULL){
			stk=getUserProperty(property_name/*,idx*/);
		}
		return stk;
	}

	bool ScriptObjectObject::existUserProperty(const char * property_name){
		return map_user_properties->exist(property_name);
	}

	zs_map *ScriptObjectObject::getMapUserProperties(){
		return map_user_properties;
	}

	zs_map *ScriptObjectObject::getMapBuiltinProperties(){
		return map_builtin_properties;
	}

	int	ScriptObjectObject::length(){
		return this->map_user_properties->count();
	}

	bool ScriptObjectObject::eraseUserProperty(const char * property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_properties->get(property_name,&exists);
		if(!exists){
			VM_SET_USER_ERROR(vm,"Property %s not exist",property_name);
			return false;
		}

		free(stk_user_element);
		map_user_properties->erase(property_name); // erase also property key
		return true;
	}

	void ScriptObjectObject::eraseAllUserProperties(){


		for(auto it=map_user_properties->begin();!it.end();it.next()){
			StackElement *si=(StackElement *)(it.value);
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_properties->clear();
	}

	zs_string ScriptObjectObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true,false);
	}



	ScriptObjectObject::~ScriptObjectObject(){

		eraseAllUserProperties();
		delete map_user_properties;
	}
}
