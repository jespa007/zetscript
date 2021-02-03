/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectAnonymous * ScriptObjectAnonymous::newAnonymousObject(ZetScript	*_zs){
		ScriptObjectAnonymous *ac= new ScriptObjectAnonymous();
		ac->setZetScript(_zs);
		return ac;
	}


	ScriptObjectAnonymous::ScriptObjectAnonymous(){
		idx_script_class=IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_ANONYMOUS;
		map_property_keys=new zs_map();
	}


	StackElement * ScriptObjectAnonymous::addProperty(
			const std::string & symbol_value
			,std::string & error
			//, const ScriptFunction *info_function
			//,Instruction *src_instruction
			,StackElement * sv
			,int * idx_stk_element

		){
		StackElement si;
		bool error_symbol=false;

		char *aux_p=(char *)symbol_value.c_str();
		if(
			   ('a' <= *aux_p && *aux_p <='z') ||
			   ('A' <= *aux_p && *aux_p <='Z')


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

		if(map_property_keys->exist(symbol_value.c_str())){
			error=zs_strutils::format("\"%s\" symbol already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
				if(zs->getVirtualMachine()->sharePointer(((ScriptObjectAnonymous *)(sv->stk_value))->shared_pointer) == false){
					return NULL;
				}
			}

		}else{

			si=stk_undefined;
		}

		if(idx_stk_element != NULL){
			*idx_stk_element=stk_elements.count;
		}

		std::string key_value = symbol_value;
		map_property_keys->set(key_value.c_str(),stk_elements.count);
		StackElement *new_stk=newSlot();
		*new_stk=si; //assign var
		return new_stk;
	}

	int  ScriptObjectAnonymous::getPropertyIdx(const std::string & property_name){//,bool only_var_name){

		bool exists;
		int idx_stk_element=this->map_property_keys->get(property_name.c_str(),exists);
		if(exists){
			return idx_stk_element;
		}
		return ZS_IDX_UNDEFINED;
	}

	StackElement *ScriptObjectAnonymous::getThisProperty(){
		return &this->stk_this;
	}

	StackElement * ScriptObjectAnonymous::getProperty(const std::string & property_name, int * idx){//,bool only_var_name){

		// special properties
		/*if(property_name == "length"){
			stk_count.stk_value=(void *)this->countUserProperties();
			return &stk_count;
		}*/

		bool exists;
		zs_int idx_stk_element=this->map_property_keys->get(property_name.c_str(),exists);
		if(exists){
			if(idx!=NULL){
				*idx=idx_stk_element;
			}

			return (StackElement *)stk_elements.items[idx_stk_element];
		}
		return NULL;
	}

	bool ScriptObjectAnonymous::eraseProperty(const std::string & property_name, const ScriptFunction *info_function){
		bool exists=false;
		zs_int idx_property = map_property_keys->get(property_name.c_str(),exists);
		if(!exists){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"Property %s not exist",property_name.c_str());
			return false;
		}

		if(!eraseElementAt(idx_property)){
			return false;
		}
		map_property_keys->erase(property_name.c_str()); // erase also property key

		return true;
	}

	ScriptObjectAnonymous::~ScriptObjectAnonymous(){
		delete map_property_keys;
	}
}
