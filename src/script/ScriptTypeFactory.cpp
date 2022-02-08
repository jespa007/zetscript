/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(type_class, idx_type)\
	if(script_types->count!=idx_type){\
		THROW_RUNTIME_ERROR("Error: type built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeType<type_class>(ZS_STR(type_class));


#define SCF_REGISTER_CLASS(name_class, type_class, idx_type)\
	if(script_types->count!=idx_type){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(idx_type >= IDX_TYPE_MAX){\
		PRAGMA_POP\
		THROW_RUNTIME_ERROR("The class to register '%s' should be a built in class",ZS_STR(type_class));\
		return;\
	}\
	registerNativeClass<type_class>(name_class,type_class##Wrap_New,type_class##Wrap_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(type_class, idx_type)\
	if(script_types->count!=idx_type){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeSingletonClass<type_class>(ZS_STR(type_class));

#define SCF_REGISTER_NATIVE_TYPE(type, idx_type)\
	if(script_types->count!=idx_type){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: %s",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptType *sc=registerClass(ZS_STR(type));\
		sc->class_scope->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_TYPE_PROPERTY_C_OBJECT_REF;\
		sc->type_name_ptr=(typeid(type).name());\
	}

#define SCF_REGISTER_TYPE(str_type, idx_type)\
	if(script_types->count!=idx_type){\
		THROW_RUNTIME_ERROR("Error initializing built in type: %s",str_type);\
		return;\
	}else{\
		registerClass(str_type);\
	}

namespace zetscript{

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
		main_object=registerClass(MAIN_SCRIPT_CLASS_NAME); // 0
		MAIN_SCOPE(this)->script_type=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(
				MAIN_SCRIPT_FUNCTION_NAME
		);
		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;
		main_function->function_scope=MAIN_SCOPE(this);

		idx_clear_checkpoint=1; // by default restore till main class.
	}

	ZetScript *ptrToZetScriptPtr(zs_int ptr){
		return (ZetScript *)ptr;
	}

	zs_float parseFloat(zs_int number){

		return (zs_float)(number);
	}

	zs_float parseFloat(zs_string  *number_str){
		zs_float result=0;
		zs_float *result_ptr=zs_strutils::parse_zs_float(*number_str);

		if(result_ptr != NULL){
			result=*result_ptr;
			delete result_ptr;
		}

		return result;
	}

	zs_int parseInteger(zs_float *number){

		return (zs_int)(*number);
	}

	zs_int parseInteger(zs_string  *number_str){
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
		//SCF_REGISTER_NATIVE_TYPE(ZS_TYPE_NAME_NULL,IDX_TYPE_NULL);
		SCF_REGISTER_TYPE(ZS_TYPE_NAME_NULL,IDX_TYPE_NULL);
		SCF_REGISTER_NATIVE_TYPE(void,IDX_TYPE_VOID_C);
		SCF_REGISTER_NATIVE_TYPE(zs_int,IDX_TYPE_ZS_INT_C);
		SCF_REGISTER_NATIVE_TYPE(zs_int *,IDX_TYPE_ZS_INT_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(char *,IDX_TYPE_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string,IDX_TYPE_STRING_C);
		SCF_REGISTER_NATIVE_TYPE(zs_string *,IDX_TYPE_STRING_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(bool,IDX_TYPE_BOOL_C);
		SCF_REGISTER_NATIVE_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		SCF_REGISTER_NATIVE_TYPE(zs_float,IDX_TYPE_ZS_FLOAT_C);
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
		// it needs script object class to have zetscript reference
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

		zs->registerFunction("ptrToZetScriptPtr",ptrToZetScriptPtr);
		zs->registerFunction("parseFloat",static_cast<zs_float (*)(zs_int)>(parseFloat));
		zs->registerFunction("parseFloat",static_cast<zs_float (*)(zs_string *)>(parseFloat));
		zs->registerFunction("parseInteger",static_cast<zs_int (*)(zs_float *)>(parseInteger));
		zs->registerFunction("parseInteger",static_cast<zs_int (*)(zs_string *)>(parseInteger));

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// Vector
		registerNativeMemberFunction<ScriptObjectVector>("push",&ScriptObjectVectorWrap_push);
		registerNativeMemberFunction<ScriptObjectVector>("pop",&ScriptObjectVectorWrap_pop);
		registerNativeMemberFunction<ScriptObjectVector>("insertAt",&ScriptObjectVectorWrap_insertAt);
		registerNativeMemberFunction<ScriptObjectVector>("eraseAt",&ScriptObjectVectorWrap_eraseAt);
		registerNativeMemberFunction<ScriptObjectVector>("clear",&ScriptObjectVectorWrap_clear);
		registerNativeMemberFunction<ScriptObjectVector>("size",&ScriptObjectVectorWrap_size);
		registerNativeMemberFunction<ScriptObjectVector>("join",&ScriptObjectVectorWrap_join);
		registerNativeMemberFunction<ScriptObjectVector>("iter",&ScriptObjectVectorWrap_iter);
		registerNativeMemberFunctionStatic<ScriptObjectVector>("equal",&ScriptObjectVectorWrap_equal);

		// IteratorVector
		registerNativeMemberFunction<ScriptObjectIteratorVector>("_post_inc",ScriptObjectIteratorVectorWrap_next);
		registerNativeMemberFunction<ScriptObjectIteratorVector>("end",ScriptObjectIteratorVectorWrap_end);
		registerNativeMemberFunction<ScriptObjectIteratorVector>("get",ScriptObjectIteratorVectorWrap_get);


		//---------------------------------------------
		// String
		registerNativeMemberFunctionStatic<ScriptObjectString>("formatNative",ScriptObjectString::format);
		registerNativeMemberFunction<ScriptObjectString>("eraseAt",ScriptObjectStringWrap_eraseAt);
		registerNativeMemberFunction<ScriptObjectString>("insertAt",ScriptObjectStringWrap_insertAt);
		registerNativeMemberFunction<ScriptObjectString>("clear",ScriptObjectStringWrap_clear);
		registerNativeMemberFunction<ScriptObjectString>("replace",ScriptObjectStringWrap_replace);
		registerNativeMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ScriptObjectString *so, zs_string *)>(ScriptObjectStringWrap_split));
		registerNativeMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ScriptObjectString *so, zs_int )>(ScriptObjectStringWrap_split));
		registerNativeMemberFunction<ScriptObjectString>("size",ScriptObjectStringWrap_size);
		registerNativeMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ScriptObjectString *so, zs_string *)>(&ScriptObjectStringWrap_contains));
		registerNativeMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_contains));

		registerNativeMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ScriptObjectString *so, zs_string *)>(&ScriptObjectStringWrap_indexOf));
		registerNativeMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_indexOf));

		registerNativeMemberFunction<ScriptObjectString>("startsWith",ScriptObjectStringWrap_startsWith);
		registerNativeMemberFunction<ScriptObjectString>("endsWith",ScriptObjectStringWrap_endsWith);
		registerNativeMemberFunction<ScriptObjectString>("substring",ScriptObjectStringWrap_substring);
		registerNativeMemberFunction<ScriptObjectString>("append",ScriptObjectStringWrap_append);

		registerNativeMemberFunction<ScriptObjectString>("iter",ScriptObjectStringWrap_iter);

		// IteratorString
		registerNativeMemberFunction<ScriptObjectIteratorString>("_post_inc",ScriptObjectIteratorStringWrap_next);
		registerNativeMemberFunction<ScriptObjectIteratorString>("end",ScriptObjectIteratorStringWrap_end);
		registerNativeMemberFunction<ScriptObjectIteratorString>("get",ScriptObjectIteratorStringWrap_get);

		//---------------------------------------------
		// Object
		registerNativeMemberFunctionStatic<ScriptObjectObject>("clear",&ScriptObjectObjectWrap_clear);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("erase",&ScriptObjectObjectWrap_erase);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("contains",&ScriptObjectObjectWrap_contains);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("append",&ScriptObjectObjectWrap_append);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("concat",ScriptObjectObject::concat);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("keys",ScriptObjectObjectWrap_keys);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("iter",ScriptObjectObjectWrap_iter);

		// IteratorObject
		registerNativeMemberFunction<ScriptObjectIteratorObject>("_post_inc",ScriptObjectIteratorObjectWrap_next);
		registerNativeMemberFunction<ScriptObjectIteratorObject>("end",ScriptObjectIteratorObjectWrap_end);
		registerNativeMemberFunction<ScriptObjectIteratorObject>("get",ScriptObjectIteratorObjectWrap_get);

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

		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		stk->value=value;
		stk->properties=STK_PROPERTY_ZS_INT|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, bool value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		stk->value=value;
		stk->properties=STK_PROPERTY_BOOL|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, zs_float value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		ZS_FLOAT_COPY(&stk->value,&value);
		stk->properties=STK_PROPERTY_ZS_FLOAT|STK_PROPERTY_READ_ONLY;
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, const zs_string & v, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		*stk=*(zs->registerStkStringObject(var_name,v));
	}

	void ScriptTypeFactory::registerConstantVariable(const zs_string & var_name, const char * v, const char *registered_file, short registered_line){
		registerConstantVariable(var_name, zs_string(v), registered_file, registered_line);
	}

	// REGISTER CONSTANTS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	void ScriptTypeFactory::clear(short _idx_start){
		short idx_start = _idx_start == IDX_ZS_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_types->count-1;
			v > idx_start; // avoid main class
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

	void ScriptTypeFactory::checkClassName(const zs_string & type_name){

		if(script_types->count>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(type_name.empty()){
			THROW_RUNTIME_ERRORF("Class name empty");
		}

		if(zs->getScriptFunctionFactory()->getScriptFunctions()->count > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(type_name,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN))!=NULL){
				THROW_RUNTIME_ERROR("Class name '%s' collides with symbol defined at [%s:%i]",type_name.c_str(),main_function_symbol->file,main_function_symbol->line);
			}
		}
	}

	ScriptType * ScriptTypeFactory::registerClass(
			const zs_string & type_name
			 ,const zs_string & base_class_name
			 ,const char * file
			 , short line
	){
		int  index;
		ScriptType *sc=NULL;

		checkClassName(type_name);

		if((index = getIdxScriptType(type_name))==IDX_ZS_UNDEFINED){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			index=script_types->count;

			// To avoid built-int conflict bool type
			if(index==IDX_TYPE_BOOL_C || index==IDX_TYPE_NULL){
				properties_register_scope|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope = NEW_SCOPE(this,IDX_ZS_UNDEFINED,NULL, SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=MAIN_SCOPE(this)->registerSymbolType(file,line,type_name,properties_register_scope);

			sc = new ScriptType(this->zs,index, type_name, scope);
			scope->setScriptClass(sc);
			symbol->ref_ptr=(zs_int)sc;

			//sc->type_name_ptr = TYPE_SCRIPT_VARIABLE;

			script_types->push_back((zs_int)sc);

			if(base_class_name != ""){

				ScriptType *base_class=NULL;

				if(sc->idx_base_types->count > 0){
					ScriptType *match_class=getScriptType(sc->idx_base_types->items[0]);
					THROW_RUNTIME_ERROR("Class '%s' already is inherited from '%s'"
							,type_name.c_str()
							,match_class->type_name.c_str());
				}

				if((base_class = getScriptType(base_class_name)) == NULL){
					THROW_RUNTIME_ERROR("Class %s not registered",base_class_name.c_str());
				}

				if(base_class->isNativeSingletonClass()){
					THROW_RUNTIME_ERROR("Class '%s' cannot extend from '%s' because is singleton. To allow extension register class '%' with registerNativeClass instead of registerNativeSingletonClass",type_name.c_str(),base_class_name.c_str(),base_class_name.c_str());
				}


				// 1. extend all symbols from base class
				zs_vector *symbol_functions=base_class->class_scope->symbol_functions;
				for(int i=0; i < symbol_functions->count; i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->items[i];
					Symbol *symbol_dst=scope->registerSymbolFunction(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
							,symbol_src->n_params
							,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN|REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->properties=symbol_src->properties;

//					sci->symbol_member_functions->push_back((zs_int)symbol_src);
				}

				// set idx starting member
				sc->idx_starting_this_member_functions=sc->class_scope->symbol_functions->count;

				// 1. extend all symbols from base class
				zs_vector *symbol_variables=base_class->class_scope->symbol_variables;
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
							for(unsigned j=0; i < mp_info.setters->count;j++){
								mp_dst->metamethod_members.addSetter(*it_setters,(ScriptFunction *)(((StackElement *)mp_info.setters->items[j])->value));

							}
							it_setters++;
						}

						sc->allocated_member_properties->push_back((zs_int)mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				sc->idx_starting_this_member_variables=sc->class_scope->symbol_variables->count;

				// 2. set idx base class...
				sc->idx_base_types->push_back(base_class->idx_type);
			}

			if(sc->idx_type != IDX_TYPE_CLASS_MAIN){ // main class has no field initializers and reserve first function as main function
				zs_string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sc->registerMemberFunction(
					zs_strutils::format("__@field_initializer_%s_@__",sc->type_name.c_str())
				);

				sc->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;
			}

			return sc;
		}else{
			THROW_RUNTIME_ERROR("class '%s' already registered",type_name.c_str());
		}
		return NULL;
	}

	zs_vector * ScriptTypeFactory::getScriptTypes(){
		return script_types;
	}

	int ScriptTypeFactory::getBuiltinTypeOrClass(const zs_string & name){
		ScriptType *sc;

		if(name == ZS_TYPE_NAME_NULL){
			return IDX_TYPE_NULL;
		}else if(name == ZS_TYPE_NAME_INT){
			return IDX_TYPE_ZS_INT_C;
		}else if(name == ZS_TYPE_NAME_FLOAT){
			return IDX_TYPE_ZS_FLOAT_C;
		}else if(name == ZS_TYPE_NAME_BOOL){
			return IDX_TYPE_BOOL_C;
		}else if((sc=getScriptType(name))!=NULL){
			return sc->idx_type;
		}

		return IDX_ZS_UNDEFINED;
	}

	ScriptType 	* ScriptTypeFactory::getScriptType(short _idx_type){
		if(_idx_type == IDX_ZS_UNDEFINED){
			THROW_RUNTIME_ERRORF("ScriptType node out of bound");
			return NULL;
		}
		return (ScriptType *)script_types->get(_idx_type);
	}

	ScriptType *ScriptTypeFactory::getScriptType(const zs_string & _type_name){

		for(unsigned i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->type_name){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	ScriptType *ScriptTypeFactory::getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){

		for(unsigned i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name_ptr == sc->type_name_ptr){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}


	short ScriptTypeFactory::getIdxScriptType(const zs_string & _type_name){

		for(unsigned i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(_type_name == sc->type_name){
				return i;
			}
		}
		return IDX_ZS_UNDEFINED;
	}

	short ScriptTypeFactory::getIdxScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr){
		// ok check str_native_type
		for(unsigned i = 0; i < script_types->count; i++){
			ScriptType * sc=(ScriptType *)script_types->get(i);
			if(sc->type_name_ptr == _type_name_ptr){
				return i;
			}
		}
		return IDX_ZS_UNDEFINED;
	}

	bool ScriptTypeFactory::isScriptTypeRegistered(const zs_string & _type_name){
		return getIdxScriptType(_type_name) != IDX_ZS_UNDEFINED;
	}

	ScriptObject *		ScriptTypeFactory::instanceScriptObjectByTypeName(const zs_string & _type_name){
		 // 0. Search class info ...
		 ScriptType * rc = getScriptType(_type_name);

		 if(rc != NULL){
			 return instanceScriptObjectByTypeIdx(rc->idx_type);
		 }
		 return NULL;
	 }

	 ScriptObject 		 * ScriptTypeFactory::instanceScriptObjectByTypeIdx(short idx_type, void * value_object){

		 ScriptObject *so=NULL;

		 // 0. Search class info ...
		 ScriptType *rc = getScriptType(idx_type);

		 if(rc != NULL){
			 // Is a primitive ?
			switch(rc->idx_type){
			case IDX_TYPE_SCRIPT_OBJECT_STRING: // "String"
				so=ScriptObjectString::newScriptObjectString(zs);
				break;
			case IDX_TYPE_SCRIPT_OBJECT_VECTOR: // Vector []
				so=ScriptObjectVector::newScriptObjectVector(zs);
				break;
			// Object & class
			case IDX_TYPE_SCRIPT_OBJECT_OBJECT: //  Object {}
				so=ScriptObjectObject::newScriptObjectObject(zs);
				break;

			default:

				if(rc->idx_type > IDX_TYPE_SCRIPT_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ScriptObjectClass::newScriptObjectClass(zs,rc->idx_type, value_object);
				}else{
					 THROW_RUNTIME_ERROR("Internal error: An idx class was expected but it was %i",rc->idx_type);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }


	const char * ScriptTypeFactory::getScriptTypeName(short _idx_type){
		if(_idx_type != IDX_ZS_UNDEFINED){
			ScriptType *sc=(ScriptType *)script_types->get(_idx_type);
			return sc->type_name.c_str();
		}
		 return "type_unknow";
	}

	bool 	ScriptTypeFactory::isScriptClassTypeInheritsFrom(short _idx_type,short _idx_base_type){

		if(_idx_type == _idx_base_type){
			return true;
		}

		ScriptType *sc=(ScriptType *)script_types->get(_idx_type);

		for(unsigned i=0; i < sc->idx_base_types->count; i++){
			if(isScriptClassTypeInheritsFrom(sc->idx_base_types->items[i],_idx_base_type)){
				return true;
			}
		}

		return false;
	}

	bool	ScriptTypeFactory::isScriptTypeInstanceable(short idx_type){

		if(
				idx_type==IDX_TYPE_SCRIPT_OBJECT_STRING
				|| idx_type==IDX_TYPE_SCRIPT_OBJECT_VECTOR
				|| idx_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT
				|| idx_type>=IDX_TYPE_MAX
		){
			return true;
		}

		return false;
	}

	ScriptTypeFactory::~ScriptTypeFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < script_types->count; i++) {

			delete (ScriptType *)script_types->get(i);
		}
		script_types->clear();

		delete script_types;
	}
}
