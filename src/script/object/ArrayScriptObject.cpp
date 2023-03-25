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


	StackElement * ArrayScriptObject::get(int _idx){
		if(_idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return (StackElement *)stk_elements.items[_idx];
	}

	bool ArrayScriptObject::eraseElementAt( int idx){//onst zs_string & varname){

		StackElement *si;

		if(idx >= stk_elements.size()){
			ZS_VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_elements.size());
		}

		si=(StackElement *)stk_elements.items[idx];

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_elements.erase(idx);

		return true;
	}

	void ArrayScriptObject::eraseAllElements(){
		for(int i=0; i <stk_elements.size(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_elements.items[i]);
		}
		stk_elements.clear();
	}

	StackElement *ArrayScriptObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_elements.push_back(stk);
		return stk;
	}

	void ArrayScriptObject::push(const StackElement  * _stk){
		zs->stackElementAssign(newSlot(),_stk);
	}

	void 	ArrayScriptObject::push(StackElement  _stk){
		this->push((const StackElement  *)&_stk);
	}

	void 	ArrayScriptObject::pushInteger(zs_int _value){
		StackElement stk={_value,STK_PROPERTY_ZS_INT};
		zs->stackElementAssign(newSlot(),&stk);
	}

	void 	ArrayScriptObject::pushFloat(zs_float _value){
		zs_int dst;
		StackElement stk;
		ZS_WRITE_INTPTR_FLOAT(&dst,_value);
		stk={dst,STK_PROPERTY_ZS_FLOAT};
		zs->stackElementAssign(newSlot(),&stk);
	}

	void 	ArrayScriptObject::pushBoolean(bool _value){
		StackElement stk={_value,STK_PROPERTY_BOOL};
		zs->stackElementAssign(newSlot(),&stk);
	}

	void	ArrayScriptObject::pushString(const zs_string & _value){
		StringScriptObject *so=this->zs->newStringScriptObject();
		StackElement stk={(zs_int)so,STK_PROPERTY_SCRIPT_OBJECT};
		so->set(_value);
		zs->stackElementAssign(newSlot(),&stk);
	}

	void ArrayScriptObject::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_elements.items[stk_elements.size()-1]);

		// erase
		if(!eraseElementAt(stk_elements.size()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);
	}

	void ArrayScriptObject::concat(ArrayScriptObject *_v){
		for(int i=0; i < _v->stk_elements.size();i++){
			this->push((StackElement *)_v->stk_elements.items[i]);
		}
	}


	zs_string ArrayScriptObject::toString(){
		StackElement stk={(zs_int)this,STK_PROPERTY_SCRIPT_OBJECT};
		return json::serialize(zs,&stk,true);
	}

	ArrayScriptObject::~ArrayScriptObject(){
		eraseAllElements();
	}
}
