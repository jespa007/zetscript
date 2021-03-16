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

	void   	ScriptObjectVector::popSf(ScriptObjectVector *sv){
		sv->pop();
	}

	zs_int 			ScriptObjectVector::sizeSf(ScriptObjectVector *sv){
		return sv->stk_user_elements.count;
	}

	void 			ScriptObjectVector::insertAtSf(ScriptObjectVector *sv, zs_int idx,StackElement  * _stk){

		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=*_stk;
		ZetScript *zs=sv->zs;

		// update n_refs +1
		if(_stk->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so_param=(ScriptObject *)_stk->stk_value;
			if(so_param->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING && so_param->shared_pointer==NULL){
				//STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
				ScriptObjectString *sc=ZS_NEW_OBJECT_STRING(zs);
				if(!zs->getVirtualMachine()->createSharedPointer(sc)){
					return;
				}
				sc->set(so_param->toString());
				so_param=sc;
				new_stk->stk_value=so_param; // update pointer
			}

			zs->getVirtualMachine()->sharePointer(so_param);

		}

		sv->stk_user_elements.insert(idx,(zs_int)new_stk);
	}

	void 			ScriptObjectVector::eraseAtSf(ScriptObjectVector *sv, zs_int idx){
		sv->eraseUserElementAt(idx);
	}

	void 			ScriptObjectVector::clearSf(ScriptObjectVector *sv){
		sv->eraseAllUserElements();
	}

	ScriptObjectString *		ScriptObjectVector::joinSf(ScriptObjectVector *sv, zs_int idx){
		ScriptObjectString *so_ref=NULL;
		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(sv->zs);
		std::string *ptr_str=(std::string *)so_string->value;

		for(unsigned i=0; i < sv->stk_user_elements.count;i++){
			StackElement *stk=(StackElement *)sv->stk_user_elements.items[i];
			if(i>0){
				ptr_str->push_back(idx);
			}
			*ptr_str+=stk->toString();
//			so_vector->push((StackElement *)v1->stk_user_elements.items[i]);

		}

		return so_string;
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


		if(idx >= stk_user_elements.count){
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"idx out of bounds (%i>=%i)",idx,stk_user_elements.count);
		}

		si=(StackElement *)stk_user_elements.items[idx];

		ScriptObject::unrefAndFreeStackElementContainer(si);

		// remove symbol on std::vector ...
		//free((void *)stk_user_elements.items[idx]);
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
		//while ( stk_user_elements.count!=0){
		for(int i=0; i <stk_user_elements.count; i++){
			ScriptObject::unrefAndFreeStackElementContainer((StackElement *)stk_user_elements.items[i]);
		}

		stk_user_elements.clear();
	}

	StackElement *ScriptObjectVector::pushNewUserSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=stk_undefined;
		stk_user_elements.push_back((zs_int)stk);
		return stk;
	}

	void ScriptObjectVector::push(StackElement  * _stk){

		StackElement *new_stk=pushNewUserSlot();
		*new_stk=*_stk;

		// update n_refs +1
		if(_stk->properties&MSK_STK_PROPERTY_SCRIPT_OBJECT){
			this->zs->getVirtualMachine()->sharePointer(((ScriptObject *)(_stk->stk_value)));
		}
	}

	void ScriptObjectVector::pop(){
		// save last element...
		StackElement stk_element=*((StackElement *)stk_user_elements.items[stk_user_elements.count-1]);

		// erase
		if(!eraseUserElementAt(stk_user_elements.count-1)){
			return;
		}

		// push current vm
		this->zs->getVirtualMachine()->pushStackElement(stk_element);

	}


	ScriptObjectVector::~ScriptObjectVector(){

		eraseAllUserElements();


	}
}
