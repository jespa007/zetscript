/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void    			ScriptObjectVector::pushSf(ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	StackElement *  	ScriptObjectVector::popSf(ScriptObjectVector *sv){
		return sv->pop();
	}

	ScriptObjectVector * ScriptObjectVector::newVectorObject(ZetScript *zs){
		ScriptObjectVector *sv=new ScriptObjectVector();
		sv->setZetScript(zs);
		return sv;
	}

	ScriptObjectVector::ScriptObjectVector(){
		this->idx_script_class=IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR;
		stk_count.stk_value=0;
		stk_count.properties=MSK_STK_PROPERTY_ZS_INT;
		//this->init(SCRIPT_CLASS_VECTOR(this), (void *)this);
	}

	StackElement *ScriptObjectVector::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=stk_undefined;
		stk_elements.push_back((zs_int)stk);
		return stk;
	}

	zs_vector * ScriptObjectVector::getAllElements(){ // return list of stack elements
		return &stk_elements;
	}

	zs_int ScriptObjectVector::count(){

		return this->stk_elements.count;
	}

	StackElement * ScriptObjectVector::getElementAt(short idx){



		if(idx >= (int)stk_elements.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_elements.items[idx];
	}

	bool ScriptObjectVector::eraseElementAt( short idx){//onst std::string & varname){

		StackElement *si;
		ScriptFunction * ir_fun;

		if(idx >= stk_elements.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx out of bounds (%i>=%i)",idx,stk_elements.count);
			return false;
		}

		si=(StackElement *)stk_elements.items[idx];
		unsigned short var_type = GET_MSK_STK_PROPERTY_TYPES(si->properties);

		switch(var_type){

			case MSK_STK_PROPERTY_BOOL:
			case MSK_STK_PROPERTY_ZS_INT:
			case MSK_STK_PROPERTY_UNDEFINED:
			case MSK_STK_PROPERTY_FLOAT:
				break;
			case MSK_STK_PROPERTY_FUNCTION:
				 ir_fun  = (ScriptFunction *)(si->stk_value);
				break;
			default: // properties ...

				if(var_type & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					if(((si->properties & MSK_STK_PROPERTY_IS_VAR_C) != MSK_STK_PROPERTY_IS_VAR_C)
						&& (si->stk_value != this) // ensure that property don't holds its same var.
						&& (si->stk_value != 0)
					  ){ // deallocate but not if is c or this ref

						// remove property if not referenced anymore
						if(!this->zs->getVirtualMachine()->unrefSharedScriptObjectAndRemoveIfZero((ScriptObject **)&si->stk_value)){
							return false;
						}

					}
				}
				break;
		}

		// remove symbol on std::vector ...
		free((void *)stk_elements.items[idx]);
		stk_elements.erase(idx);

		/*if(stk_elements.count<idx_start_user_properties){
			lenght_user_properties=0; // invalidate any user property
		}
		else{
			lenght_user_properties=stk_elements.count-idx_start_user_properties;
		}*/

		return true;
	}

	void ScriptObjectVector::push(StackElement  * _stk){

		StackElement *new_stk=newSlot();
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
			this->zs->getVirtualMachine()->sharePointer(((ScriptObject *)(_stk->stk_value))->shared_pointer);
		}
	}

	StackElement *ScriptObjectVector::pop(){
		/*if(stk_elements.count<=idx_start_user_properties){
			VM_SET_USER_ERROR(this->virtual_machine,"pop(): error stack already empty");
			return NULL;
		}*/

		StackElement *stk_element=((StackElement *)stk_elements.items[stk_elements.count-1]);
		if(!eraseElementAt(stk_elements.count-1)){
			return NULL;
		}
		return stk_element;
	}

	ScriptObjectVector::~ScriptObjectVector(){

		while ( stk_elements.count!=0){
			eraseElementAt(stk_elements.count-1);
		}

		stk_elements.clear();


	}
}
