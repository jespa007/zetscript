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
		std::string error;
		ScriptObjectObject *obj = ZS_NEW_OBJECT_OBJECT(zs);

		// get properties from object o1
		auto map_properties=o1->getMapUserProperties();
		for(auto it=map_properties->begin(); it!=map_properties->end();it++){
			obj->addUserProperty(it->first,error,(StackElement *)it->second);
		}

		// get properties from object o2
		map_properties=o2->getMapUserProperties();
		for(auto it=map_properties->begin(); it!=map_properties->end();it++){
			obj->addUserProperty(it->first,error,(StackElement *)it->second);
		}
		return obj;
	}

	void  ScriptObjectObject::append(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2){
		ZS_UNUSUED_PARAM(zs);
		std::string error;

		// get properties from object o2
		auto map_properties=o2->getMapUserProperties();
		for(auto it=map_properties->begin(); it!=map_properties->end();it++){
			o1->addUserProperty(it->first,error,it->second);
		}
	}
	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_properties=new std::map<std::string,StackElement *>();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const std::string & symbol_value
			,std::string & error
			,StackElement * sv
		){
		StackElement si;

		if(map_user_properties->count(symbol_value)!=0){
			error=zs_strutils::format("'%s' symbol already exists",symbol_value.c_str());
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

		std::string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		map_user_properties->at(key_value)=new_stk;

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const std::string & property_name){

		if(this->map_user_properties->count(property_name)!=0){
			return this->map_user_properties->at(property_name);
		}
		return NULL;
	}

	StackElement * ScriptObjectObject::addProperty(
			const char * symbol_value
			,std::string & _error
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
		return map_user_properties->count(property_name)!=0;
	}

	std::map<std::string,StackElement *> * ScriptObjectObject::getMapUserProperties(){
		return map_user_properties;
	}

	std::map<std::string,StackElement *> * ScriptObjectObject::getMapBuiltinProperties(){
		return map_builtin_properties;
	}

	size_t	ScriptObjectObject::length(){
		return this->map_user_properties->size();
	}

	bool ScriptObjectObject::eraseUserProperty(const char * property_name/*, const ScriptFunction *info_function*/){

		if(map_user_properties->count(property_name)==0){
			VM_SET_USER_ERROR(vm,"Property %s not exist",property_name);
			return false;
		}

		StackElement *stk_user_element = (StackElement *)map_user_properties->at(property_name);
		free(stk_user_element);
		map_user_properties->erase(property_name); // erase also property key
		return true;
	}

	void ScriptObjectObject::eraseAllUserProperties(){
		for(auto it=map_user_properties->begin();it!=map_user_properties->end();it++){
			ScriptObject::unrefAndFreeStackElementContainer(it->second);
		}
		map_user_properties->clear();
	}

	std::string ScriptObjectObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true,false);
	}

	ScriptObjectObject::~ScriptObjectObject(){

		eraseAllUserProperties();
		delete map_user_properties;
	}
}
