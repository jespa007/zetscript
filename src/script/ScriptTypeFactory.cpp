/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(type_class, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(ZS_STR(type_class));


#define SCF_REGISTER_CLASS(name_class, type_class, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(idx_script_type >= IDX_TYPE_MAX){\
		PRAGMA_POP\
		THROW_RUNTIME_ERROR("The type to bind '%s' should be a built in type",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(name_class,type_class##Wrap_New,type_class##Wrap_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(type_class, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	bindType<type_class>(ZS_STR(type_class));

#define SCF_REGISTER_NATIVE_TYPE(type, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(ZS_STR(type),"",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);\
		sc->scope_script_type->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->str_script_type_ptr=(typeid(type).name());\
	}

#define SCF_REGISTER_NATIVE_CUSTOM_TYPE(__name__, type, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(__name__);\
		sc->scope_script_type->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->str_script_type_ptr=(typeid(type).name());\
	}

#define SCF_REGISTER_TYPE(str_type, idx_script_type)\
	if(script_types->count!=idx_script_type){\
		THROW_RUNTIME_ERROR("Error initializing built in type: '%s'",str_type);\
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

	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectString)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorString)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectVector)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorVector)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectClass)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectVarRef)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectMemberFunction)

	ScriptTypeFactory::ScriptTypeFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		main_function=NULL;
		main_object=NULL;
		script_types=new zs_vector;
		main_object=NULL;
		main_function=NULL;
		idx_clear_checkpoint=0;
	}

	void ScriptTypeFactory::init(){
		zs_string error;
		// ScriptFunctionFactory has to be created
		main_object=registerScriptType(MAIN_SCRIPT_CLASS_NAME); // 0
		MAIN_SCOPE(this)->script_type_owner=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(
				MAIN_SCRIPT_FUNCTION_NAME
		);
		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;
		main_function->scope_script_function=MAIN_SCOPE(this);

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

	zs_int parseInteger(ZetScript *_zs,zs_float *number){
		ZS_UNUSUED_PARAM(_zs);
		return (zs_int)(*number);
	}

	bool isNumber(ZetScript *_zs,StackElement *_stk){
		ZS_UNUSUED_PARAM(_zs);
		return (_stk->properties & (STK_PROPERTY_ZS_INT | STK_PROPERTY_ZS_FLOAT)) != 0;
	}

	zs_int parseInteger(ZetScript *_zs,zs_string  *number_str){
		ZS_UNUSUED_PARAM(_zs);
		zs_int result=0;
		zs_int *result_ptr=zs_strutils::parse_zs_int(*number_str);
		if(result_ptr!=NULL){
			result=*result_ptr;
			delete result_ptr;
		}
		return result;
	}


	void ScriptTypeFactory::registerSystem(){

		// !!!
		// !!! START REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		// primitives
		SCF_REGISTER_TYPE(ZS_TYPE_NAME_UNDEFINED,IDX_TYPE_UNDEFINED);
		SCF_REGISTER_TYPE(ZS_TYPE_NAME_NULL,IDX_TYPE_NULL);
		SCF_REGISTER_NATIVE_TYPE(void,IDX_TYPE_VOID_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_INT,zs_int,IDX_TYPE_ZS_INT_C);
		SCF_REGISTER_NATIVE_TYPE(zs_int *,IDX_TYPE_ZS_INT_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(char *,IDX_TYPE_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string,IDX_TYPE_ZS_STRING_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string *,IDX_TYPE_ZS_STRING_PTR_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_BOOL,bool,IDX_TYPE_BOOL_C);
		SCF_REGISTER_NATIVE_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_FLOAT,zs_float,IDX_TYPE_ZS_FLOAT_C);
		SCF_REGISTER_NATIVE_TYPE(zs_float *,IDX_TYPE_ZS_FLOAT_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(const zs_float *,IDX_TYPE_CONST_ZS_FLOAT_PTR_C);

		// estructures
		SCF_REGISTER_STRUCT(StackElement,IDX_TYPE_STACK_ELEMENT);


		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		SCF_REGISTER_SINGLETON_CLASS(ScriptFunction,IDX_TYPE_FUNCTION);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_VAR_REF,ScriptObjectVarRef,IDX_TYPE_SCRIPT_OBJECT_VAR_REF);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER,ScriptObjectMemberFunction,IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_STRING,ScriptObjectString,IDX_TYPE_SCRIPT_OBJECT_STRING);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_VECTOR,ScriptObjectVector,IDX_TYPE_SCRIPT_OBJECT_VECTOR);

		// Script object iterators
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_STRING,ScriptObjectIteratorString,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_VECTOR,ScriptObjectIteratorVector,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT,ScriptObjectIteratorObject,IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_OBJECT,ScriptObjectObject,IDX_TYPE_SCRIPT_OBJECT_OBJECT);
		SCF_REGISTER_CLASS("Class",ScriptObjectClass,IDX_TYPE_SCRIPT_OBJECT_CLASS);
		// it needs script object type to have zetscript reference
		SCF_REGISTER_SINGLETON_CLASS(ZetScript,IDX_TYPE_SCRIPT_OBJECT_CLASS_ZETSCRIPT);
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
		zs->bindFunction("parseFloat",static_cast<zs_float (*)(ZetScript *,zs_int)>(parseFloat));
		zs->bindFunction("parseFloat",static_cast<zs_float (*)(ZetScript *,zs_string *)>(parseFloat));
		zs->bindFunction("parseInteger",static_cast<zs_int (*)(ZetScript *,zs_float *)>(parseInteger));
		zs->bindFunction("parseInteger",static_cast<zs_int (*)(ZetScript *,zs_string *)>(parseInteger));
		zs->bindFunction("isNumber",isNumber);

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// Vector
		bindMemberFunction<ScriptObjectVector>("push",&ScriptObjectVectorWrap_push);
		bindMemberFunction<ScriptObjectVector>("pop",&ScriptObjectVectorWrap_pop);
		bindMemberFunction<ScriptObjectVector>("insertAt",&ScriptObjectVectorWrap_insertAt);
		bindMemberFunction<ScriptObjectVector>("eraseAt",&ScriptObjectVectorWrap_eraseAt);
		bindMemberFunction<ScriptObjectVector>("clear",&ScriptObjectVectorWrap_clear);
		bindMemberFunction<ScriptObjectVector>("size",&ScriptObjectVectorWrap_size);
		bindMemberFunction<ScriptObjectVector>("join",&ScriptObjectVectorWrap_join);
		bindMemberFunction<ScriptObjectVector>("iter",&ScriptObjectVectorWrap_iter);
		bindMemberFunctionStatic<ScriptObjectVector>("equal",&ScriptObjectVectorWrap_equal);

		// IteratorVector
		bindMemberFunction<ScriptObjectIteratorVector>("_post_inc",ScriptObjectIteratorVectorWrap_next);
		bindMemberFunction<ScriptObjectIteratorVector>("end",ScriptObjectIteratorVectorWrap_end);
		bindMemberFunction<ScriptObjectIteratorVector>("get",ScriptObjectIteratorVectorWrap_get);


		//---------------------------------------------
		// String
		bindMemberFunctionStatic<ScriptObjectString>("formatNative",ScriptObjectString::format);
		bindMemberFunction<ScriptObjectString>("eraseAt",ScriptObjectStringWrap_eraseAt);
		bindMemberFunction<ScriptObjectString>("insertAt",ScriptObjectStringWrap_insertAt);
		bindMemberFunction<ScriptObjectString>("clear",ScriptObjectStringWrap_clear);
		bindMemberFunction<ScriptObjectString>("replace",ScriptObjectStringWrap_replace);
		bindMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ZetScript *_zs,ScriptObjectString *so, zs_string *)>(ScriptObjectStringWrap_split));
		bindMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ZetScript *_zs,ScriptObjectString *so, zs_int )>(ScriptObjectStringWrap_split));
		bindMemberFunction<ScriptObjectString>("size",ScriptObjectStringWrap_size);
		bindMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ZetScript *_zs,ScriptObjectString *so, zs_string *)>(&ScriptObjectStringWrap_contains));
		bindMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ZetScript *_zs,ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_contains));

		bindMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,ScriptObjectString *so, zs_string *)>(&ScriptObjectStringWrap_indexOf));
		bindMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_indexOf));

		bindMemberFunction<ScriptObjectString>("startsWith",ScriptObjectStringWrap_startsWith);
		bindMemberFunction<ScriptObjectString>("endsWith",ScriptObjectStringWrap_endsWith);
		bindMemberFunction<ScriptObjectString>("substring",ScriptObjectStringWrap_substring);
		bindMemberFunction<ScriptObjectString>("append",ScriptObjectStringWrap_append);

		bindMemberFunction<ScriptObjectString>("iter",ScriptObjectStringWrap_iter);

		// IteratorString
		bindMemberFunction<ScriptObjectIteratorString>("_post_inc",ScriptObjectIteratorStringWrap_next);
		bindMemberFunction<ScriptObjectIteratorString>("end",ScriptObjectIteratorStringWrap_end);
		bindMemberFunction<ScriptObjectIteratorString>("get",ScriptObjectIteratorStringWrap_get);

		//---------------------------------------------
		// Object
		bindMemberFunctionStatic<ScriptObjectObject>("clear",&ScriptObjectObjectWrap_clear);
		bindMemberFunctionStatic<ScriptObjectObject>("erase",&ScriptObjectObjectWrap_erase);
		bindMemberFunctionStatic<ScriptObjectObject>("contains",&ScriptObjectObjectWrap_contains);
		bindMemberFunctionStatic<ScriptObjectObject>("append",&ScriptObjectObjectWrap_append);
		bindMemberFunctionStatic<ScriptObjectObject>("concat",ScriptObjectObject::concat);
		bindMemberFunctionStatic<ScriptObjectObject>("keys",ScriptObjectObjectWrap_keys);
		bindMemberFunctionStatic<ScriptObjectObject>("iter",ScriptObjectObjectWrap_iter);

		// IteratorObject
		bindMemberFunction<ScriptObjectIteratorObject>("_post_inc",ScriptObjectIteratorObjectWrap_next);
		bindMemberFunction<ScriptObjectIteratorObject>("end",ScriptObjectIteratorObjectWrap_end);
		bindMemberFunction<ScriptObjectIteratorObject>("get",ScriptObjectIteratorObjectWrap_get);

		zs->saveState();
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------
	// REGISTER CONSTANTS
	//

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, int value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		symbol_variable->ref_ptr=(zs_int)malloc(sizeof(StackElement));
		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		stk->value=value;
		stk->properties=STK_PROPERTY_ZS_INT|STK_PROPERTY_READ_ONLY;
		symbol_variable->properties|=SYMBOL_PROPERTY_ALLOCATED_STK;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, bool value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		symbol_variable->ref_ptr=(zs_int)malloc(sizeof(StackElement));
		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		stk->value=value;
		stk->properties=STK_PROPERTY_BOOL|STK_PROPERTY_READ_ONLY;
		symbol_variable->properties|=SYMBOL_PROPERTY_ALLOCATED_STK;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, zs_float value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		symbol_variable->ref_ptr=(zs_int)malloc(sizeof(StackElement));
		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		ZS_FLOAT_COPY(&stk->value,&value);
		stk->properties=STK_PROPERTY_ZS_FLOAT|STK_PROPERTY_READ_ONLY;
		symbol_variable->properties|=SYMBOL_PROPERTY_ALLOCATED_STK;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, const zs_string & v, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		symbol_variable->ref_ptr=(zs_int)malloc(sizeof(StackElement));
		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		*stk=*(zs->registerStkStringObject(var_name,v));
		symbol_variable->properties|=SYMBOL_PROPERTY_ALLOCATED_STK;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, const char * v, const char *registered_file, short registered_line){
		registerConstantVariable(var_name, zs_string(v), registered_file, registered_line);
	}

	// REGISTER CONSTANTS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	void ScriptTypeFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_types->count-1;
			v > idx_start; // avoid main type
			v--
		){
			ScriptType * sc = (ScriptType *)script_types->get(v);
			delete sc;
			script_types->pop_back();
		}
	}

	void ScriptTypeFactory::saveState(){
		idx_clear_checkpoint = script_types->count-1;
	}

	void ScriptTypeFactory::checkScriptTypeName(const zs_string & _str_script_type){

		if(script_types->count>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(_str_script_type.empty()){
			THROW_RUNTIME_ERRORF("Class name empty");
		}

		if(zs->getScriptFunctionFactory()->getScriptFunctions()->count > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(
					_str_script_type
					,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN
			))!=NULL){
				THROW_RUNTIME_ERROR("Type name '%s' is already defined %s"
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
			index=script_types->count;

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
			Scope * scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL, SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=MAIN_SCOPE(this)->registerSymbolScriptType(_file,_line,_str_script_type,properties_register_scope);

			sc = new ScriptType(this->zs,index, _str_script_type, scope,TYPE_SCRIPT_VARIABLE,_properties);
			scope->setScriptTypeOwner(sc);
			symbol->ref_ptr=(zs_int)sc;

			//sc->str_script_type_ptr = TYPE_SCRIPT_VARIABLE;

			script_types->push_back((zs_int)sc);

			if(_base_class_name != ""){

				ScriptType *base_type=NULL;

				if(sc->idx_base_types->count > 0){
					ScriptType *match_class=getScriptType(sc->idx_base_types->items[0]);
					THROW_RUNTIME_ERROR("Type '%s' already is inherited from '%s'"
							,_str_script_type.c_str()
							,match_class->str_script_type.c_str());
				}

				if((base_type = getScriptType(_base_class_name)) == NULL){
					THROW_RUNTIME_ERROR("Type '%s' not registered",_base_class_name.c_str());
				}

				/*if(base_type->isStatic()){
					THROW_RUNTIME_ERROR("Class '%s' cannot extend from '%s' because is static. To allow extension register with 'bindNativeType' instead of 'registerClass'"
						,_str_script_type.c_str()
						,_base_class_name.c_str()
					);
				}*/


				// 1. extend all symbols from base type
				zs_vector *symbol_functions=base_type->scope_script_type->symbol_functions;
				for(int i=0; i < symbol_functions->count; i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->items[i];
					Symbol *symbol_dst=scope->registerSymbolFunction(
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
				sc->idx_starting_this_member_functions=sc->scope_script_type->symbol_functions->count;

				// 1. extend all symbols from base type
				zs_vector *symbol_variables=base_type->scope_script_type->symbol_variables;
				for(int i=0; i < symbol_variables->count; i++){
					Symbol *symbol_src=(Symbol *)symbol_variables->items[i];
					Symbol *symbol_dst=scope->registerSymbolVariable(
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

						const ByteCodeMetamethod *it_setters=MetamethodMembers::byte_code_metamethod_member_setter_list;

						while(*it_setters!=0){
							MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setters);
							for(int j=0; j < mp_info.setters->count;j++){
								mp_dst->metamethod_members.addSetter(*it_setters,(Symbol *)(((StackElement *)mp_info.setters->items[j])->value));

							}
							it_setters++;
						}

						sc->allocated_member_properties->push_back((zs_int)mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				sc->idx_starting_this_member_variables=sc->scope_script_type->symbol_variables->count;

				// 2. set idx base type...
				sc->idx_base_types->push_back(base_type->idx_script_type);
			}

			if(sc->idx_script_type != IDX_TYPE_CLASS_MAIN){ // main type has no field initializers and reserve first function as main function
				zs_string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sc->registerMemberFunction(
					zs_strutils::format("__@field_initializer_%s_@__",sc->str_script_type.c_str())
				);

				sc->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;
			}

			return sc;
		}else{
			THROW_RUNTIME_ERROR("Type '%s' already registered",_str_script_type.c_str());
		}
		return NULL;
	}

	zs_vector * ScriptTypeFactory::getScriptTypes(){
		return script_types;
	}

	ScriptType 	* ScriptTypeFactory::getScriptType(short _idx_script_type){
		if(_idx_script_type == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERRORF("ScriptType node out of bound");
			return NULL;
		}
		return (ScriptType *)script_types->get(_idx_script_type);
	}

	ScriptType *ScriptTypeFactory::getScriptType(const zs_string & _type_name){

		for(int i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->str_script_type){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	ScriptType *ScriptTypeFactory::getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){

		for(int i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name_ptr == sc->str_script_type_ptr){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}


	short ScriptTypeFactory::getIdxScriptType(const zs_string & _type_name){

		for(int i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->str_script_type){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	short ScriptTypeFactory::getIdxScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){
		// ok check str_native_type
		for(int i = 0; i < script_types->count; i++){
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
				so=ScriptObjectString::newScriptObjectString(zs);
				break;
			case IDX_TYPE_SCRIPT_OBJECT_VECTOR: // Vector []
				so=ScriptObjectVector::newScriptObjectVector(zs);
				break;
			// Object
			case IDX_TYPE_SCRIPT_OBJECT_OBJECT: //  Object {}
				so=ScriptObjectObject::newScriptObjectObject(zs);
				break;

			default:

				if(rc->idx_script_type > IDX_TYPE_SCRIPT_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ScriptObjectClass::newScriptObjectClass(zs,rc->idx_script_type, value_object);
				}else{
					 THROW_RUNTIME_ERROR("Internal error: An idx type was expected but it was %i",rc->idx_script_type);
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

		for(int i=0; i < sc->idx_base_types->count; i++){
			if(scriptTypeInheritsFrom(sc->idx_base_types->items[i],_idx_base_type)){
				return true;
			}
		}

		return false;
	}

	bool	ScriptTypeFactory::isScriptTypeInstanceable(short idx_script_type){

		if(
				idx_script_type==IDX_TYPE_SCRIPT_OBJECT_STRING
				|| idx_script_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR
				|| idx_script_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT
				|| idx_script_type>=IDX_TYPE_MAX
		){
			return ((ScriptType *)script_types->get(idx_script_type))->isNonInstantiable()==false;
		}

		return false;
	}

	ScriptTypeFactory::~ScriptTypeFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (int i = 0; i < script_types->count; i++) {

			delete (ScriptType *)script_types->get(i);
		}
		script_types->clear();

		delete script_types;
	}
}
