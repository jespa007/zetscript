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
		//zs_map_iterator it_1=o1->map_user_property_keys->begin();
		//zs_map_iterator it_2=o2->map_user_property_keys->begin();

		for(auto it=o1->begin(); !it.end();it.next()){
			obj->addUserProperty(it.key,error,(StackElement *)it.value);
		}

		for(auto it=o2->begin(); !it.end();it.next()){
			obj->addUserProperty(it.key,error,(StackElement *)it.value);
		}
		return obj;
	}

	void  ScriptObjectObject::append(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		zs_string error="";

		for(auto it=o2->begin(); !it.end();it.next()){
			o1->addUserProperty(it.key,error,(StackElement *)it.value);
		}

	}

	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_script_class=IDX_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_property_keys=new zs_map();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const zs_string & symbol_value
			,zs_string & error
			,StackElement * sv
		){
		StackElement si;
		bool error_symbol=false;

		if(map_user_property_keys->exist(symbol_value.c_str())){
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

		/*if(idx_stk_element != NULL){
			*idx_stk_element=stk_user_elements.count;
		}*/

		zs_string key_value = symbol_value;
		//StackElement *new_stk=pushNewUserSlot();
		StackElement *new_stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		map_user_property_keys->set(key_value.c_str(),(zs_int)new_stk);

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const zs_string & property_name){

		bool exists;
		StackElement *stk_element=(StackElement *)this->map_user_property_keys->get(property_name.c_str(),&exists);
		if(exists){
			/*if(idx!=NULL){
				*idx=idx_stk_element;
			}

			return (StackElement *)stk_user_elements.items[idx_stk_element];*/
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
		return map_user_property_keys->exist(property_name.c_str());
	}

	zs_map_iterator ScriptObjectObject::begin(){
		return this->map_user_property_keys->begin();
	}

	zs_map_iterator ScriptObjectObject::begin_builtin(){
		return this->map_builtin_property_keys->begin();
	}

	size_t	ScriptObjectObject::length(){
		return this->map_builtin_property_keys->count;
	}


	bool ScriptObjectObject::eraseUserProperty(const zs_string & property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_property_keys->get(property_name.c_str(),&exists);
		if(!exists){
			VM_SET_USER_ERROR(vm,"Property %s not exist",property_name.c_str());
			return false;
		}

		/*if(!eraseUserElementAt(stk_user_element)){
			return false;
		}*/
		free(stk_user_element);

		map_user_property_keys->erase(property_name.c_str()); // erase also property key

		return true;
	}

	void ScriptObjectObject::eraseAllUserProperties(){
		for(auto it=map_user_property_keys->begin(); !it.end(); it.next()){
			StackElement *si=(StackElement *)it.value;
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_property_keys->clear();

	}

	zs_string ScriptObjectObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ScriptObjectObject::~ScriptObjectObject(){
		eraseAllUserProperties();
		delete map_user_property_keys;

	}
}
