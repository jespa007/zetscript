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
	ObjectScriptObject * ObjectScriptObject::newScriptObjectObject(ZetScript	*_zs){
		return new ObjectScriptObject(_zs);
	}

	ObjectScriptObject * ObjectScriptObject::concat(ZetScript *zs,ObjectScriptObject *o1,ObjectScriptObject *o2){
		zs_string error;
		ObjectScriptObject *obj = ZS_NEW_OBJECT_OBJECT(zs);

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

	void  ObjectScriptObject::append(ZetScript *zs,ObjectScriptObject *o1,ObjectScriptObject *o2){
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

	ObjectScriptObject::ObjectScriptObject(
			ZetScript	*_zs
	):ContainerScriptObject(_zs,IDX_TYPE_SCRIPT_OBJECT_OBJECT){
		map_user_properties=new zs_map();
	}

	StackElement * ObjectScriptObject::addUserProperty(
			const zs_string &  symbol_value
			,zs_string & error
			,StackElement * sv
		){
		StackElement si;

		if(map_user_properties->exist(symbol_value.c_str())){
			error=zs_strutils::format("'%s' symbol already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;
			// update n_refs +1
			if(sv->properties&STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_script_object(vm,(ObjectScriptObject *)(sv->value)) == false){
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

	StackElement * ObjectScriptObject::getUserProperty(const zs_string &  property_name){

		bool exists;
		StackElement *stk_element=(StackElement *)this->map_user_properties->get(property_name.c_str(),&exists);
		if(exists){
			return stk_element;
		}
		return NULL;
	}

	StackElement * ObjectScriptObject::addProperty(
			const zs_string & symbol_value
			,zs_string & _error
			,StackElement * stk_element
	){

		return addUserProperty(symbol_value,_error,stk_element);
	}

	StackElement 	* ObjectScriptObject::getProperty(const zs_string &  property_name){
		StackElement *stk=getBuiltinProperty(property_name);
		if(stk==NULL){
			stk=getUserProperty(property_name/*,idx*/);
		}
		return stk;
	}

	bool ObjectScriptObject::existUserProperty(const zs_string &  property_name){
		return map_user_properties->exist(property_name.c_str());
	}

	zs_map *ObjectScriptObject::getMapUserProperties(){
		return map_user_properties;
	}

	zs_map *ObjectScriptObject::getMapBuiltinProperties(){
		return map_builtin_properties;
	}

	int	ObjectScriptObject::length(){
		return this->map_user_properties->count();
	}

	bool ObjectScriptObject::eraseUserProperty(const zs_string &  property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_properties->get(property_name.c_str(),&exists);
		if(!exists){
			ZS_VM_SET_USER_ERROR(vm,"Property %s not exist",property_name.c_str());
			return false;
		}

		free(stk_user_element);
		map_user_properties->erase(property_name.c_str()); // erase also property key
		return true;
	}

	void ObjectScriptObject::eraseAllUserProperties(){

		for(auto it=map_user_properties->begin();!it.end();it.next()){
			StackElement *si=(StackElement *)(it.value);
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_properties->clear();
	}

	zs_string ObjectScriptObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true,false);
	}

	ObjectScriptObject::~ObjectScriptObject(){

		eraseAllUserProperties();
		delete map_user_properties;
	}
}
