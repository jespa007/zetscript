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
		std::string error="";
		ScriptObjectObject *obj = ZS_NEW_OBJECT_OBJECT(zs);
		//zs_map_iterator it_1=o1->map_user_property_keys->begin();
		//zs_map_iterator it_2=o2->map_user_property_keys->begin();

		for(auto it=o1->begin(); it!=o1->end();it++){
			obj->addUserProperty(it->first,error,it->second);
		}

		for(auto it=o2->begin(); it!=o2->end();it++){
			obj->addUserProperty(it->first,error,it->second);
		}
		return obj;
	}

	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_property_keys=new std::map<std::string,StackElement *>();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const std::string & symbol_value
			,std::string & error
			,StackElement * sv
		){
		StackElement si;
		bool error_symbol=false;

		if(map_user_property_keys->count(symbol_value.c_str())>=1){
			error=zs_strutils::format("\"%s\" symbol already exists",symbol_value.c_str());
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

			si=k_stk_undefined;
		}

		/*if(idx_stk_element != NULL){
			*idx_stk_element=stk_user_elements.count;
		}*/

		std::string key_value = symbol_value;
		//StackElement *new_stk=pushNewUserSlot();
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		(*map_user_property_keys)[key_value.c_str()]=new_stk;

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const std::string & property_name){

		return this->map_user_property_keys->at(property_name.c_str());
	}


	StackElement * ScriptObjectObject::addProperty(
			const std::string & symbol_value
			,std::string & error
			,StackElement * stk_element
	){
		return addUserProperty(symbol_value,error);
	}

	StackElement 	* ScriptObjectObject::getProperty(const std::string & property_name){
		StackElement *stk=getBuiltinProperty(property_name);
		if(stk==NULL){
			stk=getUserProperty(property_name/*,idx*/);
		}

		return stk;

	}

	bool ScriptObjectObject::existUserProperty(const std::string & property_name){
		return map_user_property_keys->count(property_name.c_str()) > 1;
	}

	std::map<std::string,StackElement *>::iterator ScriptObjectObject::begin(){
		return this->map_user_property_keys->begin();
	}

	std::map<std::string,StackElement *>::iterator ScriptObjectObject::begin_builtin(){
		return this->map_builtin_property_keys->begin();
	}

	std::map<std::string,StackElement *>::iterator ScriptObjectObject::end(){
		return this->map_user_property_keys->end();
	}

	std::map<std::string,StackElement *>::iterator ScriptObjectObject::end_builtin(){
		return this->map_builtin_property_keys->end();
	}



	bool ScriptObjectObject::eraseUserProperty(const std::string & property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_property_keys->at(property_name.c_str());
		if(stk_user_element == NULL){
			VM_SET_USER_ERROR(vm,"Property %s not exist",property_name.c_str());
			return false;
		}


		free(stk_user_element);

		map_user_property_keys->erase(property_name.c_str()); // erase also property key

		return true;
	}

	void ScriptObjectObject::eraseAllUserProperties(){
		for(auto it=map_user_property_keys->begin(); it!=map_user_property_keys->end(); it++){
			StackElement *si=it->second;
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_property_keys->clear();

	}

	std::string ScriptObjectObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ScriptObjectObject::~ScriptObjectObject(){
		eraseAllUserProperties();
		delete map_user_property_keys;

	}
}
