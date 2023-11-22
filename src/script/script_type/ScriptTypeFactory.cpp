/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(type_class, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerType<type_class>(ZS_STR(type_class));


#define SCF_REGISTER_CLASS(name_class, type_class, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(id >= ScriptTypeId::SCRIPT_TYPE_ID_MAX){\
		PRAGMA_POP\
		ZS_THROW_RUNTIME_ERROR("The type to bind '%s' should be a built in type",ZS_STR(type_class));\
		return;\
	}\
	registerType<type_class>(name_class,type_class##_New,type_class##_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(type_class, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerType<type_class>(ZS_STR(type_class));

#define SCF_REGISTER_SINGLETON_NAME_CLASS(name, type_class, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerType<type_class>(name);

#define SCF_REGISTER_NATIVE_TYPE(type, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(ZS_STR(type),"",ZS_SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);\
		sc->scope->properties|=ZS_SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->native_name=(typeid(type).name());\
	}

#define SCF_REGISTER_NATIVE_CUSTOM_TYPE(__name__, type, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerScriptType(__name__);\
		sc->scope->properties|=ZS_SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->native_name=(typeid(type).name());\
	}

#define SCF_REGISTER_TYPE(name, id)\
	if(script_types->size()!=id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing built in type: '%s'",name);\
		return;\
	}else{\
		registerScriptType(name);\
	}

namespace zetscript{


	const char * k_str_void_type =typeid(void).name();
	const char * k_str_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_float_type_ptr=typeid(zs_float *).name();
	const char * k_str_const_float_type_ptr=typeid(const zs_float *).name();
	const char * k_str_string_type_ptr=typeid(zs_string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	const char * k_str_int_type=typeid(zs_int).name();
	const char * k_str_float_type=typeid(zs_float).name();
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
		main_function->scope=ZS_MAIN_SCOPE(this);

		idx_clear_checkpoint=1; // by default restore till main type.
	}

	ZetScript *ptrToZetScriptPtr(ZetScript *_zs, zs_int ptr){
		ZS_UNUSUED_PARAM(_zs);
		return (ZetScript *)ptr;
	}

	zs_int intMaxValue(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return INTPTR_MAX;
	}

	zs_float floatMaxValue(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return FLT_MAX;
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
		zs_float *result_ptr=zs_strutils::parse_float(*number_str);

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
		zs_int *result_ptr=zs_strutils::parse_int(*number_str);
		if(result_ptr!=NULL){
			result=*result_ptr;
			delete result_ptr;
		}
		return result;
	}

	bool isNumber(ZetScript *_zs,StackElement *_stk){
		ZS_UNUSUED_PARAM(_zs);
		return (_stk->properties & (STACK_ELEMENT_PROPERTY_INT | STACK_ELEMENT_PROPERTY_FLOAT)) != 0;
	}

	void ScriptTypeFactory::setup(){

		// !!!
		// !!! START REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		// primitives
		SCF_REGISTER_TYPE(ZS_TYPE_NAME_UNDEFINED,ScriptTypeId::SCRIPT_TYPE_ID_UNDEFINED);
		SCF_REGISTER_TYPE(ZS_TYPE_NAME_NULL,ScriptTypeId::SCRIPT_TYPE_ID_NULL);
		SCF_REGISTER_NATIVE_TYPE(void,ScriptTypeId::SCRIPT_TYPE_ID_VOID_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_INT,zs_int,ScriptTypeId::SCRIPT_TYPE_ID_INT_C);
		SCF_REGISTER_NATIVE_TYPE(zs_int *,ScriptTypeId::SCRIPT_TYPE_ID_INT_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(char *,ScriptTypeId::SCRIPT_TYPE_ID_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(const char *,ScriptTypeId::SCRIPT_TYPE_ID_CONST_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string,ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string *,ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_PTR_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_BOOL,bool,ScriptTypeId::SCRIPT_TYPE_ID_BOOL_C);
		SCF_REGISTER_NATIVE_TYPE(bool *,ScriptTypeId::SCRIPT_TYPE_ID_BOOL_PTR_C);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(ZS_TYPE_NAME_FLOAT,zs_float,ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C);
		SCF_REGISTER_NATIVE_TYPE(zs_float *,ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(const zs_float *,ScriptTypeId::SCRIPT_TYPE_ID_CONST_FLOAT_PTR_C);

		// estructures
		SCF_REGISTER_STRUCT(StackElement,ScriptTypeId::SCRIPT_TYPE_ID_STACK_ELEMENT);
		SCF_REGISTER_STRUCT(ContainerSlot,ScriptTypeId::SCRIPT_TYPE_ID_CONTAINER_SLOT);


		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_FUNCTION,ScriptFunction,ScriptTypeId::SCRIPT_TYPE_ID_FUNCTION);
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_VAR_REF,VarRefScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_VAR_REF);
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER,MemberFunctionScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_FUNCTION_MEMBER);
		SCF_REGISTER_SINGLETON_CLASS(ZetScript,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ZETSCRIPT);

		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_STRING,StringScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING);

		// Script object iterators
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_ASSIGNRING,StringIteratorScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_STRING);
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_ARRAY,ArrayIteratorScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_ARRAY);
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT,ObjectIteratorScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_OBJECT);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_ARRAY,ArrayScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ARRAY);
		SCF_REGISTER_CLASS(ZS_TYPE_NAME_OBJECT_OBJECT,ObjectScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_OBJECT);
		SCF_REGISTER_SINGLETON_NAME_CLASS(ZS_TYPE_NAME_OBJECT_CLASS,ClassScriptObject,ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_CLASS);
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

		zs->registerFunction("ptrToZetScriptPtr",ptrToZetScriptPtr);

		ScriptType *integer_type=this->getScriptType(ZS_TYPE_NAME_INT);
		integer_type->registerConstMemberProperty("MAX_VALUE",intMaxValue);
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_float *)>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_int )>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_string *)>(parseInt));

		ScriptType *float_type=this->getScriptType(ZS_TYPE_NAME_FLOAT);
		float_type->registerConstMemberProperty("MAX_VALUE",floatMaxValue);
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_int )>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_float *)>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_string *)>(parseFloat));


		// Global symbols
		zs->registerFunction("isNumber",isNumber);

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// String

		// properties
		registerMemberPropertyMetamethod<StringScriptObject>("length","_get",StringScriptObjectZs_length);

		// members
		registerStaticMemberFunction<StringScriptObject>("formatNative",StringScriptObject::format);
		registerMemberFunction<StringScriptObject>("eraseAt",StringScriptObjectZs_eraseAt);
		registerMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringScriptObject *so, zs_int, zs_int )>(StringScriptObjectZs_insertAt));
		registerMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringScriptObject *so, zs_int, zs_string *)>(StringScriptObjectZs_insertAt));
		registerMemberFunction<StringScriptObject>("clear",StringScriptObjectZs_clear);
		registerMemberFunction<StringScriptObject>("replace",StringScriptObjectZs_replace);
		registerMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(StringScriptObjectZs_split));
		registerMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(StringScriptObjectZs_split));
		registerMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(&StringScriptObjectZs_contains));
		registerMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_contains));

		registerMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringScriptObject *so, zs_string *)>(&StringScriptObjectZs_indexOf));
		registerMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_indexOf));

		registerMemberFunction<StringScriptObject>("startsWith",StringScriptObjectZs_startsWith);
		registerMemberFunction<StringScriptObject>("endsWith",StringScriptObjectZs_endsWith);
		registerMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int,zs_int )>(&StringScriptObjectZs_subassignring));
		registerMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ZetScript *_zs,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_subassignring));
		registerMemberFunction<StringScriptObject>("append",static_cast<void (*)(ZetScript *,StringScriptObject *, StringScriptObject *)>(&StringScriptObjectZs_append));
		registerMemberFunction<StringScriptObject>("append",static_cast<void (*)(ZetScript *,StringScriptObject *, zs_int )>(&StringScriptObjectZs_append));
		registerMemberFunction<StringScriptObject>("toLowerCase",StringScriptObjectZs_toLowerCase);
		registerMemberFunction<StringScriptObject>("toUpperCase",StringScriptObjectZs_toUpperCase);

		registerMemberFunction<StringScriptObject>("_iter",StringScriptObjectZs_iter);

		// IteratorString
		registerMemberFunction<StringIteratorScriptObject>("_next",StringIteratorScriptObjectZs_next);
		registerMemberFunction<StringIteratorScriptObject>("_end",StringIteratorScriptObjectZs_end);
		registerMemberFunction<StringIteratorScriptObject>("_get",StringIteratorScriptObjectZs_get);

		//---------------------------------------------
		// Array

		// properties
		registerMemberPropertyMetamethod<ArrayScriptObject>("length","_get",&ArrayScriptObjectZs_length);

		// members
		registerMemberFunction<ArrayScriptObject>("push",&ArrayScriptObjectZs_push);
		registerMemberFunction<ArrayScriptObject>("pop",&ArrayScriptObjectZs_pop);
		registerMemberFunction<ArrayScriptObject>("insertAt",&ArrayScriptObjectZs_insertAt);
		registerMemberFunction<ArrayScriptObject>("eraseAt",&ArrayScriptObjectZs_eraseAt);
		registerMemberFunction<ArrayScriptObject>("clear",&ArrayScriptObjectZs_clear);
		registerMemberFunction<ArrayScriptObject>("join",&ArrayScriptObjectZs_join);
		registerMemberFunction<ArrayScriptObject>("extend",&ArrayScriptObjectZs_extend);
		registerStaticMemberFunction<ArrayScriptObject>("concat",&ArrayScriptObjectZs_concat);
		registerMemberFunction<ArrayScriptObject>("contains",&ArrayScriptObjectZs_contains);

		registerMemberFunction<ArrayScriptObject>("_iter",&ArrayScriptObjectZs_iter);
		registerStaticMemberFunction<ArrayScriptObject>("_equ",&ArrayScriptObjectZs_equal);
		//registerStaticMemberFunction<ArrayScriptObject>("_add",&ArrayScriptObjectZs_equal_add);

		// IteratorArray
		registerMemberFunction<ArrayIteratorScriptObject>("_next",ArrayIteratorScriptObjectZs_next);
		registerMemberFunction<ArrayIteratorScriptObject>("_end",ArrayIteratorScriptObjectZs_end);
		registerMemberFunction<ArrayIteratorScriptObject>("_get",ArrayIteratorScriptObjectZs_get);


		//---------------------------------------------
		// Object
		registerStaticMemberFunction<ObjectScriptObject>("clear",&ObjectScriptObjectZs_clear);
		registerStaticMemberFunction<ObjectScriptObject>("erase",&ObjectScriptObjectZs_erase);
		registerStaticMemberFunction<ObjectScriptObject>("contains",&ObjectScriptObjectZs_contains);
		registerStaticMemberFunction<ObjectScriptObject>("extend",&ObjectScriptObjectZs_extend);
		registerStaticMemberFunction<ObjectScriptObject>("concat",ObjectScriptObject::concat);
		registerStaticMemberFunction<ObjectScriptObject>("keys",ObjectScriptObjectZs_keys);
		registerStaticMemberFunction<ObjectScriptObject>("_iter",ObjectScriptObjectZs_iter);

		// IteratorObject
		registerMemberFunction<ObjectIteratorScriptObject>("_next",ObjectIteratorScriptObjectZs_next);
		registerMemberFunction<ObjectIteratorScriptObject>("_end",ObjectIteratorScriptObjectZs_end);
		registerMemberFunction<ObjectIteratorScriptObject>("_get",ObjectIteratorScriptObjectZs_get);

		zs->saveState();
	}

	void ScriptTypeFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_UNDEFINED_IDX ?  idx_clear_checkpoint:_idx_start;
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
							zs_strutils::format("at " ZS_FORMAT_FILE_LINE
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

		if((index = getScriptTypeId(_str_script_type))==ZS_UNDEFINED_IDX){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			index=script_types->size();

			// To avoid built-int conflict bool type
			if(
					index==ScriptTypeId::SCRIPT_TYPE_ID_BOOL_C
					|| index==ScriptTypeId::SCRIPT_TYPE_ID_UNDEFINED
					|| index==ScriptTypeId::SCRIPT_TYPE_ID_NULL
					|| index==ScriptTypeId::SCRIPT_TYPE_ID_INT_C
					|| index==ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C
			){
				properties_register_scope|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope_class = ZS_NEW_SCOPE(this,ZS_UNDEFINED_IDX,NULL, ZS_SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=ZS_MAIN_SCOPE(this)->registerSymbolScriptType(_file,_line,_str_script_type,properties_register_scope);

			sc = new ScriptType(this->zs,index, _str_script_type, scope_class,ZS_TYPE_NAME_SCRIPT_OBJECT,_properties);
			scope_class->setScriptTypeOwner(sc);
			symbol->ref_ptr=(zs_int)sc;

			//sc->native_name = ZS_TYPE_NAME_SCRIPT_OBJECT;

			script_types->push_back(sc);

			if(_base_class_name != ""){

				ScriptType *base_type=NULL;

				if(sc->idx_base_types->size() > 0){
					ScriptType *match_class=getScriptType(sc->idx_base_types->get(0));
					ZS_THROW_RUNTIME_ERROR("Type '%s' already is inherited from '%s'"
							,_str_script_type.c_str()
							,match_class->name.c_str());
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
				zs_vector<Symbol *> *symbol_functions=base_type->scope->symbol_functions;
				for(int i=0; i < symbol_functions->size(); i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->get(i);
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
				sc->idx_starting_this_member_functions=sc->scope->symbol_functions->size();

				// 1. extend all symbols from base type
				zs_vector<Symbol *> *symbol_variables=base_type->scope->symbol_variables;
				for(int i=0; i < symbol_variables->size(); i++){
					Symbol *symbol_src=(Symbol *)symbol_variables->get(i);
					Symbol *symbol_dst=scope_class->registerSymbolVariable(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->properties=symbol_src->properties;


					// attribs has to be copy MemberProperty...
					if(symbol_src->properties & ZS_SYMBOL_PROPERTY_MEMBER_PROPERTY){
						MemberProperty *mp_src=(MemberProperty *)symbol_src->ref_ptr;
						MemberProperty *mp_dst=new MemberProperty(sc,mp_src->property_name);
						mp_dst->metamethod_members.getter=mp_src->metamethod_members.getter;
						mp_dst->metamethod_members.postinc=mp_src->metamethod_members.postinc;
						mp_dst->metamethod_members.postdec=mp_src->metamethod_members.postdec;
						mp_dst->metamethod_members.preinc=mp_src->metamethod_members.preinc;
						mp_dst->metamethod_members.predec=mp_src->metamethod_members.predec;
						mp_dst->metamethod_members.neg=mp_src->metamethod_members.neg;
						mp_dst->metamethod_members.bwc=mp_src->metamethod_members.bwc;

						const Metamethod *it_setters=MetamethodMembers::member_setter_list;

						while(*it_setters!=0){
							MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setters);
							for(int j=0; j < mp_info.setters->size();j++){
								mp_dst->metamethod_members.addSetter(*it_setters,(Symbol *)(((StackElement *)mp_info.setters->get(j))->value));

							}
							it_setters++;
						}

						sc->allocated_member_properties->push_back(mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				sc->idx_starting_this_member_variables=sc->scope->symbol_variables->size();

				// 2. set idx base type...
				sc->idx_base_types->push_back(base_type->id);
			}

			if(sc->id != ScriptTypeId::SCRIPT_TYPE_ID_CLASS_MAIN){ // main type has no field initializers and reserve first function as main function
				zs_string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sc->registerMemberFunction(
					zs_strutils::format(
						"@__field_initializer_%s_@__"
						,sc->name.c_str()
					)
				);

				sc->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;

				// create new scope
				sc->sf_field_initializer->scope=(((zs)->getScopeFactory()))->newScope(
						sc->sf_field_initializer->id
						,scope_class
						,ZS_SCOPE_PROPERTY_IS_SCOPE_FUNCTION
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

	ScriptType 	* ScriptTypeFactory::getScriptType(short _idx_type){
		if(_idx_type == ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERRORF("ScriptType node out of bound");
			return NULL;
		}
		return (ScriptType *)script_types->get(_idx_type);
	}

	ScriptType *ScriptTypeFactory::getScriptType(const zs_string & _type_name){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->name){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	ScriptType *ScriptTypeFactory::getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name_ptr == sc->native_name){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}


	short ScriptTypeFactory::getScriptTypeId(const zs_string & _type_name){

		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->name){
				return i;
			}
		}
		return ZS_UNDEFINED_IDX;
	}

	short ScriptTypeFactory::getScriptTypeIdFromTypeNamePtr(const zs_string & _type_name_ptr){
		// ok check str_native_type
		for(int i = 0; i < script_types->size(); i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(sc->native_name == _type_name_ptr){
				return i;
			}
		}
		return ZS_UNDEFINED_IDX;
	}

	bool ScriptTypeFactory::isScriptTypeRegistered(const zs_string & _type_name){
		return getScriptTypeId(_type_name) != ZS_UNDEFINED_IDX;
	}

	ScriptObject *		ScriptTypeFactory::instanceScriptObjectByTypeName(const zs_string & _type_name){
		 // 0. Search type info ...
		 ScriptType * rc = getScriptType(_type_name);

		 if(rc != NULL){
			 return instanceScriptObjectByTypeIdx(rc->id);
		 }
		 return NULL;
	 }

	 ScriptObject 		 * ScriptTypeFactory::instanceScriptObjectByTypeIdx(short id, void * value_object){

		 ScriptObject *so=NULL;

		 // 0. Search type info ...
		 ScriptType *rc = getScriptType(id);

		 if(rc != NULL){
			 // Is a primitive ?
			switch(rc->id){
			case ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING: // "String"
				so=StringScriptObject::newStringScriptObject(zs);
				break;
			case ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ARRAY: // Array []
				so=ArrayScriptObject::newArrayScriptObject(zs);
				break;
			// Object
			case ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_OBJECT: //  Object {}
				so=ObjectScriptObject::newObjectScriptObject(zs);
				break;

			default:

				if(rc->id > ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ClassScriptObject::newClassScriptObject(zs,rc->id, value_object);
				}else{
					 ZS_THROW_RUNTIME_ERROR("Internal error: An idx type was expected but it was %i",rc->id);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }


	const char * ScriptTypeFactory::getScriptTypeName(short _idx_type){
		if(_idx_type != ZS_UNDEFINED_IDX){
			ScriptType *sc=(ScriptType *)script_types->get(_idx_type);
			return sc->name.c_str();
		}
		 return "type_unknow";
	}

	bool 	ScriptTypeFactory::scriptTypeInheritsFrom(short _idx_type,short _idx_base_type){

		if(_idx_type == _idx_base_type){
			return true;
		}

		ScriptType *sc=(ScriptType *)script_types->get(_idx_type);

		for(int i=0; i < sc->idx_base_types->size(); i++){
			if(scriptTypeInheritsFrom(sc->idx_base_types->get(i),_idx_base_type)){
				return true;
			}
		}

		return false;
	}

	bool	ScriptTypeFactory::isScriptTypeInstanceable(short id){

		if(
				id==ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING
				|| id==ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ARRAY
				|| id==ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_OBJECT
				|| id>=ScriptTypeId::SCRIPT_TYPE_ID_MAX
		){
			return ((ScriptType *)script_types->get(id))->isNonInstantiable()==false;
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
	}
}
