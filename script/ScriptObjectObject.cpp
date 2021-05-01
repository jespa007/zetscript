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

		for(auto it=o1->begin(); !it.end();it.next()){
			obj->addUserProperty(it.getKey(),error,(StackElement *)it.getValue());
		}

		for(auto it=o2->begin(); !it.end();it.next()){
			obj->addUserProperty(it.getKey(),error,(StackElement *)it.getValue());
		}
		return obj;
	}

	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectObject::ScriptObjectObject(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT;
		map_user_property_keys=new zs_map();
	}

	StackElement * ScriptObjectObject::addUserProperty(
			const std::string & symbol_value
			,std::string & error
			//, const ScriptFunction *info_function
			//,Instruction *src_instruction
			,StackElement * sv
			//,int * idx_stk_element

		){
		StackElement si;
		bool error_symbol=false;

		char *aux_p=(char *)symbol_value.c_str();
		if(
			   ('a' <= *aux_p && *aux_p <='z') ||
			   ('A' <= *aux_p && *aux_p <='Z') ||
			   (*aux_p=='_')


			){ // let's see it has right chars...
				aux_p++;
				while((*aux_p!=0) && (
					  ('a' <= *aux_p && *aux_p <='z') ||
					  ('0' <= *aux_p && *aux_p <='9') ||
					  (*aux_p=='_') ||
					  ('A' <= *aux_p && *aux_p <='Z'))){
					aux_p++;
				}

				if((*aux_p!=0)){
					error_symbol=true;
				}
		}else{
			error_symbol=true;
		}

		if(error_symbol){
			error=zs_strutils::format("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			return NULL;
		}

		if(map_user_property_keys->exist(symbol_value.c_str())){
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

			si=stk_undefined;
		}

		/*if(idx_stk_element != NULL){
			*idx_stk_element=stk_user_elements.count;
		}*/

		std::string key_value = symbol_value;
		//StackElement *new_stk=pushNewUserSlot();
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		map_user_property_keys->set(key_value.c_str(),(zs_int)new_stk);

		*new_stk=si; //assign var
		return new_stk;
	}

	StackElement * ScriptObjectObject::getUserProperty(const std::string & property_name/*, int * idx*/){//,bool only_var_name){

		// special properties
		/*if(property_name == "length"){
			stk_count.value=(void *)this->countUserProperties();
			return &stk_count;
		}*/

		bool exists;
		StackElement *stk_element=(StackElement *)this->map_user_property_keys->get(property_name.c_str(),exists);
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
			const std::string & symbol_value
			//, const ScriptFunction *info_function
			//, Instruction *src_instruction
			,std::string & error
			,StackElement * stk_element
			/*,int * idx_stk_element*/

	){
		return addUserProperty(symbol_value,error/*,stk_element,idx_stk_element*/);
	}

	StackElement 	* ScriptObjectObject::getProperty(const std::string & property_name, int * idx){
		StackElement *stk=getBuiltinProperty(property_name, idx);
		if(stk==NULL){
			stk=getUserProperty(property_name/*,idx*/);
		}

		return stk;

	}

	bool ScriptObjectObject::existUserProperty(const std::string & property_name){
		return map_user_property_keys->exist(property_name.c_str());
	}

	zs_map_iterator ScriptObjectObject::begin(){
		return this->map_user_property_keys->begin();
	}


	bool ScriptObjectObject::eraseUserProperty(const std::string & property_name/*, const ScriptFunction *info_function*/){
		bool exists=false;
		StackElement *stk_user_element = (StackElement *)map_user_property_keys->get(property_name.c_str(),exists);
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
			StackElement *si=(StackElement *)it.getValue();
			ScriptObject::unrefAndFreeStackElementContainer(si);
		}
		map_user_property_keys->clear();

	}

	ScriptObjectObject::~ScriptObjectObject(){
		eraseAllUserProperties();
		delete map_user_property_keys;

	}
}
