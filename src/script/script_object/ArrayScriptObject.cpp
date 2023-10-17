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
	ArrayScriptObject * ArrayScriptObject::newArrayScriptObject(ZetScript *_zs){
		return new ArrayScriptObject(_zs);
	}

	ArrayScriptObject * ArrayScriptObject::concat(
			ZetScript *zs
			,ArrayScriptObject *v1
			,ArrayScriptObject *v2
	){
		ArrayScriptObject *so_vector = ZS_NEW_ARRAY_OBJECT(zs);

		so_vector->concat(v1);
		so_vector->concat(v2);

		return so_vector;
	}
	//
	// Helpers
	//
	//----------------------------------------------


	ArrayScriptObject::ArrayScriptObject(
			ZetScript *_zs
	):ContainerScriptObject(_zs,IDX_TYPE_SCRIPT_OBJECT_ARRAY){
	}

	zs_vector<StackElement *> * ArrayScriptObject::getStkListElements(){ // return list of stack elements
		return &stk_elements;
	}

	int ArrayScriptObject::length(){

		return this->stk_elements.size();
	}

	StackElement	*ArrayScriptObject::setStackElementByIndex(int _idx, StackElement *_stk_src){
		if(_idx >= stk_elements.size()){
			ZS_THROW_EXCEPTION("idx symbol index out of bounds (%i)",_idx);
		}

		StackElement *stk_dst=(StackElement *)stk_elements.get(_idx);

		ScriptObject::unrefStackElementContainer(stk_dst);

		zs->stackElementAssign(stk_dst,_stk_src);

		return stk_dst;
	}


	StackElement * ArrayScriptObject::getStackElementByIndex(int _idx){
		if(_idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return stk_elements.get(_idx);
	}

	bool ArrayScriptObject::eraseElementAt( int idx){//onst zs_string & varname){

		StackElement *si;

		if(idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_elements.size());
		}

		si=(StackElement *)stk_elements.get(idx);

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_elements.erase(idx);

		return true;
	}

	void ArrayScriptObject::eraseAllElements(){
		for(int i=0; i <stk_elements.size(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_elements.get(i));
		}
		stk_elements.clear();
	}

	StackElement *ArrayScriptObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_elements.push_back(stk);
		return stk;
	}

	void ArrayScriptObject::pushStackElement(const StackElement  * _stk){
		zs->stackElementAssign(newSlot(),_stk);
	}

	void 	ArrayScriptObject::pushStackElement(StackElement  _stk){
		this->pushStackElement((const StackElement  *)&_stk);
	}

	void ArrayScriptObject::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_elements.get(stk_elements.size()-1));

		// erase
		if(!eraseElementAt(stk_elements.size()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);
	}

	void ArrayScriptObject::concat(ArrayScriptObject *_v){
		for(int i=0; i < _v->stk_elements.size();i++){
			this->pushStackElement((StackElement *)_v->stk_elements.get(i));
		}
	}


	zs_string ArrayScriptObject::toString(){
		StackElement stk={(zs_int)this,ZS_STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ArrayScriptObject::~ArrayScriptObject(){
		eraseAllElements();
	}
}
