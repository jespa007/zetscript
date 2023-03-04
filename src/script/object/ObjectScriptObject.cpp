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
		//zs_string error;
		ObjectScriptObject *obj = ZS_NEW_OBJECT_OBJECT(zs);

		// get properties from object o1
		zs_map *map=o1->getMapFields();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->set(it.key,(StackElement *)it.value);
		}

		// get properties from object o2
		map=o2->getMapFields();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->set(it.key,(StackElement *)it.value);
		}
		return obj;
	}

	void  ObjectScriptObject::append(ZetScript *zs,ObjectScriptObject *o1,ObjectScriptObject *o2){
		ZS_UNUSUED_PARAM(zs);
		zs_string error;

		// get properties from object o2
		zs_map *fields=o2->getMapFields();
		for(auto it=fields->begin(); !it.end();it.next()){
			o1->set(it.key,(StackElement *)it.value);
		}
	}

	//
	// Helpers
	//
	//----------------------------------------------

	ObjectScriptObject::ObjectScriptObject(
			ZetScript	*_zs
	):ContainerScriptObject(_zs,IDX_TYPE_SCRIPT_OBJECT_OBJECT){
		map_fields=new zs_map();
	}

	StackElement * ObjectScriptObject::set(
			const zs_string &  _key_name
			//,zs_string & error
			,StackElement * sv
		){
		StackElement si;

		bool exists=false;
		if(map_builtin_fields->exist(_key_name.c_str())){
			ZS_THROW_RUNTIME_ERROR("Cannot set value on '%s' field because is internally used",_key_name.c_str());
		}

		StackElement *stk_user_element = (StackElement *)map_fields->get(_key_name.c_str(),&exists);
		if(exists){
			ScriptObject::unrefAndFreeStackElementContainer(stk_user_element);
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

		zs_string key_name = _key_name;
		StackElement *new_stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		map_fields->set(key_name.c_str(),(zs_int)new_stk);

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ObjectScriptObject::setInteger(
			const zs_string &  _key_name
			,zs_int _value
	){
		StackElement stk={_value,STK_PROPERTY_ZS_INT};

		return set(_key_name,&stk);
	}

	StackElement * ObjectScriptObject::setFloat(
			const zs_string &  _key_name
			,zs_float _value
	){
		zs_int dst;
		StackElement stk;
		ZS_WRITE_INTPTR_FLOAT(&dst,_value);
		stk={dst,STK_PROPERTY_ZS_FLOAT};

		return set(_key_name,&stk);
	}

	StackElement * ObjectScriptObject::setBoolean(
			const zs_string &  _key_name
			,bool _value
	){
		StackElement stk={_value,STK_PROPERTY_BOOL};

		return set(_key_name,&stk);
	}

	StackElement * ObjectScriptObject::setString(
			const zs_string &  _key_name
			,const zs_string & _value
	){
		StringScriptObject *so=this->zs->newStringScriptObject();
		StackElement stk={(zs_int)so,STK_PROPERTY_SCRIPT_OBJECT};

		so->set(_value);

		return set(_key_name,&stk);
	}

	StackElement 	* ObjectScriptObject::get(const zs_string &  _key_name){
		StackElement *stk=getBuiltinField(_key_name);
		if(stk==NULL){
			// get user field
			bool exists;
			StackElement *stk_element=(StackElement *)this->map_fields->get(_key_name.c_str(),&exists);
			if(exists){
				return stk_element;
			}
			return NULL;
		}
		return stk;
	}

	bool ObjectScriptObject::exists(const zs_string &  _key_name){
		if(map_builtin_fields->exist(_key_name.c_str())){
			return true;
		}
		return map_fields->exist(_key_name.c_str());
	}

	zs_map *ObjectScriptObject::getMapFields(){
		return map_fields;
	}

	int	ObjectScriptObject::length(){
		return this->map_fields->count();
	}

	zs_vector<zs_string> ObjectScriptObject::getKeys(){
		zs_vector<zs_string> keys;
		for(auto mi=map_fields->begin();!mi.end();mi.next()){
				keys.push_back(mi.key);
		}
		return keys;
	}

	bool ObjectScriptObject::erase(const zs_string &  _property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_fields->get(_property_name.c_str(),&exists);
		if(!exists){
			ZS_VM_SET_USER_ERROR(vm,"field '%s' not exist",_property_name.c_str());
			return false;
		}

		map_fields->erase(_property_name.c_str()); // erase also property key
		ScriptObject::unrefAndFreeStackElementContainer(stk_user_element);
		return true;
	}

	void ObjectScriptObject::eraseAll(){

		for(auto it=map_fields->begin();!it.end();it.next()){
			StackElement *si=(StackElement *)(it.value);
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_fields->clear();
	}

	zs_string ObjectScriptObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,false,false);
	}

	ObjectScriptObject::~ObjectScriptObject(){

		eraseAll();
		delete map_fields;
	}
}
