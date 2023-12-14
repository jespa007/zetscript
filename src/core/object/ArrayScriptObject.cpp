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
	ArrayScriptObject * ArrayScriptObject::newArrayScriptObject(ScriptEngine *_script_engine){
		return new ArrayScriptObject(_script_engine);
	}

	ArrayScriptObject * ArrayScriptObject::concat(
			ScriptEngine *_script_engine
			,ArrayScriptObject *v1
			,ArrayScriptObject *v2
	){
		ArrayScriptObject *so_vector = ZS_NEW_ARRAY_SCRIPT_OBJECT(_script_engine);

		so_vector->concat(v1);
		so_vector->concat(v2);

		return so_vector;
	}
	//
	// Helpers
	//
	//----------------------------------------------


	ArrayScriptObject::ArrayScriptObject(
			ScriptEngine *_script_engine
	):ContainerScriptObject(_script_engine,SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT){
	}

	Vector<StackElement *> * ArrayScriptObject::getStkListElements(){ // return list of stack elements
		return &stk_elements;
	}

	int ArrayScriptObject::length(){

		return this->stk_elements.length();
	}

	StackElement	*ArrayScriptObject::setStackElementByIndex(int _idx, StackElement *_stk_src){
		if(_idx >= stk_elements.length()){
			ZS_THROW_EXCEPTION("idx symbol index out of bounds (%i)",_idx);
		}

		StackElement *stk_dst=(StackElement *)stk_elements.get(_idx);

		ScriptObject::unrefStackElementContainer(stk_dst);

		script_engine->stackElementAssign(stk_dst,_stk_src);

		return stk_dst;
	}


	StackElement * ArrayScriptObject::getStackElementByIndex(int _idx){
		if(_idx >= stk_elements.length()){
			ZS_VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",_idx);
			return NULL;
		}

		return stk_elements.get(_idx);
	}

	bool ArrayScriptObject::eraseElementAt( int idx){//onst String & varname){

		StackElement *si;

		if(idx >= stk_elements.length()){
			ZS_VM_SET_USER_ERROR(vm,"idx out of bounds (%i>=%i)",idx,stk_elements.length());
		}

		si=(StackElement *)stk_elements.get(idx);

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on vector ...
		stk_elements.erase(idx);

		return true;
	}

	void ArrayScriptObject::eraseAllElements(){
		for(int i=0; i <stk_elements.length(); i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_elements.get(i));
		}
		stk_elements.clear();
	}

	StackElement *ArrayScriptObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=k_stk_undefined;
		stk_elements.push(stk);
		return stk;
	}

	void ArrayScriptObject::pushStackElement(const StackElement  * _stk){
		script_engine->stackElementAssign(newSlot(),_stk);
	}

	void 	ArrayScriptObject::pushStackElement(StackElement  _stk){
		this->pushStackElement((const StackElement  *)&_stk);
	}

	void ArrayScriptObject::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_elements.get(stk_elements.length()-1));

		// erase
		if(!eraseElementAt(stk_elements.length()-1)){
			return;
		}

		// push current vm
		vm_push_stack_element(vm,stk_element);
	}

	void ArrayScriptObject::concat(ArrayScriptObject *_v){
		for(int i=0; i < _v->stk_elements.length();i++){
			this->pushStackElement((StackElement *)_v->stk_elements.get(i));
		}
	}


	String ArrayScriptObject::toString(){
		StackElement stk={(zs_int)this,STACK_ELEMENT_PROPERTY_OBJECT};
		return json::serialize(script_engine,&stk,true);
	}

	ArrayScriptObject::~ArrayScriptObject(){
		eraseAllElements();
	}
}
