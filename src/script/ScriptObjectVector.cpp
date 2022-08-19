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

	ScriptObjectVector *ScriptObjectVector::newShareableScriptObjectVector(ZetScript	*_zs){
		VirtualMachine *virtual_machine = _zs->getVirtualMachine();

		ScriptObjectVector *so= new ScriptObjectVector();
		so->init(_zs);
		// share this variable++
		vm_create_shared_script_object(virtual_machine,(ScriptObject *)so);
		vm_share_script_object(virtual_machine,(ScriptObject *)so);
		return so;
	}

	ScriptObjectVector * ScriptObjectVector::newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2){
		ScriptObjectVector *so_vector = ZS_NEW_OBJECT_VECTOR(zs);

		for(unsigned i=0; i < v1->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v1->stk_user_elements[i]);
		}

		for(unsigned i=0; i < v2->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v2->stk_user_elements[i]);
		}


		return so_vector;
	}



	//
	// Helpers
	//
	//----------------------------------------------


	ScriptObjectVector::ScriptObjectVector(){
		this->idx_script_type=IDX_TYPE_SCRIPT_OBJECT_VECTOR;
	}

	std::vector<StackElement *> * ScriptObjectVector::getStkUserListElements(){ // return list of stack elements
		return &stk_user_elements;
	}

	size_t ScriptObjectVector::length(){

		return this->stk_user_elements.size();
	}


	StackElement * ScriptObjectVector::getUserElementAt(size_t _idx){
		if(_idx >= stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return (StackElement *)stk_user_elements[_idx];
	}

	bool ScriptObjectVector::eraseUserElementAt( size_t idx){//onst std::string & varname){

		StackElement *si;

		if(idx >= stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_user_elements.size());
		}

		si=(StackElement *)stk_user_elements[idx];

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_user_elements.erase(stk_user_elements.begin()+idx);

		return true;
	}

	void ScriptObjectVector::eraseAllUserElements(){
		for(unsigned i=0; i <stk_user_elements.size(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_user_elements[i]);
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
		stk_assign(zs,pushNewUserSlot(),_stk);
	}

	void ScriptObjectVector::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_user_elements[stk_user_elements.size()-1]);

		// erase
		if(!eraseUserElementAt((int)stk_user_elements.size()-1)){
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
