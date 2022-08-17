/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define CALL_CONSTRUCTOR_CLASS(_zs,sc) (*((void *(*)(zetscript::ZetScript *))sc->c_constructor))(_zs)
#define CALL_DESTRUCTOR_CLASS(_zs,sc,obj) (*((void (*)(zetscript::ZetScript *, void *))sc->c_destructor))(_zs,obj)

namespace zetscript{

	ScriptObjectClass * ScriptObjectClass::newScriptObjectClass(ZetScript *_zs, short _idx_script_type,void *_c_object){
		ScriptObjectClass *sc=new ScriptObjectClass();
		sc->init(_zs,_idx_script_type,_c_object);
		return sc;
	}

	ScriptObjectClass::ScriptObjectClass(){
		was_created_by_constructor=false;
		info_function_new=NULL;
		instruction_new=NULL;

	//	script_type = NULL;
		c_object = NULL;
		created_object = NULL;
		idx_script_type = IDX_TYPE_SCRIPT_OBJECT_CLASS;
		delete_c_object_on_destroy = false; // --> user is responsible to delete C objects!
		script_class_native=NULL;
	}

	void ScriptObjectClass::callConstructorMemberVariables(ScriptType *sc ){
		if(sc == NULL){
			return;
		}

		if(sc->idx_base_types->size()>0){
			callConstructorMemberVariables(this->zs->getScriptTypeFactory()->getScriptType(sc->idx_base_types->at(0)));
		}

		if(sc->sf_field_initializer != NULL){ // execute if only script type
			vm_execute(vm,this,sc->sf_field_initializer);
		}
	}


	void ScriptObjectClass::init(ZetScript *_zs, short _idx_script_type,void *_c_object){
		zs = _zs;
		StackElement *se;
		std::string error;

		vm=zs->getVirtualMachine();

		idx_script_type=_idx_script_type;
		ScriptType *script_type=getScriptType();
		std::vector<Symbol *> *member_vars=script_type->scope_script_type->symbol_variables;
		//------------------------------------------------------------------------------
		// pre-register built-in members...
		for(unsigned i = 0; i < member_vars->size(); i++){

			Symbol * symbol = (Symbol *)member_vars->at(i);

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each type
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

		c_object = _c_object;
		script_class_native=NULL;

		// search native type
		if(script_type->isNativeType()){
			script_class_native=script_type;
		}else {
			ScriptType *sc=script_type;
			// get first type with c inheritance...
			while((sc->idx_base_types->size()>0) && (script_class_native==NULL)){
				sc=this->zs->getScriptTypeFactory()->getScriptType(sc->idx_base_types->at(0)); // get base type (only first in script because has single inheritance)...
				if(sc->isNativeType()){ // we found the native script type!
					script_class_native=sc;
					break;
				}
			}
		}

		// create object if type is native or it derives from a native type
		if(c_object == NULL && script_class_native != NULL){
			// if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			created_object = CALL_CONSTRUCTOR_CLASS(zs,script_class_native); // (*script_type->c_constructor)();
			was_created_by_constructor=true;
			c_object = created_object;
			delete_c_object_on_destroy=true; // destroy object when type is destroyed. It will be safe (in principle)
		}

		// execute init for variable members (not dynamic)
		callConstructorMemberVariables(script_type);
	}

	ScriptFunction *ScriptObjectClass::getConstructorFunction(){

		ScriptType *script_type=getScriptType();
		if(script_type->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)script_type->scope_script_type->symbol_functions->at(script_type->idx_function_member_constructor);
		}

		return NULL;
	}

	bool ScriptObjectClass::itHasSetMetamethod(){
		return getProperty(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_SET)) != NULL;
	}

	void ScriptObjectClass::deleteNativeObjectOnDestroy(bool _delete_on_destroy){

		if(this->idx_script_type<Type::IDX_TYPE_MAX){
			return;
		}

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

	ScriptType * ScriptObjectClass::getNativeScriptClass(){
		 return script_class_native;
	}

	std::string ScriptObjectClass::toString(){
		// check whether toString is implemented...
		Symbol *symbol_function=getScriptType()->getSymbolMemberFunction(byte_code_metamethod_to_symbol_str(BYTE_CODE_METAMETHOD_TO_STRING));
		std::string aux="";
		if(symbol_function != NULL){ // get first element
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
					std::string *str=NULL;
					switch(ptr_function->idx_script_type_return){
					case IDX_TYPE_STRING_C:
							aux=((std::string (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(zs,this->c_object);
							break;
					case IDX_TYPE_STRING_PTR_C:
							str=((std::string * (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(zs,this->c_object);
							if(str == NULL){
								THROW_RUNTIME_ERRORF("toString: str NULL");
							}
							aux=*str;
							break;
					default:
						THROW_RUNTIME_ERRORF("toString: expected std::string or *std::string");
						break;
					}
				}
			}
			return aux;
		}
		return ScriptObjectObject::toString();
	}

	ScriptObjectClass::~ScriptObjectClass(){

		if(created_object != 0 && delete_c_object_on_destroy){
			 // only erases pointer if basic type or user/auto delete is required ...
			CALL_DESTRUCTOR_CLASS(zs,script_class_native,created_object);//(*(script_class_native->c_destructor))(created_object);
		}else if(was_created_by_constructor){
			fprintf(stderr,"%s",zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
						,SFI_GET_FILE_LINE(info_function_new, instruction_new)
				).c_str()
			);
		}
	}
}
