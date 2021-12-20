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
		idx_script_class = IDX_TYPE_SCRIPT_OBJECT_CLASS;
		delete_c_object_on_destroy = false; // --> user is responsible to delete C objects!
		script_class_native=NULL;
	}

	void ScriptObjectClass::callConstructorMemberVariables(ScriptClass *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->idx_base_classes->count>0){
			callConstructorMemberVariables(this->zs->getScriptClassFactory()->getScriptClass(sc->idx_base_classes->items[0]));
		}

		if(sc->sf_field_initializer != NULL){ // execute if only script class
			vm_execute(vm,this,sc->sf_field_initializer);
		}
	}


	void ScriptObjectClass::init(ZetScript *_zs, short _idx_script_class,void *_c_object){
		zs = _zs;
		StackElement *se;
		zs_string error;

		vm=zs->getVirtualMachine();

		idx_script_class=_idx_script_class;
		ScriptClass *script_class=getScriptClass();
		zs_vector *member_vars=script_class->class_scope->symbol_variables;
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for ( unsigned i = 0; i < member_vars->count; i++){

			Symbol * symbol = (Symbol *)member_vars->items[i];
			//bool is_script_function=symbol->properties & SYMBOL_PROPERTY_FUNCTION;
			//bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.


			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			if((se=addBuiltinProperty(
				symbol->name
			))==NULL){
				return;
			}

			if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{
				// we know the type object so we assign the pointer ...
				void *ptr_variable=(void *)((zs_int)this->c_object + symbol->ref_ptr);
				*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
			}else if(symbol->properties & (SYMBOL_PROPERTY_CONST)){ // stack element
				se->value=(zs_int)(vm_get_stack_elements(this->vm) + symbol->ref_ptr); // load from global stk
				se->properties=STK_PROPERTY_PTR_STK;
			}else if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
				se->value=(zs_int)(new StackMemberProperty(this,(MemberProperty *)symbol->ref_ptr));
				se->properties=STK_PROPERTY_MEMBER_PROPERTY;
			}

		}

		//-------------------------------------------------------------------------------
		// LINK C OBJECT

		//this->script_class = irv;
		//idx_type_class = irv->idx_type_class;
		c_object = _c_object;
		script_class_native=NULL;

		// search native class
		if(script_class->isNativeClass()){
			script_class_native=script_class;
		}else {
			ScriptClass *sc=script_class;
			// get first class with c inheritance...
			while((sc->idx_base_classes->count>0) && (script_class_native==NULL)){
				sc=this->zs->getScriptClassFactory()->getScriptClass(sc->idx_base_classes->items[0]); // get base class (only first in script because has single inheritance)...
				if(sc->isNativeClass()){ // we found the native script class!
					script_class_native=sc;
					break;
				}
			}
		}

		// create object if class is native or it derives from a native class
		if(c_object == NULL && script_class_native != NULL){
			// if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			created_object = CALL_CONSTRUCTOR_CLASS(script_class_native); // (*script_class->c_constructor)();
			was_created_by_constructor=true;
			c_object = created_object;
			delete_c_object_on_destroy=true; // destroy object when class is destroyed. It will be safe (in principle)
		}

		// execute init for variable members (not dynamic)
		callConstructorMemberVariables(script_class);
	}

	ScriptFunction *ScriptObjectClass::getConstructorFunction(){

		ScriptClass *script_class=getScriptClass();
		if(script_class->idx_function_member_constructor != IDX_ZS_UNDEFINED){
			return (ScriptFunction *)script_class->class_scope->symbol_functions->items[script_class->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ScriptObjectClass::itHasSetMetamethod(){
		return getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET)) != NULL;
	}

	void ScriptObjectClass::deleteNativeObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object_on_destroy = _delete_on_destroy)==true){
			created_object=c_object;
		}
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

	zs_string ScriptObjectClass::toString(){
		// check whether toString is implemented...
		Symbol *symbol_function=getScriptClass()->getSymbolMemberFunction(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_TO_STRING));
		zs_string aux="";
		if(symbol_function != NULL){ // get first element
			//if(symbol_function->properties & (SYMBOL_PROPERTY_MEMBER_FUNCTION | SYMBOL_PROPERTY_FUNCTION))
			//{
				ScriptFunction *ptr_function=(ScriptFunction *)symbol_function->ref_ptr;
				if((ptr_function->properties & FUNCTION_PROPERTY_STATIC) == 0){

					if((ptr_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){

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
							aux=so->toString();
							// ... destroy lifetime object we don't need anymore
							vm_unref_lifetime_object(this->vm,so);
							// return
						}else{
							aux=stk_to_str(zs,&result);
						}
					}else{ // expect return an scriptobjectstring
						zs_string *str=NULL;
						switch(ptr_function->idx_return_type){
						case IDX_TYPE_STRING_C:
								aux=((zs_string (*)(void *))(ptr_function->ref_native_function_ptr))(this->c_object);
								break;
						case IDX_TYPE_STRING_PTR_C:
								str=((zs_string * (*)(void *))(ptr_function->ref_native_function_ptr))(this->c_object);
								if(str == NULL){
									THROW_RUNTIME_ERROR("toString: str NULL");
								}
								aux=*str;
								break;
						default:
							THROW_RUNTIME_ERROR("toString: expected zs_string or *zs_string");
							break;
						}
					}
				}
			//}
			return aux;
		}
		return ScriptObjectObject::toString();
	}

	ScriptObjectClass::~ScriptObjectClass(){
		ScriptClass *script_class=getScriptClass();

		if(created_object != 0 && delete_c_object_on_destroy){
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(script_class_native,created_object);//(*(script_class_native->c_destructor))(created_object);
		}else if(was_created_by_constructor){
			fprintf(stderr,"%s",zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
						,SFI_GET_FILE_LINE(info_function_new, instruction_new)
				).c_str()
			);
		}
	}
}
