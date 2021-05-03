/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_CONSTRUCTOR_CLASS(sc) (*((void *(*)())sc->c_constructor))()
#define CALL_DESTRUCTOR_CLASS(sc,obj) (*((void (*)(void *))sc->c_destructor))(obj)

namespace zetscript{

	ScriptObjectClass * ScriptObjectClass::newScriptObjectClass(ZetScript *_zs, short _idx_script_class,void *_c_object){
		ScriptObjectClass *sc=new ScriptObjectClass();
		sc->init(_zs,_idx_script_class,_c_object);
		return sc;
	}

	ScriptObjectClass::ScriptObjectClass(){
		was_created_by_constructor=false;
		info_function_new=NULL;
		instruction_new=NULL;

	//	script_class = NULL;
		c_object = NULL;
		created_object = NULL;
		idx_script_class = IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS;
		delete_c_object = false; // --> user is responsible to delete C objects!
		script_class_native=NULL;
	}

	void ScriptObjectClass::callConstructorBuiltin(ScriptClass *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->idx_base_classes->count>0){
			callConstructorBuiltin(this->zs->getScriptClassFactory()->getScriptClass(sc->idx_base_classes->items[0]));
		}

		if(sc->sf_field_initializer != NULL){ // execute if only script class
			vm_execute(vm,this,sc->sf_field_initializer);
		}
	}


	void ScriptObjectClass::init(ZetScript *_zs, short _idx_script_class,void *_c_object){
		zs = _zs;
		StackElement *se;
		std::string error;
		ScriptClass *sc=NULL;
		vm=zs->getVirtualMachine();

		idx_script_class=_idx_script_class;
		ScriptClass *script_class=getScriptClass();
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for ( unsigned i = 0; i < script_class->symbol_members->count; i++){

			Symbol * symbol = (Symbol *)script_class->symbol_members->items[i];
			bool is_script_function=symbol->properties & SYMBOL_PROPERTY_FUNCTION;
			bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			if((se=addBuiltinProperty(
				symbol->name
			))==NULL){
				return;
			}

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ // function
				se->value=new StackMemberFunction(this,(ScriptFunction *)symbol->ref_ptr);
				se->properties=STK_PROPERTY_MEMBER_FUNCTION | STK_PROPERTY_FUNCTION; // tell stack element that is a function member
			}
			else{ // var...

				if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
				{
					// we know the type object so we assign the pointer ...
					void *ptr_variable=(void *)((zs_int)this->c_object + symbol->ref_ptr);
					*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
				}else if(symbol->properties & (SYMBOL_PROPERTY_CONST)){ // stack element
					se->value=(void *)symbol->ref_ptr;
					se->properties=STK_PROPERTY_PTR_STK;
				}else if(symbol->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE){
					se->value=new StackMemberAttribute(this,(MemberAttribute *)symbol->ref_ptr);
					se->properties=STK_PROPERTY_MEMBER_ATTRIBUTE;
				}
			}
		}

		//-------------------------------------------------------------------------------
		// LINK C OBJECT

		//this->script_class = irv;
		//idx_class = irv->idx_class;
		c_object = _c_object;
		script_class_native=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			if(script_class->isNativeClass()){
				script_class_native=script_class;
				created_object = CALL_CONSTRUCTOR_CLASS(script_class); // (*script_class->c_constructor)();
				was_created_by_constructor=true;
				c_object = created_object;
			}else {
				sc=script_class;
				// get first class with c inheritance...

				while((sc->idx_base_classes->count>0) && (script_class_native==NULL)){
					sc=this->zs->getScriptClassFactory()->getScriptClass(sc->idx_base_classes->items[0]); // get base class (only first in script because has single inheritance)...
					if(sc->isNativeClass()){ // we found the native script class!
						script_class_native=sc;
						if(sc->c_constructor!=NULL){ // if not null is class, else is singleton or static class already created
							created_object =  CALL_CONSTRUCTOR_CLASS(sc); //(*sc->c_constructor)();
							was_created_by_constructor=true;
							c_object = created_object;
						}
					}
				}

			}

		}

		// execute init var
		//sc=script_class;
		callConstructorBuiltin(script_class);



		// only create symbols if not std::string or std::vector type to make it fast ...
	}

	/*StackElement *ScriptObjectClass::newSlotBuiltin(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk=stk_undefined;
		stk_builtin_elements.push_back((zs_int)stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObjectClass::addPropertyBuiltin(const std::string & symbol_value, StackElement stk){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		map_builtin_property_keys->set(key_value.c_str(),stk_builtin_elements.count);

		StackElement *new_stk=newSlotBuiltin();
		*new_stk=stk;

  	    return new_stk;
	}*/


	ScriptFunction *ScriptObjectClass::getConstructorFunction(){

		ScriptClass *script_class=getScriptClass();
		if(script_class->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)script_class->symbol_members->items[script_class->idx_function_member_constructor];
		}

		return NULL;
	}

	/*bool ScriptObjectClass::itHasGetMetamethod(){
		return getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_GET),NULL) != NULL;
	}*/

	bool ScriptObjectClass::itHasSetMetamethod(){
		return getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET),NULL) != NULL;
	}

	void ScriptObjectClass::deleteNativeObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	Symbol * ScriptObjectClass::getSymbolMemberByIdx(int idx){
		return (Symbol *)getScriptClass()->symbol_members->items[idx];
	}

	void * ScriptObjectClass::getNativeObject(){
		return c_object;
	}

	bool ScriptObjectClass::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptClass * ScriptObjectClass::getNativeScriptClass(){
		 return script_class_native;
	}

	std::string ScriptObjectClass::toString(){
		// check whether toString is implemented...
		StackElement *stk_function=getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_TO_STRING),NULL);

		if(stk_function != NULL){ // get first element
			if(stk_function->properties & (STK_PROPERTY_MEMBER_FUNCTION | STK_PROPERTY_FUNCTION)){
				StackMemberFunction * smf=(StackMemberFunction *)stk_function->value;
				ScriptFunction *ptr_function=smf->so_function;
				if((ptr_function->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){


					StackElement result=VM_EXECUTE(
							this->vm
							,this
							,ptr_function
							,NULL
							,0
					);

					if(STK_IS_SCRIPT_OBJECT_STRING(&result)){
						ScriptObjectString *so=(ScriptObjectString *)result.value;
						// capture string...
						std::string aux=so->toString();
						// ... destroy lifetime object we don't need anymore
						vm_destroy_life_time_object(this->vm,so);
						// return
						return aux;
					}
				}
			}
		}

		return ScriptObjectObject::toString();
	}

	ScriptObjectClass::~ScriptObjectClass(){
		ScriptClass *script_class=getScriptClass();


		bool deallocated = false;
		if(created_object != 0 && delete_c_object){
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(script_class_native,created_object);//(*(script_class_native->c_destructor))(created_object);
			deallocated=true;

		}

		if(!deallocated && was_created_by_constructor){
			fprintf(stderr,"%s"
				,zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
						,SFI_GET_FILE_LINE(info_function_new, instruction_new)
				).c_str()
			);
		}

		for(unsigned i=0; i< stk_builtin_elements.count; i++){
			StackElement *stk=(StackElement *)stk_builtin_elements.items[i];
			if(stk->properties & STK_PROPERTY_MEMBER_ATTRIBUTE){
				delete (StackMemberAttribute *)stk->value;
			}
		}

	}


}
