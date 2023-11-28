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
	ArrayObject * ArrayObject::newArrayObject(ZetScript *_zs){
		return new ArrayObject(_zs);
	}

	ArrayObject * ArrayObject::concat(
			ZetScript *zs
			,ArrayObject *v1
			,ArrayObject *v2
	){
		ArrayObject *so_vector = ZS_NEW_ARRAY_OBJECT(zs);

		so_vector->concat(v1);
		so_vector->concat(v2);

		return so_vector;
	}
	//
	// Helpers
	//
	//----------------------------------------------


	ArrayObject::ArrayObject(
			ZetScript *_zs
	):ContainerObject(_zs,TYPE_ID_OBJECT_ARRAY){
	}

	Vector<StackElement *> * ArrayObject::getStkListElements(){ // return list of stack elements
		return &stk_elements;
	}

	int ArrayObject::length(){

		return this->stk_elements.size();
	}

	StackElement	*ArrayObject::setStackElementByIndex(int _idx, StackElement *_stk_src){
		if(_idx >= stk_elements.size()){
			ZS_THROW_EXCEPTION("idx symbol index out of bounds (%i)",_idx);
		}

		StackElement *stk_dst=(StackElement *)stk_elements.get(_idx);

		Object::unrefStackElementContainer(stk_dst);

		zs->stackElementAssign(stk_dst,_stk_src);

		return stk_dst;
	}


	StackElement * ArrayObject::getStackElementByIndex(int _idx){
		if(_idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return stk_elements.get(_idx);
	}

	bool ArrayObject::eraseElementAt( int idx){//onst String & varname){

		StackElement *si;

		if(idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_elements.size());
		}

		si=(StackElement *)stk_elements.get(idx);

		Object::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_elements.erase(idx);

		return true;
	}

	void ArrayObject::eraseAllElements(){
		for(int i=0; i <stk_elements.size(); i++){
			Object::unrefAndFreeStackElementContainer((StackElement *)stk_elements.get(i));
		}
		stk_elements.clear();
	}

	StackElement *ArrayObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_elements.append(stk);
		return stk;
	}

	void ArrayObject::pushStackElement(const StackElement  * _stk){
		zs->stackElementAssign(newSlot(),_stk);
	}

	void 	ArrayObject::pushStackElement(StackElement  _stk){
		this->pushStackElement((const StackElement  *)&_stk);
	}

	void ArrayObject::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_elements.get(stk_elements.size()-1));

		// erase
		if(!eraseElementAt(stk_elements.size()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);
	}

	void ArrayObject::concat(ArrayObject *_v){
		for(int i=0; i < _v->stk_elements.size();i++){
			this->pushStackElement((StackElement *)_v->stk_elements.get(i));
		}
	}


	String ArrayObject::toString(){
		StackElement stk={(zs_int)this,STACK_ELEMENT_PROPERTY_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ArrayObject::~ArrayObject(){
		eraseAllElements();
	}
}
