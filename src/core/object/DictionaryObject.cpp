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
	DictionaryObject * DictionaryObject::newObjectObject(ZetScript	*_zs){
		return new DictionaryObject(_zs);
	}

	DictionaryObject * DictionaryObject::concat(ZetScript *zs,DictionaryObject *o1,DictionaryObject *o2){
		//String error;
		DictionaryObject *obj = ZS_NEW_DICTIONARY_OBJECT(zs);

		// get properties from object o1
		MapString *map=o1->getMapStringFields();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->setStackElementByKeyName(it.key,(StackElement *)it.value);
		}

		// get properties from object o2
		map=o2->getMapStringFields();
		for(auto it=map->begin(); !it.end();it.next()){
			obj->setStackElementByKeyName(it.key,(StackElement *)it.value);
		}
		return obj;
	}

	void  DictionaryObject::append(ZetScript *zs,DictionaryObject *o1,DictionaryObject *o2){
		ZS_UNUSUED_PARAM(zs);
		String error;

		// get properties from object o2
		MapString *fields=o2->getMapStringFields();
		for(auto it=fields->begin(); !it.end();it.next()){
			o1->setStackElementByKeyName(it.key,(StackElement *)it.value);
		}
	}

	//
	// Helpers
	//
	//----------------------------------------------

	DictionaryObject::DictionaryObject(
			ZetScript	*_zs
	):ContainerObject(_zs, TYPE_ID_DICTIONARY_OBJECT){
		map_fields=new MapString();
	}

	StackElement * DictionaryObject::setStackElementByKeyName(
			const String &  _key_name
			//,String & error
			,StackElement * _stk_src
		){

		bool exists=false;
		if(map_builtin_fields->exist(_key_name.toConstChar())){
			ZS_THROW_RUNTIME_ERROR("Cannot set value on '%s' field because is internally used",_key_name.toConstChar());
		}

		StackElement *stk_dst = (StackElement *)map_fields->get(_key_name.toConstChar(),&exists);
		if(exists){
			// unref current slot
			Object::unrefStackElementContainer(stk_dst);
		}else{
			// create new slot
			String key_name = _key_name;
			stk_dst=(StackElement *)ZS_MALLOC(sizeof(StackElement));
			map_fields->set(key_name.toConstChar(),(zs_int)stk_dst);

		}

		if(_stk_src != NULL){
			zs->stackElementAssign(stk_dst,_stk_src);
		}else{
			*stk_dst=k_stk_undefined;
		}

		return stk_dst;
	}

	StackElement 	* DictionaryObject::getStackElementByKeyName(const String &  _key_name){
		StackElement *stk=getBuiltinField(_key_name);
		if(stk==NULL){
			// get user field
			bool exists;
			StackElement *stk_element=(StackElement *)this->map_fields->get(_key_name.toConstChar(),&exists);
			if(exists){
				return stk_element;
			}
			return NULL;
		}
		return stk;
	}

	bool DictionaryObject::exists(const String &  _key_name){
		if(map_builtin_fields->exist(_key_name.toConstChar())){
			return true;
		}
		return map_fields->exist(_key_name.toConstChar());
	}

	MapString *DictionaryObject::getMapStringFields(){
		return map_fields;
	}

	int	DictionaryObject::length(){
		return this->map_fields->count();
	}

	Vector<String> DictionaryObject::getKeys(){
		Vector<String> keys;
		for(auto mi=map_fields->begin();!mi.end();mi.next()){
				keys.push(mi.key);
		}
		return keys;
	}

	bool DictionaryObject::erase(const String &  _property_name/*, const Function *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_fields->get(_property_name.toConstChar(),&exists);
		if(!exists){
			ZS_VM_SET_USER_ERROR(vm,"field '%s' not exist",_property_name.toConstChar());
			return false;
		}

		map_fields->erase(_property_name.toConstChar()); // erase also property key
		Object::unrefAndFreeStackElementContainer(stk_user_element);
		return true;
	}

	void DictionaryObject::eraseAll(){

		for(auto it=map_fields->begin();!it.end();it.next()){
			StackElement *si=(StackElement *)(it.value);
			Object::unrefAndFreeStackElementContainer(si);
		}
		map_fields->clear();
	}

	String DictionaryObject::toString(){
		StackElement stk={(zs_int)this,STACK_ELEMENT_PROPERTY_OBJECT};
		return json::serialize(zs,&stk,false,false);
	}

	DictionaryObject::~DictionaryObject(){

		eraseAll();
		delete map_fields;
	}
}
