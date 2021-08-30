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
	ScriptObjectVector * ScriptObjectVector::newScriptObjectVector(ZetScript *zs){
		ScriptObjectVector *sv=new ScriptObjectVector();
		sv->init(zs);

		return sv;
	}

	ScriptObjectVector * ScriptObjectVector::newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2){
		ScriptObjectVector *so_vector = ZS_NEW_OBJECT_VECTOR(zs);

		for(unsigned i=0; i < v1->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v1->stk_user_elements.at(i));
		}

		for(unsigned i=0; i < v2->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v2->stk_user_elements.at(i));
		}


		return so_vector;
	}



	//
	// Helpers
	//
	//----------------------------------------------


	ScriptObjectVector::ScriptObjectVector(){
		this->idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR;
	}

	std::vector<StackElement *> * ScriptObjectVector::getStkUserListElements(){ // return list of stack elements
		return &stk_user_elements;
	}

	size_t ScriptObjectVector::length(){
		return this->stk_user_elements.size();
	}

	StackElement * ScriptObjectVector::getUserElementAt(int idx){
		if(idx >= (int)stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_user_elements.at(idx);
	}

	bool ScriptObjectVector::eraseUserElementAt( int idx){//onst std::string & varname){

		StackElement *si;

		if(idx >= (int)stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_user_elements.size());
		}

		si=(StackElement *)stk_user_elements.at(idx);

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on std::vector ...
		//free((void *)stk_user_elements.items[idx]);
		stk_user_elements.erase(stk_user_elements.begin()+idx);

		/*if(stk_user_elements.count<idx_start_user_properties){
			lenght_user_properties=0; // invalidate any user property
		}
		else{
			lenght_user_properties=stk_user_elements.count-idx_start_user_properties;
		}*/

		return true;
	}

	void ScriptObjectVector::eraseAllUserElements(){
		//while ( stk_user_elements.count!=0){
		for(unsigned i=0; i <stk_user_elements.size(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_user_elements.at(i));
		}

		stk_user_elements.clear();
	}

	StackElement *ScriptObjectVector::pushNewUserSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_user_elements.push_back(stk);
		return stk;
	}

	void ScriptObjectVector::push(StackElement  * _stk){

		StackElement *new_stk=pushNewUserSlot();
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&STK_PROPERTY_SCRIPT_OBJECT){
			vm_share_pointer(vm,(ScriptObject *)(_stk->value));
		}
	}

	void ScriptObjectVector::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_user_elements.at(stk_user_elements.size()-1));

		// erase
		if(!eraseUserElementAt(stk_user_elements.size()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);

	}

	std::string ScriptObjectVector::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}


	ScriptObjectVector::~ScriptObjectVector(){
		eraseAllUserElements();
	}
}
