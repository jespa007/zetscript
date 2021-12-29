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

	ScriptObjectObject * ScriptObjectObject::concat(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		zs_string error="";
		ScriptObjectObject *obj = ZS_NEW_OBJECT_OBJECT(zs);

		for(int i=0; i < o1->map_user_properties->count; i++){
			obj->addUserProperty((o1->map_user_properties->items+i)->key,error,(StackElement *)(o1->map_user_properties->items+i)->value);
		}

		for(int i=0; i < o2->map_user_properties->count; i++){
			obj->addUserProperty((o2->map_user_properties->items+i)->key,error,(StackElement *)(o2->map_user_properties->items+i)->value);
		}
		return obj;
	}

	void  ScriptObjectObject::append(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		zs_string error="";

		for(int i=0;i< o2->map_user_properties->count; i++){

			o1->addUserProperty((o2->map_user_properties->items+i)->key,error,(StackElement *)(o2->map_user_properties->items+i)->value);
		}
	}
	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_type=IDX_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_properties=new zs_map();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const zs_string & symbol_value
			,zs_string & error
			,StackElement * sv
		){
		StackElement si;
		bool error_symbol=false;

		if(map_user_properties->exist(symbol_value.c_str())){
			error=zs_strutils::format("'%s' symbol already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;
			// update n_refs +1
			if(sv->properties&STK_PROPERTY_SCRIPT_OBJECT){
				if(vm_share_pointer(vm,(ScriptObjectObject *)(sv->value)) == false){
					return NULL;
				}
			}
		}else{

			si=k_stk_null;
		}

		zs_string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		map_user_properties->set(key_value.c_str(),(zs_int)new_stk);

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const zs_string & property_name){

		bool exists;
		StackElement *stk_element=(StackElement *)this->map_user_properties->get(property_name.c_str(),&exists);
		if(exists){
			return stk_element;
		}
		return NULL;
	}

	StackElement * ScriptObjectObject::addProperty(
			const zs_string & symbol_value
			,zs_string & error
			,StackElement * stk_element
	){
		return addUserProperty(symbol_value,error);
	}

	StackElement 	* ScriptObjectObject::getProperty(const zs_string & property_name){
		StackElement *stk=getBuiltinProperty(property_name);
		if(stk==NULL){
			stk=getUserProperty(property_name/*,idx*/);
		}
		return stk;
	}

	bool ScriptObjectObject::existUserProperty(const zs_string & property_name){
		return map_user_properties->exist(property_name.c_str());
	}

	zs_map *ScriptObjectObject::getMapUserProperties(){
		return map_user_properties;
	}

	zs_map *ScriptObjectObject::getMapBuiltinProperties(){
		return map_builtin_properties;
	}

	size_t	ScriptObjectObject::length(){
		return this->map_builtin_properties->count;
	}

	bool ScriptObjectObject::eraseUserProperty(const zs_string & property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_properties->get(property_name.c_str(),&exists);
		if(!exists){
			VM_SET_USER_ERROR(vm,"Property %s not exist",property_name.c_str());
			return false;
		}

		free(stk_user_element);
		map_user_properties->erase(property_name.c_str()); // erase also property key
		return true;
	}

	void ScriptObjectObject::eraseAllUserProperties(){

		for(int i=0; i < map_user_properties->count; i++){
			StackElement *si=(StackElement *)(map_user_properties->items+i)->value;
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_properties->clear();
	}

	zs_string ScriptObjectObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ScriptObjectObject::~ScriptObjectObject(){
		eraseAllUserProperties();
		delete map_user_properties;
	}
}
