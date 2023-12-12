/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_CONSTRUCTOR_CLASS(_script_engine,sc) (*((void *(*)(zetscript::ScriptEngine *))sc->new_native_instance))(_script_engine)
#define CALL_DESTRUCTOR_CLASS(_script_engine,sc,obj) (*((void (*)(zetscript::ScriptEngine *, void *))sc->delete_native_instance))(_script_engine,obj)

namespace zetscript{

	ClassScriptObject * ClassScriptObject::newClassScriptObject(ScriptEngine *_script_engine, ScriptTypeId _script_type_id,void *_c_object){
		return new ClassScriptObject(_script_engine, _script_type_id,_c_object);
	}

	ClassScriptObject::ClassScriptObject(
			ScriptEngine *_script_engine
			, ScriptTypeId _script_type_id
			,void *_c_object
	):ObjectScriptObject(_script_engine){
		was_created_by_constructor=false;
		info_function_new=NULL;
		instruction_new=NULL;

	//	type = NULL;
		c_object = NULL;
		created_object = NULL;
		script_type_id = SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT;
		delete_c_object_on_destroy = false; // --> user is responsible to delete C objects!
		script_class_native=NULL;

		script_engine = _script_engine;
		StackElement *stack_element;
		String error;

		vm=script_engine->getVirtualMachine();

		script_type_id=_script_type_id;
		ScriptType *type=getScriptType();
		Vector<Symbol *> *member_vars=type->scope->symbol_variables;
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for(int i = 0; i < member_vars->length(); i++){

			Symbol * symbol = (Symbol *)member_vars->get(i);

			// we add symbol as field. In it will have the same idx as when were evaluated declared symbols on each type
			if((stack_element=addBuiltinField(
				symbol->name
			))==NULL){
				return;
			}

			if(symbol->properties & SYMBOL_PROPERTY_NATIVE_OBJECT_REF) //if(IS_CLASS_C)
			{
				// we know the type object so we assign the pointer ...
				void *ptr_variable=(void *)((zs_int)this->c_object + symbol->ref_ptr);
				*stack_element=convertSymbolToStackElement(this->script_engine,symbol,ptr_variable);
			}else if(symbol->properties & (SYMBOL_PROPERTY_CONST)){ // stack element
				stack_element->value=(zs_int)(vm_get_stack_elements(this->vm) + symbol->ref_ptr); // load from global stk
				stack_element->properties=StackElementProperty::STACK_ELEMENT_PROPERTY_PTR_STK;
			}else if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
				stack_element->value=(zs_int)(new StackElementMemberProperty(this,(MemberProperty *)symbol->ref_ptr));
				stack_element->properties=STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY;
			}
		}

		//-------------------------------------------------------------------------------
		// LINK C OBJECT

		c_object = _c_object;
		script_class_native=NULL;

		// search native type
		if(type->isNativeType()){
			script_class_native=type;
		}else {
			ScriptType *sc=type;
			// get first type with c inheritance...
			while((sc->base_script_type_ids->length()>0) && (script_class_native==NULL)){
				sc=this->script_engine->getScriptTypesFactory()->getScriptType(sc->base_script_type_ids->get(0)); // get base type (only first in script because has single inheritance)...
				if(sc->isNativeType()){ // we found the native script type!
					script_class_native=sc;
					break;
				}
			}
		}

		// create object if type is native or it derives from a native type
		if(c_object == NULL && script_class_native != NULL){
			// check that native type is instantiable
			if(script_class_native->new_native_instance == NULL){
				ZS_THROW_RUNTIME_ERROR(
						"Cannot instantiate object '%s' because it extends from native type '%s' as not instantiable. "
						"To solve this issue, register type '%s' as instantiable (i.e register type '%s' with new/delete functions)"
						,type->name.toConstChar()
						,script_class_native->name.toConstChar()
						,script_class_native->name.toConstChar()
						,script_class_native->name.toConstChar()
				);
			}
			// if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			created_object = CALL_CONSTRUCTOR_CLASS(_script_engine,script_class_native); // (*type->new_native_instance)();
			was_created_by_constructor=true;
			c_object = created_object;
			delete_c_object_on_destroy=true; // destroy object when type is destroyed. It will be safe (in principle)
		}

		// execute init for variable members (not dynamic)
		callConstructorMemberVariables(type);
	}

	void ClassScriptObject::callConstructorMemberVariables(ScriptType *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->base_script_type_ids->length()>0){
			callConstructorMemberVariables(this->script_engine->getScriptTypesFactory()->getScriptType(sc->base_script_type_ids->get(0)));
		}

		if(sc->sf_field_initializer != NULL){ // execute if only script type
			vm_execute(vm,this,sc->sf_field_initializer);
		}
	}

	ScriptFunction *ClassScriptObject::getConstructorFunction(){

		ScriptType *type=getScriptType();
		if(type->idx_constructor_member_function != ZS_UNDEFINED_IDX){
			return (ScriptFunction *)type->scope->symbol_functions->get(type->idx_constructor_member_function);
		}

		return NULL;
	}

	bool ClassScriptObject::itHasSetMetamethod(){
		return getStackElementByKeyName(MetamethodHelper::getMetamethodSymbolName(METAMETHOD_SET)) != NULL;
	}

	void ClassScriptObject::deleteNativeObjectOnDestroy(bool _delete_on_destroy){

		if(this->script_type_id < SCRIPT_TYPE_ID_MAX){
			return;
		}

		created_object=NULL;
		if((this->delete_c_object_on_destroy = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	void * ClassScriptObject::getNativeObject(){
		return c_object;
	}

	bool ClassScriptObject::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptType * ClassScriptObject::getNativeScriptClass(){
		 return script_class_native;
	}

	String ClassScriptObject::toString(){
		// check whether toString is implemented...
		Symbol *symbol_function=getScriptType()->getSymbolMemberFunction(MetamethodHelper::getMetamethodSymbolName(METAMETHOD_TO_STRING));
		String aux="";
		if(symbol_function != NULL){ // get first element
			ScriptFunction *ptr_function=(ScriptFunction *)symbol_function->ref_ptr;
			if((ptr_function->properties & SCRIPT_FUNCTION_PROPERTY_STATIC) == 0){

				if((ptr_function->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF) == 0){

					StackElement result=ZS_VM_EXECUTE(
							this->vm
							,this
							,ptr_function
							,NULL
							,0
					);

					if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&result)){
						StringScriptObject *so=(StringScriptObject *)result.value;
						// capture string...
						aux=so->toString();
						// ... destroy lifetime object we don't need anymore
						vm_unref_lifetime_object(this->vm,so);
						// return
					}else{
						aux=script_engine->stackElementToString(&result);
					}
				}else{ // expect return an scriptobjectstring
					String *str=NULL;
					switch(ptr_function->return_script_type_id){
					case SCRIPT_TYPE_ID_STRING:
							aux=((String (*)(ScriptEngine *,void *))(ptr_function->ref_native_function_ptr))(script_engine,this->c_object);
							break;
					case SCRIPT_TYPE_ID_STRING_PTR:
							str=((String * (*)(ScriptEngine *,void *))(ptr_function->ref_native_function_ptr))(script_engine,this->c_object);
							if(str == NULL){
								ZS_THROW_RUNTIME_ERRORF("toString: str NULL");
							}
							aux=*str;
							break;
					default:
						ZS_THROW_RUNTIME_ERRORF("toString: expected String or *String");
						break;
					}
				}
			}
			return aux;
		}
		return ObjectScriptObject::toString();
	}

	ClassScriptObject::~ClassScriptObject(){

		if(created_object != 0 && delete_c_object_on_destroy){

			if(script_class_native->delete_native_instance==NULL){
				ZS_THROW_RUNTIME_ERROR(
						"Cannot delete variable as type '%s' because it was defined as not instantiable but created through 'newClassScriptObject'. "
						"To solve this issue, register type '%s' as instantiable (i.e register type '%s' with new/delete functions)"
						,script_class_native->name.toConstChar()
						,script_class_native->name.toConstChar()
						,script_class_native->name.toConstChar()
				);
			}
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(script_engine,script_class_native,created_object);//(*(script_class_native->delete_native_instance))(created_object);
		}else if(was_created_by_constructor){
			fprintf(stderr,"%s",String::format(ZS_FORMAT_FILE_LINE" Allocated C pointer not deallocated"
						,SFI_GET_FILE_LINE(info_function_new, instruction_new)
				).toConstChar()
			);
		}
	}
}
