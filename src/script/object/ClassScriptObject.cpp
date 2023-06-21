/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_CONSTRUCTOR_CLASS(_zs,sc) (*((void *(*)(zetscript::ZetScript *))sc->new_native_instance))(_zs)
#define CALL_DESTRUCTOR_CLASS(_zs,sc,obj) (*((void (*)(zetscript::ZetScript *, void *))sc->delete_native_instance))(_zs,obj)

namespace zetscript{

	ClassScriptObject * ClassScriptObject::newClassScriptObject(ZetScript *_zs, short _idx_script_type,void *_c_object){
		return new ClassScriptObject(_zs, _idx_script_type,_c_object);
	}

	ClassScriptObject::ClassScriptObject(
			ZetScript *_zs
			, short _idx_script_type
			,void *_c_object
	):ObjectScriptObject(_zs){
		was_created_by_constructor=false;
		info_function_new=NULL;
		instruction_new=NULL;

	//	script_type = NULL;
		c_object = NULL;
		created_object = NULL;
		idx_script_type = IDX_TYPE_SCRIPT_OBJECT_CLASS;
		delete_c_object_on_destroy = false; // --> user is responsible to delete C objects!
		script_class_native=NULL;

		zs = _zs;
		StackElement *se;
		zs_string error;

		vm=zs->getVirtualMachine();

		idx_script_type=_idx_script_type;
		ScriptType *script_type=getScriptType();
		zs_vector<Symbol *> *member_vars=script_type->scope_script_type->symbol_variables;
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for(int i = 0; i < member_vars->size(); i++){

			Symbol * symbol = (Symbol *)member_vars->items[i];

			// we add symbol as field. In it will have the same idx as when were evaluated declared symbols on each type
			if((se=addBuiltinField(
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
				se->value=(zs_int)(new StackElementMemberProperty(this,(MemberProperty *)symbol->ref_ptr));
				se->properties=STK_PROPERTY_MEMBER_PROPERTY;
			}
		}

		//-------------------------------------------------------------------------------
		// LINK C OBJECT

		c_object = _c_object;
		script_class_native=NULL;

		// search native type
		if(script_type->isNativeType()){
			script_class_native=script_type;
		}else {
			ScriptType *sc=script_type;
			// get first type with c inheritance...
			while((sc->idx_base_types->size()>0) && (script_class_native==NULL)){
				sc=this->zs->getScriptTypeFactory()->getScriptType(sc->idx_base_types->items[0]); // get base type (only first in script because has single inheritance)...
				if(sc->isNativeType()){ // we found the native script type!
					script_class_native=sc;
					break;
				}
			}
		}

		// create object if type is native or it derives from a native type
		if(c_object == NULL && script_class_native != NULL){
			// if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			created_object = CALL_CONSTRUCTOR_CLASS(zs,script_class_native); // (*script_type->new_native_instance)();
			was_created_by_constructor=true;
			c_object = created_object;
			delete_c_object_on_destroy=true; // destroy object when type is destroyed. It will be safe (in principle)
		}

		// execute init for variable members (not dynamic)
		callConstructorMemberVariables(script_type);
	}

	void ClassScriptObject::callConstructorMemberVariables(ScriptType *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->idx_base_types->size()>0){
			callConstructorMemberVariables(this->zs->getScriptTypeFactory()->getScriptType(sc->idx_base_types->items[0]));
		}

		if(sc->sf_field_initializer != NULL){ // execute if only script type
			vm_execute(vm,this,sc->sf_field_initializer);
		}
	}

	ScriptFunction *ClassScriptObject::getConstructorFunction(){

		ScriptType *script_type=getScriptType();
		if(script_type->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)script_type->scope_script_type->symbol_functions->items[script_type->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ClassScriptObject::itHasSetMetamethod(){
		return getStackElement(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET)) != NULL;
	}

	void ClassScriptObject::deleteNativeObjectOnDestroy(bool _delete_on_destroy){

		if(this->idx_script_type<Type::IDX_TYPE_MAX){
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

	zs_string ClassScriptObject::toString(){
		// check whether toString is implemented...
		Symbol *symbol_function=getScriptType()->getSymbolMemberFunction(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_TO_STRING));
		zs_string aux="";
		if(symbol_function != NULL){ // get first element
			ScriptFunction *ptr_function=(ScriptFunction *)symbol_function->ref_ptr;
			if((ptr_function->properties & FUNCTION_PROPERTY_STATIC) == 0){

				if((ptr_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){

					StackElement result=ZS_VM_EXECUTE(
							this->vm
							,this
							,ptr_function
							,NULL
							,0
					);

					if(STK_IS_STRING_SCRIPT_OBJECT(&result)){
						StringScriptObject *so=(StringScriptObject *)result.value;
						// capture string...
						aux=so->toString();
						// ... destroy lifetime object we don't need anymore
						vm_unref_lifetime_object(this->vm,so);
						// return
					}else{
						aux=zs->stackElementToString(&result);
					}
				}else{ // expect return an scriptobjectstring
					zs_string *str=NULL;
					switch(ptr_function->idx_script_type_return){
					case IDX_TYPE_ZS_STRING_C:
							aux=((zs_string (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(zs,this->c_object);
							break;
					case IDX_TYPE_ZS_STRING_PTR_C:
							str=((zs_string * (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(zs,this->c_object);
							if(str == NULL){
								ZS_THROW_RUNTIME_ERRORF("toString: str NULL");
							}
							aux=*str;
							break;
					default:
						ZS_THROW_RUNTIME_ERRORF("toString: expected zs_string or *zs_string");
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
						"Cannot delete variable as type '%s' because it was defined as not instanceable but created through 'newClassScriptObject'. To solve this issue, define type '%s' as instanceable (i.e to have defined type '%s' with constructor/destructor functions)"
						,script_class_native->str_script_type.c_str()
						,script_class_native->str_script_type.c_str()
						,script_class_native->str_script_type.c_str()
				);
			}
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(zs,script_class_native,created_object);//(*(script_class_native->delete_native_instance))(created_object);
		}else if(was_created_by_constructor){
			fprintf(stderr,"%s",zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
						,SFI_GET_FILE_LINE(info_function_new, instruction_new)
				).c_str()
			);
		}
	}
}
