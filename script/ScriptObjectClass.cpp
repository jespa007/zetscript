/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_CONSTRUCTOR_CLASS(sc) ((sc->static_constructor_destructor)?(*((void *(*)())sc->c_constructor))():(*((std::function<void *()> *)(sc->c_constructor)))())
#define CALL_DESTRUCTOR_CLASS(sc,obj) ((sc->static_constructor_destructor)?(*((void (*)(void *))sc->c_destructor))(obj):(*((std::function<void (void *)> *)(sc->c_destructor)))(obj))

namespace zetscript{

	ScriptObjectClass * ScriptObjectClass::newScriptObjectClass(ZetScript *zs){
		ScriptObjectClass *sc=new ScriptObjectClass();
		sc->setZetScript(zs);
		return sc;
	}

	ScriptObjectClass::ScriptObjectClass(){
		was_created_by_constructor=false;
		info_function_new=NULL;
		instruction_new=NULL;

	//	script_class = NULL;
		c_object = NULL;
		created_object = NULL;
		idx_script_class = IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_CLASS;
		delete_c_object = false; // --> user is responsible to delete C objects!
		memset(&stk_this,0,sizeof(stk_this));
		script_class_native=NULL;
	}

	void ScriptObjectClass::callConstructorBuiltin(ScriptClass *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->idx_base_classes->count>0){
			callConstructorBuiltin(this->zs->getScriptClassFactory()->getScriptClass(sc->idx_base_classes->items[0]));
		}

		zs->getVirtualMachine()->execute(this,sc->sf_field_initializer);
	}


	void ScriptObjectClass::init(short _idx_script_class,void *_c_object){
		StackElement *se;
		std::string error;
		ScriptClass *sc=NULL;

		idx_script_class=_idx_script_class;
		ScriptClass *script_class=getScriptClass();
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for ( unsigned i = 0; i < script_class->symbol_members->count; i++){

			Symbol * symbol = (Symbol *)script_class->symbol_members->items[i];
			bool is_script_function=symbol->properties & SYMBOL_PROPERTY_FUNCTION;
			bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			if((se=addPropertyBuiltIn(
				symbol->name
			))==NULL){
				return;
			}

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ // function

				se->stk_value=new FunctionMember(this,(ScriptFunction *)symbol->ref_ptr);
				se->properties=MSK_STK_PROPERTY_FUNCTION_MEMBER | MSK_STK_PROPERTY_FUNCTION; // tell stack element that is a function member
			}
			else{ // var...

				if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
				{
					// we know the type object so we assign the pointer ...
					void *ptr_variable=(void *)((zs_int)this->c_object + symbol->ref_ptr);
					*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
				}else if(symbol->properties & (SYMBOL_PROPERTY_CONST)){ // stack element
					se->stk_value=(void *)symbol->ref_ptr;
					se->properties=MSK_STK_PROPERTY_PTR_STK;
				}/*else{ // fixed var
					StackElement *stk_new=(StackElement *)malloc(sizeof(StackElement));
					se->stk_value=stk_new;
					stk_new->setUndefined();
					se->properties=MSK_STK_PROPERTY_PTR_STK;

					//VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"internal error: symbol should be const or native var");
					//return;
				}*/
			}
		}

		// init built in vars



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

		}/*else{ // pass the pointer reference but in principle is cannot be deleted when the scriptvar is deleted...
			idx_script_class_native=irv->idx_class;
		}*/

		// execute init var
		//sc=script_class;
		callConstructorBuiltin(script_class);



		// only create symbols if not std::string or std::vector type to make it fast ...
	}

	/*void ScriptObjectClass::setup(){
		//FunctionSymbol *si;



		// register custom built-in vars
		if((se=addPropertyBuiltIn(
			"length"
		))==NULL){
			return;
		}

		se->var_ref=&lenght_user_properties;
		se->properties=(MSK_STK_PROPERTY_ZS_INT|MSK_STK_PROPERTY_IS_VAR_C|MSK_STK_PROPERTY_READ_ONLY);

		// start property idx starts  from last built-in property...
		//idx_start_user_properties=stk_properties->count;
		//lenght_user_properties=0;
	}

	void ScriptObjectClass::init(ScriptClass *irv, void * _c_object){


		createSymbols(irv);

	}*/

	ScriptFunction *ScriptObjectClass::getConstructorFunction(){

		ScriptClass *script_class=getScriptClass();
		if(script_class->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)script_class->symbol_members->items[script_class->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ScriptObjectClass::itHasSetMetamethod(){
		return getProperty(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_SET),NULL) != NULL;
		/*metamethod_operator[BYTE_CODE_METAMETHOD_SET]!=NULL){
			return script_class->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->count > 0;
		}
		return false;*/
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
		StackElement *stk_function=getProperty(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_TO_STRING),NULL);

		if(stk_function != NULL){ // get first element
			if(stk_function->properties & MSK_STK_PROPERTY_FUNCTION){
				ScriptFunction *ptr_function=(ScriptFunction *)stk_function->stk_value;
				if((ptr_function->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){

					StackElement result=VM_EXECUTE(
							this->zs->getVirtualMachine()
							,this
							,ptr_function
							,NULL
							,0
					);
					if(STK_IS_SCRIPT_OBJECT_STRING(&result)){
						ScriptObjectString *so=(ScriptObjectString *)result.stk_value;
						// capture string...
						std::string aux=so->toString();
						// ... destroy lifetime object we don't need anymore
						this->zs->getVirtualMachine()->destroyLifetimeObject(so);
						// return
						return aux;
					}
				}
			}
		}

		return ScriptObjectAnonymous::toString();
	}


	// built-in only for initialized
	StackElement * ScriptObjectClass::addPropertyBuiltIn(const std::string & symbol_value){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		map_property_keys->set(key_value.c_str(),stk_elements.count);

		StackElement *new_stk=newSlot();
		*new_stk=stk_undefined;

  	    return new_stk;
	}

	ScriptObjectClass::~ScriptObjectClass(){
		ScriptClass *script_class=getScriptClass();

		// deallocate member function objects
		for(int i=0; i< stk_elements.count; i++){
			StackElement *stk=(StackElement *)stk_elements.items[i];
			if(stk->properties & MSK_STK_PROPERTY_FUNCTION_MEMBER){
				delete (FunctionMember *)stk->stk_value;
			}
		}

		for ( unsigned i = 0; i < script_class->symbol_members->count; i++){
			Symbol *symbol = (Symbol *)script_class->symbol_members->items[i];

			if(symbol->properties == 0){ // member var
				StackElement *stk=(StackElement *)stk_elements.items[i];
				if(stk->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					ScriptObject *so=(ScriptObject *)stk->stk_value;

					if(so->shared_pointer->data.n_shares==1){ // can delete because is shared with only this
						free(so->shared_pointer);
						delete so;
					}else{
						if(so->shared_pointer->data.n_shares>1){ // we deferr this share
							so->shared_pointer->data.n_shares--;
						}
					}
				}
			}
		}


		bool deallocated = false;
		if(created_object != 0 && delete_c_object){
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(script_class_native,created_object);//(*(script_class_native->c_destructor))(created_object);
			deallocated=true;

		}

//#ifdef __DEBUG__
		if(!deallocated && was_created_by_constructor){
			fprintf(stderr,"%s"
					,zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
							,SFI_GET_FILE_LINE(info_function_new, instruction_new)
					).c_str()
				);
			//return false;
		}

	}


}
