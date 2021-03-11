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

		for(unsigned i=0; i < v1->stk_user_elements.count;i++){
			so_vector->push((StackElement *)v1->stk_user_elements.items[i]);
		}

		for(unsigned i=0; i < v2->stk_user_elements.count;i++){
			so_vector->push((StackElement *)v2->stk_user_elements.items[i]);
		}


		return so_vector;
	}


	void    			ScriptObjectVector::pushSf(ScriptObjectVector *sv,StackElement  * stk){
		return sv->push(stk);
	}

	StackElement *  	ScriptObjectVector::popSf(ScriptObjectVector *sv){
		return sv->pop();
	}

	zs_int 			ScriptObjectVector::sizeSf(ScriptObjectVector *sv){
		return sv->stk_user_elements.count;
	}

	void 			ScriptObjectVector::insertAtSf(ScriptObjectVector *sv, zs_int idx,StackElement  * stk){

	}

	void 			ScriptObjectVector::eraseAtSf(ScriptObjectVector *sv, zs_int idx){
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVector::clearSf(ScriptObjectVector *sv){
		sv->eraseAllUserElements();
	}
	//
	// Helpers
	//
	//----------------------------------------------


	ScriptObjectVector::ScriptObjectVector(){
		this->idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR;
	}

	zs_vector * ScriptObjectVector::getAllUserElements(){ // return list of stack elements
		return &stk_user_elements;
	}

	size_t ScriptObjectVector::length(){

		return this->stk_user_elements.count;
	}


	StackElement * ScriptObjectVector::getUserElementAt(short idx){
		if(idx >= (int)stk_user_elements.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_user_elements.items[idx];
	}

	bool ScriptObjectVector::eraseUserElementAt( short idx){//onst std::string & varname){

		StackElement *si;
		ScriptFunction * ir_fun;

		if(idx >= stk_user_elements.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx out of bounds (%i>=%i)",idx,stk_user_elements.count);
		}

		si=(StackElement *)stk_user_elements.items[idx];
		unsigned short var_type = GET_MSK_STK_PROPERTY_TYPES(si->properties);

		switch(var_type){

			case MSK_STK_PROPERTY_BOOL:
			case MSK_STK_PROPERTY_ZS_INT:
			case MSK_STK_PROPERTY_UNDEFINED:
			case MSK_STK_PROPERTY_ZS_FLOAT:
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
		free((void *)stk_user_elements.items[idx]);
		stk_user_elements.erase(idx);

		/*if(stk_user_elements.count<idx_start_user_properties){
			lenght_user_properties=0; // invalidate any user property
		}
		else{
			lenght_user_properties=stk_user_elements.count-idx_start_user_properties;
		}*/

		return true;
	}

	void ScriptObjectVector::eraseAllUserElements(){
		while ( stk_user_elements.count!=0){
			eraseUserElementAt(stk_user_elements.count-1);
		}

		stk_user_elements.clear();
	}

	StackElement *ScriptObjectVector::newUserSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=stk_undefined;
		stk_user_elements.push_back((zs_int)stk);
		return stk;
	}

	void ScriptObjectVector::push(StackElement  * _stk){

		StackElement *new_stk=newUserSlot();
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
			this->zs->getVirtualMachine()->sharePointer(((ScriptObject *)(_stk->stk_value)));
		}
	}

	StackElement *ScriptObjectVector::pop(){
		/*if(stk_user_elements.count<=idx_start_user_properties){
			VM_SET_USER_ERROR(this->virtual_machine,"pop(): error stack already empty");
			return NULL;
		}*/
		StackElement *stk_element=((StackElement *)stk_user_elements.items[stk_user_elements.count-1]);
		if(!eraseUserElementAt(stk_user_elements.count-1)){
			return NULL;
		}
		return stk_element;
	}


	ScriptObjectVector::~ScriptObjectVector(){

		eraseAllUserElements();


	}
}
