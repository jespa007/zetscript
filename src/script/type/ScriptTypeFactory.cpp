/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_BIND_STRUCT(type_class, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(ZS_STR(type_class));


#define SCF_BIND_CLASS(name_class, type_class, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(idx_script_type >= IDX_TYPE_MAX){\
		PRAGMA_POP\
		ZS_THROW_RUNTIME_ERROR("The type to bind '%s' should be a built in type",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(name_class,type_class##Wrap_New,type_class##Wrap_Delete);

#define SCF_BIND_SINGLETON_CLASS(type_class, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(ZS_STR(type_class));

#define SCF_BIND_SINGLETON_NAME_CLASS(name, type_class, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(name);

#define SCF_BIND_NATIVE_TYPE(type, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(ZS_STR(type),"",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);\
		sc->scope_script_type->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->str_script_type_ptr=(typeid(type).name());\
	}

#define SCF_BIND_NATIVE_CUSTOM_TYPE(__name__, type, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(__name__);\
		sc->scope_script_type->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->str_script_type_ptr=(typeid(type).name());\
	}

#define SCF_BIND_TYPE(str_type, idx_script_type)\
	if(script_types->size()!=idx_script_type){\
		ZS_THROW_RUNTIME_ERROR("Error initializing built in type: '%s'",str_type);\
		return;\
	}else{\
		registerScriptType(str_type);\
	}

namespace zetscript{


	const char * k_str_void_type =typeid(void).name();
	const char * k_str_zs_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_zs_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_zs_float_type_ptr=typeid(zs_float *).name();
	const char * k_str_const_zs_float_type_ptr=typeid(const zs_float *).name();
	const char * k_str_zs_string_type_ptr=typeid(zs_string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	const char * k_str_zs_int_type=typeid(zs_int).name();
	const char * k_str_zs_float_type=typeid(zs_float).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(StackElement).name();
	const char * k_str_zetscript_type_ptr=typeid(ZetScript *).name();

	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ObjectScriptObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(StringScriptObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ArrayScriptObject)

	ScriptTypeFactory::ScriptTypeFactory(ZetScript *_zs){
		zs = _zs;
		vm = _zs->getVirtualMachine();
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		main_function=NULL;
		main_object=NULL;
		script_types=new zs_vector<ScriptType *>;
		main_object=NULL;
		main_function=NULL;
		idx_clear_checkpoint=0;

		stk_constants=NULL;
		stk_constants=new zs_map();

	}

	void ScriptTypeFactory::init(){
		zs_string error;

		// ScriptFunctionFactory has to be created
		main_object=registerScriptType(MAIN_SCRIPT_CLASS_NAME); // 0
		ZS_MAIN_SCOPE(this)->script_type_owner=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(
				ZS_MAIN_SCRIPT_FUNCTION_NAME
		);

		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;
		main_function->scope_script_function=ZS_MAIN_SCOPE(this);

		idx_clear_checkpoint=1; // by default restore till main type.
	}

	ZetScript *ptrToZetScriptPtr(ZetScript *_zs, zs_int ptr){
		ZS_UNUSUED_PARAM(_zs);
		return (ZetScript *)ptr;
	}

	zs_float parseFloat(ZetScript *_zs,zs_int number){
		ZS_UNUSUED_PARAM(_zs);
		return (zs_float)(number);
	}

	zs_float parseFloat(ZetScript *_zs,zs_float *number){
		ZS_UNUSUED_PARAM(_zs);
		return *number;
	}

	zs_float parseFloat(ZetScript *_zs,zs_string  *number_str){
		ZS_UNUSUED_PARAM(_zs);
		zs_float result=0;
		zs_float *result_ptr=zs_strutils::parse_zs_float(*number_str);

		if(result_ptr != NULL){
			result=*result_ptr;
			delete result_ptr;
		}

		return result;
	}

	zs_int parseInt(ZetScript *_zs,zs_int number){
		ZS_UNUSUED_PARAM(_zs);
		return number;
	}

	zs_int parseInt(ZetScript *_zs,zs_float *number){
		ZS_UNUSUED_PARAM(_zs);
		return (zs_int)(*number);
	}

	zs_int parseInt(ZetScript *_zs,zs_string  *number_str){
		ZS_UNUSUED_PARAM(_zs);
		zs_int result=0;
		zs_int *result_ptr=zs_strutils::parse_zs_int(*number_str);
		if(result_ptr!=NULL){
			result=*result_ptr;
			delete result_ptr;
		}
		return result;
	}

	bool isNumber(ZetScript *_zs,StackElement *_stk){
		ZS_UNUSUED_PARAM(_zs);
		return (_stk->properties & (STK_PROPERTY_ZS_INT | STK_PROPERTY_ZS_FLOAT)) != 0;
	}

	void ScriptTypeFactory::setup(){

		// !!!
		// !!! START REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		// primitives
		SCF_BIND_TYPE(ZS_TYPE_NAME_UNDEFINED,IDX_TYPE_UNDEFINED);
		SCF_BIND_TYPE(ZS_TYPE_NAME_NULL,IDX_TYPE_NULL);
		SCF_BIND_NATIVE_TYPE(void,IDX_TYPE_VOID_C);
		SCF_BIND_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_INT,zs_int,IDX_TYPE_ZS_INT_C);
		SCF_BIND_NATIVE_TYPE(zs_int *,IDX_TYPE_ZS_INT_PTR_C);
		SCF_BIND_NATIVE_TYPE(char *,IDX_TYPE_CHAR_PTR_C);
		SCF_BIND_NATIVE_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		SCF_BIND_NATIVE_TYPE(zs_string,IDX_TYPE_ZS_STRING_C);
		SCF_BIND_NATIVE_TYPE(zs_string *,IDX_TYPE_ZS_STRING_PTR_C);
		SCF_BIND_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_BOOL,bool,IDX_TYPE_BOOL_C);
		SCF_BIND_NATIVE_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		SCF_BIND_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_FLOAT,zs_float,IDX_TYPE_ZS_FLOAT_C);
		SCF_BIND_NATIVE_TYPE(zs_float *,IDX_TYPE_ZS_FLOAT_PTR_C);
		SCF_BIND_NATIVE_TYPE(const zs_float *,IDX_TYPE_CONST_ZS_FLOAT_PTR_C);

		// estructures
		SCF_BIND_STRUCT(StackElement,IDX_TYPE_STACK_ELEMENT);
		SCF_BIND_STRUCT(ContainerSlot,IDX_TYPE_CONTAINER_SLOT);


		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_FUNCTION,ScriptFunction,IDX_TYPE_FUNCTION);
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_VAR_REF,VarRefScriptObject,IDX_TYPE_SCRIPT_OBJECT_VAR_REF);
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER,MemberFunctionScriptObject,IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER);
		SCF_BIND_SINGLETON_CLASS(ZetScript,IDX_TYPE_SCRIPT_OBJECT_ZETSCRIPT);

		SCF_BIND_CLASS(ZS_TYPE_NAME_OBJECT_STRING,StringScriptObject,IDX_TYPE_SCRIPT_OBJECT_STRING);

		// Script object iterators
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_STRING,StringIteratorScriptObject,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING);
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_ARRAY,ArrayIteratorScriptObject,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY);
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT,ObjectIteratorScriptObject,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		SCF_BIND_CLASS(ZS_TYPE_NAME_OBJECT_ARRAY,ArrayScriptObject,IDX_TYPE_SCRIPT_OBJECT_ARRAY);
		SCF_BIND_CLASS(ZS_TYPE_NAME_OBJECT_OBJECT,ObjectScriptObject,IDX_TYPE_SCRIPT_OBJECT_OBJECT);
		SCF_BIND_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_CLASS,ClassScriptObject,IDX_TYPE_SCRIPT_OBJECT_CLASS);
		// it needs script object type to have zetscript reference
		// BUILT-IN SCRIPT OBJECTS CLASSES
		//------------------------

		// !!!
		// !!! END REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		zs->bindFunction("ptrToZetScriptPtr",ptrToZetScriptPtr);

		ScriptType *integer_type=this->getScriptType(ZS_TYPE_NAME_INT);
		integer_type->bindStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_float *)>(parseInt));
		integer_type->bindStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_int )>(parseInt));
		integer_type->bindStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_string *)>(parseInt));

		ScriptType *float_type=this->getScriptType(ZS_TYPE_NAME_FLOAT);
		float_type->bindStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_int )>(parseFloat));
		float_type->bindStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_float *)>(parseFloat));
		float_type->bindStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_string *)>(parseFloat));


		// Global symbols
		zs->bindFunction("isNumber",isNumber);

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// String

		// properties
		bindMemberPropertyGetter<StringScriptObject>("length",StringScriptObjectWrap_length);

		// members
		bindStaticMemberFunction<StringScriptObject>("formatNative",StringScriptObject::format);
		bindMemberFunction<StringScriptObject>("eraseAt",StringScriptObjectWrap_eraseAt);
		bindMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringScriptObject *so, zs_int, zs_int )>(StringScriptObjectWrap_insertAt));
		bindMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringScriptObject *so, zs_int, zs_string *)>(StringScriptObjectWrap_insertAt));
		bindMemberFunction<StringScriptObject>("clear",StringScriptObjectWrap_clear);
		bindMemberFunction<StringScriptObject>("replace",StringScriptObjectWrap_replace);
		bindMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(StringScriptObjectWrap_split));
		bindMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(StringScriptObjectWrap_split));
		bindMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(&StringScriptObjectWrap_contains));
		bindMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectWrap_contains));

		bindMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(&StringScriptObjectWrap_indexOf));
		bindMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectWrap_indexOf));

		bindMemberFunction<StringScriptObject>("startsWith",StringScriptObjectWrap_startsWith);
		bindMemberFunction<StringScriptObject>("endsWith",StringScriptObjectWrap_endsWith);
		bindMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int,zs_int )>(&StringScriptObjectWrap_substring));
		bindMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectWrap_substring));
		bindMemberFunction<StringScriptObject>("append",static_cast<void (*)(ZetScript *,StringScriptObject *, StringScriptObject *)>(&StringScriptObjectWrap_append));
		bindMemberFunction<StringScriptObject>("append",static_cast<void (*)(ZetScript *,StringScriptObject *, zs_int )>(&StringScriptObjectWrap_append));
		bindMemberFunction<StringScriptObject>("toLowerCase",StringScriptObjectWrap_toLowerCase);
		bindMemberFunction<StringScriptObject>("toUpperCase",StringScriptObjectWrap_toUpperCase);

		bindMemberFunction<StringScriptObject>("_iter",StringScriptObjectWrap_iter);

		// IteratorString
		bindMemberFunction<StringIteratorScriptObject>("_postinc",StringIteratorScriptObjectWrap_next);
		bindMemberFunction<StringIteratorScriptObject>("_end",StringIteratorScriptObjectWrap_end);
		bindMemberFunction<StringIteratorScriptObject>("_get",StringIteratorScriptObjectWrap_get);

		//---------------------------------------------
		// Array

		// properties
		bindMemberPropertyGetter<ArrayScriptObject>("length",&ArrayScriptObjectWrap_length);

		// members
		bindMemberFunction<ArrayScriptObject>("push",&ArrayScriptObjectWrap_push);
		bindMemberFunction<ArrayScriptObject>("pop",&ArrayScriptObjectWrap_pop);
		bindMemberFunction<ArrayScriptObject>("insertAt",&ArrayScriptObjectWrap_insertAt);
		bindMemberFunction<ArrayScriptObject>("eraseAt",&ArrayScriptObjectWrap_eraseAt);
		bindMemberFunction<ArrayScriptObject>("clear",&ArrayScriptObjectWrap_clear);
		bindMemberFunction<ArrayScriptObject>("join",&ArrayScriptObjectWrap_join);
		bindMemberFunction<ArrayScriptObject>("extend",&ArrayScriptObjectWrap_extend);
		bindStaticMemberFunction<ArrayScriptObject>("concat",&ArrayScriptObjectWrap_concat);
		bindMemberFunction<ArrayScriptObject>("contains",&ArrayScriptObjectWrap_contains);

		bindMemberFunction<ArrayScriptObject>("_iter",&ArrayScriptObjectWrap_iter);
		bindStaticMemberFunction<ArrayScriptObject>("_equ",&ArrayScriptObjectWrap_equal);
		//bindStaticMemberFunction<ArrayScriptObject>("_add",&ArrayScriptObjectWrap_equal_add);

		// IteratorArray
		bindMemberFunction<ArrayIteratorScriptObject>("_postinc",ArrayIteratorScriptObjectWrap_next);
		bindMemberFunction<ArrayIteratorScriptObject>("_end",ArrayIteratorScriptObjectWrap_end);
		bindMemberFunction<ArrayIteratorScriptObject>("_get",ArrayIteratorScriptObjectWrap_get);


		//---------------------------------------------
		// Object
		bindStaticMemberFunction<ObjectScriptObject>("clear",&ObjectScriptObjectWrap_clear);
		bindStaticMemberFunction<ObjectScriptObject>("erase",&ObjectScriptObjectWrap_erase);
		bindStaticMemberFunction<ObjectScriptObject>("contains",&ObjectScriptObjectWrap_contains);
		bindStaticMemberFunction<ObjectScriptObject>("extend",&ObjectScriptObjectWrap_extend);
		bindStaticMemberFunction<ObjectScriptObject>("concat",ObjectScriptObject::concat);
		bindStaticMemberFunction<ObjectScriptObject>("keys",ObjectScriptObjectWrap_keys);
		bindStaticMemberFunction<ObjectScriptObject>("_iter",ObjectScriptObjectWrap_iter);

		// IteratorObject
		bindMemberFunction<ObjectIteratorScriptObject>("_postinc",ObjectIteratorScriptObjectWrap_next);
		bindMemberFunction<ObjectIteratorScriptObject>("_end",ObjectIteratorScriptObjectWrap_end);
		bindMemberFunction<ObjectIteratorScriptObject>("_get",ObjectIteratorScriptObjectWrap_get);

		zs->saveState();
	}

	//-----------------------------------------------------------------------------------------
	// STK REGISTER OBJECT
	StackElement * ScriptTypeFactory::registerStkConstantStringObject(const zs_string & _key,const zs_string & _value){

		StackElement *stk=NULL;
		StringScriptObject *so=NULL;

		if((stk = getStkConstantStringObject(_key))!=NULL){
			if(stk->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY)){
				return stk;
			}
			else{
				// throw
				ZS_THROW_RUNTIME_ERROR(
					"Cannot register constant '%s' as 'StringScriptObject', because is already registered as '%s'"
					,_key.c_str()
					,this->zs->stackElementToStringTypeOf(stk).c_str()
				);
			}
		}

		stk=new StackElement;

		stk_constants->set(_key.c_str(),(zs_int)stk);

		so=ZS_NEW_STRING_OBJECT(this->zs);

		// swap values stk_ref/value
		so->set(_value.c_str());
		stk->value=(zs_int)so;
		stk->properties=STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY;

		return stk;
	}

	StackElement *ScriptTypeFactory::getStkConstantStringObject(const zs_string & _key){
		return (StackElement *)stk_constants->get(_key.c_str());
	}

	/*StackElement * ScriptTypeFactory::registerStkObject(const zs_string & _key, zs_int _value){

		StackElement *stk=NULL;

		if((stk = getStkObject(_key))!=NULL){
			return stk;
		}

		stk=new StackElement;
		stk->value=(zs_int)_value;
		stk->properties=STK_PROPERTY_SCRIPT_OBJECT;

		this->stk_objects->set(_key.c_str(),(zs_int)stk);

		return stk;
	}

	StackElement *ScriptTypeFactory::getStkObject(const zs_string & _key){
		return (StackElement *)stk_objects->get(_key.c_str());
	}*/

	//
	// STK REGISTER OBJECT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------------------------------------------
	// REGISTER CONSTANTS
	//

	void ScriptTypeFactory::bindConstantVariable(const zs_string & _key, int _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->vm,symbol_variable->idx_position);
		stk->value=_value;
		stk->properties=STK_PROPERTY_ZS_INT|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::bindConstantVariable(const zs_string & _key, bool _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->vm,symbol_variable->idx_position);

		stk->value=_value;
		stk->properties=STK_PROPERTY_BOOL|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::bindConstantVariable(const zs_string & _key, zs_float _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->vm,symbol_variable->idx_position);
		ZS_FLOAT_COPY(&stk->value,&_value);
		stk->properties=STK_PROPERTY_ZS_FLOAT|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::bindConstantVariable(const zs_string & _key, const zs_string & _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->vm,symbol_variable->idx_position);
		*stk=*(this->registerStkConstantStringObject(_key,_value));
	}

	void ScriptTypeFactory::bindConstantVariable(const zs_string & _key, const char * _value, const char *registered_file, short registered_line){
		bindConstantVariable(_key, zs_string(_value), registered_file, registered_line);
	}

	// REGISTER CONSTANTS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	void ScriptTypeFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_types->size()-1;
			v > idx_start; // avoid main type
			v--
		){
			ScriptType * sc = (ScriptType *)script_types->get(v);
			delete sc;
			script_types->pop_back();
		}
	}

	void ScriptTypeFactory::saveState(){
		idx_clear_checkpoint = script_types->size()-1;
	}

	void ScriptTypeFactory::checkScriptTypeName(const zs_string & _str_script_type){

		if(script_types->size()>=MAX_REGISTER_CLASSES){
			ZS_THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(_str_script_type.empty()){
			ZS_THROW_RUNTIME_ERRORF("Class name empty");
		}

		if(zs->getScriptFunctionFactory()->getScriptFunctions()->size() > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(
					_str_script_type
					,ZS_NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN
			))!=NULL){
				ZS_THROW_RUNTIME_ERROR("Type name '%s' is already defined %s"
						,_str_script_type.c_str()
						,main_function_symbol->file!=NULL && strcmp(main_function_symbol->file,"")!=0 && main_function_symbol->line!=-1?
							zs_strutils::format("at [%s:%i]"
								,zs_path::get_filename(main_function_symbol->file).c_str()
								,main_function_symbol->line
							).c_str():""
				);
			}
		}
	}

	ScriptType * ScriptTypeFactory::registerScriptType(
			const zs_string & _str_script_type
			 ,const zs_string & _base_class_name
			 ,uint16_t _properties
			 ,const char * _file
			 , short _line
	){
		int  index;
		ScriptType *sc=NULL;

		checkScriptTypeName(_str_script_type);

		if((index = getIdxScriptType(_str_script_type))==ZS_IDX_UNDEFINED){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			index=script_types->size();

			// To avoid built-int conflict bool type
			if(
					index==IDX_TYPE_BOOL_C
					|| index==IDX_TYPE_UNDEFINED
					|| index==IDX_TYPE_NULL
					|| index==IDX_TYPE_ZS_INT_C
					|| index==IDX_TYPE_ZS_FLOAT_C
			){
				properties_register_scope|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope_class = ZS_NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL, SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=ZS_MAIN_SCOPE(this)->registerSymbolScriptType(_file,_line,_str_script_type,properties_register_scope);

			sc = new ScriptType(this->zs,index, _str_script_type, scope_class,TYPE_SCRIPT_VARIABLE,_properties);
			scope_class->setScriptTypeOwner(sc);
			symbol->ref_ptr=(zs_int)sc;

			//sc->str_script_type_ptr = TYPE_SCRIPT_VARIABLE;

			script_types->push_back(sc);

			if(_base_class_name != ""){

				ScriptType *base_type=NULL;

				if(sc->idx_base_types->size() > 0){
					ScriptType *match_class=getScriptType(sc->idx_base_types->items[0]);
					ZS_THROW_RUNTIME_ERROR("Type '%s' already is inherited from '%s'"
							,_str_script_type.c_str()
							,match_class->str_script_type.c_str());
				}

				if((base_type = getScriptType(_base_class_name)) == NULL){
					ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",_base_class_name.c_str());
				}

				/*if(base_type->isStatic()){
					ZS_THROW_RUNTIME_ERROR("Class '%s' cannot extend from '%s' because is static. To allow extension register with 'bindNativeType' instead of 'registerClass'"
						,_str_script_type.c_str()
						,_base_class_name.c_str()
					);
				}*/


				// 1. extend all symbols from base type
				zs_vector<Symbol *> *symbol_functions=base_type->scope_script_type->symbol_functions;
				for(int i=0; i < symbol_functions->size(); i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->items[i];
					Symbol *symbol_dst=scope_class->registerSymbolFunction(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
							,symbol_src->n_params
							,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN|REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->overrided_symbol=symbol_src->overrided_symbol;
					symbol_dst->properties=symbol_src->properties;
				}

				// set idx starting member
				sc->idx_starting_this_member_functions=sc->scope_script_type->symbol_functions->size();

				// 1. extend all symbols from base type
				zs_vector<Symbol *> *symbol_variables=base_type->scope_script_type->symbol_variables;
				for(int i=0; i < symbol_variables->size(); i++){
					Symbol *symbol_src=(Symbol *)symbol_variables->items[i];
					Symbol *symbol_dst=scope_class->registerSymbolVariable(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->properties=symbol_src->properties;


					// attribs has to be copy MemberProperty...
					if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
						MemberProperty *mp_src=(MemberProperty *)symbol_src->ref_ptr;
						MemberProperty *mp_dst=new MemberProperty(sc,mp_src->property_name);
						mp_dst->metamethod_members.getter=mp_src->metamethod_members.getter;
						mp_dst->metamethod_members.post_inc=mp_src->metamethod_members.post_inc;
						mp_dst->metamethod_members.post_dec=mp_src->metamethod_members.post_dec;
						mp_dst->metamethod_members.pre_inc=mp_src->metamethod_members.pre_inc;
						mp_dst->metamethod_members.pre_dec=mp_src->metamethod_members.pre_dec;
						mp_dst->metamethod_members.neg=mp_src->metamethod_members.neg;
						mp_dst->metamethod_members.bwc=mp_src->metamethod_members.bwc;

						const ByteCodeMetamethod *it_setters=MetamethodMembers::byte_code_metamethod_member_setter_list;

						while(*it_setters!=0){
							MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setters);
							for(int j=0; j < mp_info.setters->size();j++){
								mp_dst->metamethod_members.addSetter(*it_setters,(Symbol *)(((StackElement *)mp_info.setters->items[j])->value));

							}
							it_setters++;
						}

						sc->allocated_member_properties->push_back(mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				sc->idx_starting_this_member_variables=sc->scope_script_type->symbol_variables->size();

				// 2. set idx base type...
				sc->idx_base_types->push_back(base_type->idx_script_type);
			}

			if(sc->idx_script_type != IDX_TYPE_CLASS_MAIN){ // main type has no field initializers and reserve first function as main function
				zs_string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sc->registerMemberFunction(
					zs_strutils::format(
						"@__field_initializer_%s_@__"
						,sc->str_script_type.c_str()
					)
				);

				sc->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;

				// create new scope
				sc->sf_field_initializer->scope_script_function=(((zs)->getScopeFactory()))->newScope(
						sc->sf_field_initializer->idx_script_function
						,scope_class
						,SCOPE_PROPERTY_IS_SCOPE_FUNCTION
				);
			}

			return sc;
		}else{
			ZS_THROW_RUNTIME_ERROR("Type '%s' already registered",_str_script_type.c_str());
		}
		return NULL;
	}

	zs_vector<ScriptType *> * ScriptTypeFactory::getScriptTypes(){
		return script_types;
	}

	ScriptType 	* ScriptTypeFactory::getScriptType(short _idx_script_type){
		if(_idx_script_type == ZS_IDX_UNDEFINED){
			ZS_THROW_RUNTIME_ERRORF("ScriptType node out of bound");
			return NULL;
		}
		return (ScriptType *)script_types->get(_idx_script_type);
	}

	ScriptType *ScriptTypeFactory::getScriptType(const zs_string & _type_name){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->str_script_type){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	ScriptType *ScriptTypeFactory::getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name_ptr == sc->str_script_type_ptr){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}


	short ScriptTypeFactory::getIdxScriptType(const zs_string & _type_name){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->str_script_type){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	short ScriptTypeFactory::getIdxScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){
		// ok check str_native_type
		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(sc->str_script_type_ptr == _type_name_ptr){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	bool ScriptTypeFactory::isScriptTypeRegistered(const zs_string & _type_name){
		return getIdxScriptType(_type_name) != ZS_IDX_UNDEFINED;
	}

	ScriptObject *		ScriptTypeFactory::instanceScriptObjectByTypeName(const zs_string & _type_name){
		 // 0. Search type info ...
		 ScriptType * rc = getScriptType(_type_name);

		 if(rc != NULL){
			 return instanceScriptObjectByTypeIdx(rc->idx_script_type);
		 }
		 return NULL;
	 }

	 ScriptObject 		 * ScriptTypeFactory::instanceScriptObjectByTypeIdx(short idx_script_type, void * value_object){

		 ScriptObject *so=NULL;

		 // 0. Search type info ...
		 ScriptType *rc = getScriptType(idx_script_type);

		 if(rc != NULL){
			 // Is a primitive ?
			switch(rc->idx_script_type){
			case IDX_TYPE_SCRIPT_OBJECT_STRING: // "String"
				so=StringScriptObject::newStringScriptObject(zs);
				break;
			case IDX_TYPE_SCRIPT_OBJECT_ARRAY: // Array []
				so=ArrayScriptObject::newArrayScriptObject(zs);
				break;
			// Object
			case IDX_TYPE_SCRIPT_OBJECT_OBJECT: //  Object {}
				so=ObjectScriptObject::newScriptObjectObject(zs);
				break;

			default:

				if(rc->idx_script_type > IDX_TYPE_SCRIPT_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ClassScriptObject::newClassScriptObject(zs,rc->idx_script_type, value_object);
				}else{
					 ZS_THROW_RUNTIME_ERROR("Internal error: An idx type was expected but it was %i",rc->idx_script_type);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }


	const char * ScriptTypeFactory::getScriptTypeName(short _idx_script_type){
		if(_idx_script_type != ZS_IDX_UNDEFINED){
			ScriptType *sc=(ScriptType *)script_types->get(_idx_script_type);
			return sc->str_script_type.c_str();
		}
		 return "type_unknow";
	}

	bool 	ScriptTypeFactory::scriptTypeInheritsFrom(short _idx_script_type,short _idx_base_type){

		if(_idx_script_type == _idx_base_type){
			return true;
		}

		ScriptType *sc=(ScriptType *)script_types->get(_idx_script_type);

		for(int i=0; i < sc->idx_base_types->size(); i++){
			if(scriptTypeInheritsFrom(sc->idx_base_types->items[i],_idx_base_type)){
				return true;
			}
		}

		return false;
	}

	bool	ScriptTypeFactory::isScriptTypeInstanceable(short idx_script_type){

		if(
				idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING
				|| idx_script_type==IDX_TYPE_SCRIPT_OBJECT_ARRAY
				|| idx_script_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT
				|| idx_script_type>=IDX_TYPE_MAX
		){
			return ((ScriptType *)script_types->get(idx_script_type))->isNonInstantiable()==false;
		}

		return false;
	}

	ScriptTypeFactory::~ScriptTypeFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for(int i = 0; i < script_types->size(); i++) {

			delete (ScriptType *)script_types->get(i);
		}
		script_types->clear();

		delete script_types;

		if(stk_constants != NULL){

			for(auto it=stk_constants->begin(); !it.end();it.next()){
				StackElement *stk=(StackElement *)(it.value);
				if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
					delete (StringScriptObject *)stk->value;
				}
				delete stk;
			}
			stk_constants->clear();
			delete stk_constants;

		}

		/*if(stk_objects != NULL){

			for(auto it=stk_objects->begin(); !it.end();it.next()){
				StackElement *stk=(StackElement *)(it.value);
				delete (ScriptObject *)stk->value;
				delete stk;
			}
			stk_objects->clear();
			delete stk_objects;
		}*/
	}
}
