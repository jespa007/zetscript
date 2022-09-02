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
	VectorScriptObject * VectorScriptObject::newVectorScriptObject(ZetScript *zs){
		VectorScriptObject *sv=new VectorScriptObject();
		sv->init(zs);

		return sv;
	}

	VectorScriptObject *VectorScriptObject::newShareableVectorScriptObject(ZetScript	*_zs){
		VirtualMachine *virtual_machine = _zs->getVirtualMachine();

		VectorScriptObject *so= new VectorScriptObject();
		so->init(_zs);
		// share this variable++
		vm_create_shared_script_object(virtual_machine,(ScriptObject *)so);
		vm_share_script_object(virtual_machine,(ScriptObject *)so);
		return so;
	}

	VectorScriptObject * VectorScriptObject::newVectorScriptObjectAdd(ZetScript *zs,VectorScriptObject *v1,VectorScriptObject *v2){
		VectorScriptObject *so_vector = ZS_NEW_OBJECT_VECTOR(zs);

		for(int i=0; i < v1->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v1->stk_user_elements.items[i]);
		}

		for(int i=0; i < v2->stk_user_elements.size();i++){
			so_vector->push((StackElement *)v2->stk_user_elements.items[i]);
		}


		return so_vector;
	}



	//
	// Helpers
	//
	//----------------------------------------------


	VectorScriptObject::VectorScriptObject(){
		this->idx_script_type=IDX_TYPE_SCRIPT_OBJECT_VECTOR;
	}

	zs_vector<StackElement *> * VectorScriptObject::getStkUserListElements(){ // return list of stack elements
		return &stk_user_elements;
	}

	int VectorScriptObject::length(){

		return this->stk_user_elements.size();
	}


	StackElement * VectorScriptObject::getUserElementAt(int _idx){
		if(_idx >= stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return (StackElement *)stk_user_elements.items[_idx];
	}

	bool VectorScriptObject::eraseUserElementAt( int idx){//onst zs_string & varname){

		StackElement *si;

		if(idx >= stk_user_elements.size()){
			VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_user_elements.size());
		}

		si=(StackElement *)stk_user_elements.items[idx];

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_user_elements.erase(idx);

		return true;
	}

	void VectorScriptObject::eraseAllUserElements(){
		for(int i=0; i <stk_user_elements.size(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_user_elements.items[i]);
		}
		stk_user_elements.clear();
	}

	StackElement *VectorScriptObject::pushNewUserSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_user_elements.push_back(stk);
		return stk;
	}

	void VectorScriptObject::push(StackElement  * _stk){
		stk_assign(zs,pushNewUserSlot(),_stk);
	}

	void VectorScriptObject::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_user_elements.items[stk_user_elements.size()-1]);

		// erase
		if(!eraseUserElementAt(stk_user_elements.size()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);
	}

	zs_string VectorScriptObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	VectorScriptObject::~VectorScriptObject(){
		eraseAllUserElements();
	}
}
