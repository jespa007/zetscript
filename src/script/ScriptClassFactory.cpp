/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeClass<type_class>(ZS_STR(type_class),NULL,NULL);


#define SCF_REGISTER_CLASS(name_class, type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	if(idx_class >= IDX_TYPE_MAX){\
		THROW_RUNTIME_ERROR("The class to register '%s' should be a built in class",ZS_STR(type_class));\
		return;\
	}\
	registerNativeClass<type_class>(name_class,type_class##Wrap_New,type_class##Wrap_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeSingletonClass<type_class>(ZS_STR(type_class));

#define SCF_REGISTER_NATIVE_TYPE(type, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: %s",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptClass *sc=registerClass(ZS_STR(type));\
		sc->class_scope->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		sc->properties=SCRIPT_CLASS_PROPERTY_C_OBJECT_REF;\
		sc->str_class_ptr_type=(typeid(type).name());\
	}

#define SCF_REGISTER_TYPE(str_type, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error initializing built in type: %s",str_type);\
		return;\
	}else{\
		registerClass(str_type);\
	}

namespace zetscript{

	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectObject);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorObject);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectString);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorString);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectVector);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectIteratorVector);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectClass);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectVarRef);
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ScriptObjectMemberFunction);

	ScriptClassFactory::ScriptClassFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		register_c_base_symbols=false;
		main_function=NULL;
		main_object=NULL;
		script_classes=new zs_vector;
		main_object=NULL;
		main_function=NULL;
		idx_clear_checkpoint=0;
	}

	void ScriptClassFactory::init(){
		zs_string error;
		// ScriptFunctionFactory has to be created
		main_object=registerClass(MAIN_SCRIPT_CLASS_NAME); // 0
		MAIN_SCOPE(this)->script_class=main_object;

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


	void ScriptClassFactory::registerSystem(){

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

		ZS_REGISTER_FUNCTION(zs,"ptrToZetScriptPtr",ptrToZetScriptPtr);
		ZS_REGISTER_FUNCTION(zs,"parseFloat",static_cast<zs_float (*)(zs_int)>(parseFloat));
		ZS_REGISTER_FUNCTION(zs,"parseFloat",static_cast<zs_float (*)(zs_string *)>(parseFloat));
		ZS_REGISTER_FUNCTION(zs,"parseInteger",static_cast<zs_int (*)(zs_float *)>(parseInteger));
		ZS_REGISTER_FUNCTION(zs,"parseInteger",static_cast<zs_int (*)(zs_string *)>(parseInteger));

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

	void ScriptClassFactory::registerNativeBaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------
	// REGISTER CONSTANTS
	//

	void ScriptClassFactory::registerConstantVariable(const zs_string & var_name, int value, const char *registered_file, short registered_line){
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

	void ScriptClassFactory::registerConstantVariable(const zs_string & var_name, bool value, const char *registered_file, short registered_line){
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

	void ScriptClassFactory::registerConstantVariable(const zs_string & var_name, zs_float value, const char *registered_file, short registered_line){
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

	void ScriptClassFactory::registerConstantVariable(const zs_string & var_name, const zs_string & v, const char *registered_file, short registered_line){
		Symbol *symbol_variable=MAIN_FUNCTION(this)->registerLocalVariable(
			MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, var_name
		);

		StackElement *stk=(StackElement *)symbol_variable->ref_ptr;
		*stk=*(zs->registerStkStringObject(var_name,v));
	}

	void ScriptClassFactory::registerConstantVariable(const zs_string & var_name, const char * v, const char *registered_file, short registered_line){
		registerConstantVariable(var_name, zs_string(v), registered_file, registered_line);
	}

	// REGISTER CONSTANTS
	//--------------------------------------------------------------------------------------------------------------------------------------------

	void ScriptClassFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_classes->count-1;
			v > idx_start; // avoid main class
			v--
		){
			ScriptClass * sc = (ScriptClass *)script_classes->get(v);
			delete sc;
			script_classes->pop_back();
		}
	}

	void ScriptClassFactory::saveState(){
		idx_clear_checkpoint = script_classes->count-1;
	}

	void ScriptClassFactory::checkClassName(const zs_string & class_name){

		if(script_classes->count>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
		}

		if(zs->getScriptFunctionFactory()->getScriptFunctions()->count > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(class_name,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN))!=NULL){
				THROW_RUNTIME_ERROR("Class name '%s' collides with symbol defined at [%s:%i]",class_name.c_str(),main_function_symbol->file,main_function_symbol->line);
			}
		}
	}

	ScriptClass * ScriptClassFactory::registerClass(
			const zs_string & class_name
			 ,const zs_string & base_class_name
			 ,const char * file
			 , short line
	){
		int  index;
		ScriptClass *sc=NULL;

		checkClassName(class_name);

		if((index = getIdxScriptClassInternal(class_name))==ZS_IDX_UNDEFINED){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			index=script_classes->count;

			// To avoid built-int conflict bool type
			if(index==IDX_TYPE_BOOL_C || index==IDX_TYPE_NULL){
				properties_register_scope|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL, SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=MAIN_SCOPE(this)->registerSymbolType(file,line,class_name,properties_register_scope);

			sc = new ScriptClass(this->zs,index, class_name, scope);
			scope->setScriptClass(sc);
			symbol->ref_ptr=(zs_int)sc;

			//sc->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;

			script_classes->push_back((zs_int)sc);

			if(base_class_name != ""){

				ScriptClass *base_class=NULL;

				if(sc->idx_base_classes->count > 0){
					ScriptClass *match_class=getScriptClass(sc->idx_base_classes->items[0]);
					THROW_RUNTIME_ERROR("Class '%s' already is inherited from '%s'"
							,class_name.c_str()
							,match_class->class_name.c_str());
				}

				if((base_class = getScriptClass(base_class_name)) == NULL){
					THROW_RUNTIME_ERROR("Class %s not registered",base_class_name.c_str());
				}

				if(base_class->isNativeSingletonClass()){
					THROW_RUNTIME_ERROR("Class '%s' cannot extend from '%s' because is singleton. To allow extension register class '%' with registerNativeClass instead of registerNativeSingletonClass",class_name.c_str(),base_class_name.c_str(),base_class_name.c_str());
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
						MemberProperty *ma_src=(MemberProperty *)symbol_src->ref_ptr;
						MemberProperty *ma_dst=new MemberProperty(sc,ma_src->property_name);
						ma_dst->getter=ma_src->getter;
						ma_dst->post_inc=ma_src->post_inc;
						ma_dst->post_dec=ma_src->post_dec;
						ma_dst->pre_inc=ma_src->pre_inc;
						ma_dst->pre_dec=ma_src->pre_dec;
						ma_dst->neg=ma_src->neg;

						const ByteCodeMetamethod *it_setters=MemberProperty::byte_code_metamethod_member_setter_list;

						while(*it_setters!=0){
							MemberPropertySetterInfo mp_info=ma_src->getSetterInfo(*it_setters);
							for(unsigned i=0; i < mp_info.setters->count;i++){
								ma_dst->addSetter(*it_setters,(ScriptFunction *)(((StackElement *)mp_info.setters->items[i])->value));

							}
							it_setters++;
						}

						sc->allocated_member_properties->push_back((zs_int)ma_dst);
						symbol_dst->ref_ptr=(zs_int)ma_dst;
					}
				}

				// set idx starting member
				sc->idx_starting_this_member_variables=sc->class_scope->symbol_variables->count;

				// 2. set idx base class...
				sc->idx_base_classes->push_back(base_class->idx_class);
			}

			if(sc->idx_class != IDX_SCRIPT_CLASS_MAIN){ // main class has no field initializers and reserve first function as main function
				zs_string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sc->registerMemberFunction(
					zs_strutils::format("__@field_initializer_%s_@__",sc->class_name.c_str())
				);

				sc->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;
			}

			return sc;
		}else{
			THROW_RUNTIME_ERROR("class '%s' already registered",class_name.c_str());
		}
		return NULL;
	}

	zs_vector * ScriptClassFactory::getScriptClasses(){
		return script_classes;
	}

	int ScriptClassFactory::getBuiltinTypeOrClass(const zs_string & name){
		ScriptClass *sc;

		if(name == ZS_TYPE_NAME_NULL){
			return IDX_TYPE_NULL;
		}else if(name == ZS_TYPE_NAME_INT){
			return IDX_TYPE_ZS_INT_C;
		}else if(name == ZS_TYPE_NAME_FLOAT){
			return IDX_TYPE_ZS_FLOAT_C;
		}else if(name == ZS_TYPE_NAME_BOOL){
			return IDX_TYPE_BOOL_C;
		}else if((sc=getScriptClass(name))!=NULL){
			return sc->idx_class;
		}

		return ZS_IDX_UNDEFINED;
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(short idx){
		if(idx == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("ScriptClass node out of bound");
			return NULL;
		}
		return (ScriptClass *)script_classes->get(idx);
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(const zs_string & class_name){
		int idx;
		if((idx = getIdxScriptClassInternal(class_name))!=ZS_IDX_UNDEFINED){ // check whether is local var registered scope ...
			return (ScriptClass *)script_classes->get(idx);
		}
		return NULL;
	}

	ScriptClass *ScriptClassFactory::getScriptClassByNativeClassPtr(const zs_string & class_type){

		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(class_type == sc->str_class_ptr_type){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	int ScriptClassFactory::getIdxScriptClassInternal(const zs_string & class_name){

		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(class_name == sc->class_name){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	bool ScriptClassFactory::isClassRegistered(const zs_string & v){
		return getIdxScriptClassInternal(v) != ZS_IDX_UNDEFINED;
	}

	ScriptObject *		ScriptClassFactory::instanceScriptObjectByClassName(const zs_string & class_name){
		 // 0. Search class info ...
		 ScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptObjectByClassIdx(rc->idx_class);
		 }
		 return NULL;
	 }

	 ScriptObject 		 * ScriptClassFactory::instanceScriptObjectByClassIdx(short idx_class, void * value_object){

		 ScriptObject *so=NULL;

		 // 0. Search class info ...
		 ScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){
			 // Is a primitive ?
			switch(rc->idx_class){
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

				if(rc->idx_class > IDX_TYPE_SCRIPT_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ScriptObjectClass::newScriptObjectClass(zs,rc->idx_class, value_object);
				}else{
					 THROW_RUNTIME_ERROR("Internal error: An idx class was expected but it was %i",rc->idx_class);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }

	short ScriptClassFactory::getIdxNativeRegisteredClass(const zs_string & str_classPtr){
		// ok check str_native_type
		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(sc->str_class_ptr_type == str_classPtr){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	const char * ScriptClassFactory::getScriptClassName(short idx){
		if(idx != ZS_IDX_UNDEFINED){
			ScriptClass *sc=(ScriptClass *)script_classes->get(idx);
			return sc->class_name.c_str();
		}
		 return "class_unknow";
	}

	int ScriptClassFactory::getIdxScriptInternalFrom_C_Type(const char * str_native_type){

		// 1. we have to handle primitives like void, (zs_int *), (bool *),(zs_float *) and (zs_string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < script_classes->count; i++){
			 ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			 if(sc->str_class_ptr_type==str_native_type)
			 {
				 return i;
			 }
		 }

		 return ZS_IDX_UNDEFINED;
	 }

	short 			ScriptClassFactory::getIdxClassFromItsNativeType(const char * str_native_type){
		return getIdxScriptInternalFrom_C_Type(str_native_type);
	}

	bool 	ScriptClassFactory::isClassInheritsFrom(short idx_class,short idx_base_class){

		if(idx_class == idx_base_class){
			return true;
		}

		ScriptClass *sc=(ScriptClass *)script_classes->get(idx_class);

		for(unsigned i=0; i < sc->idx_base_classes->count; i++){
			if(isClassInheritsFrom(sc->idx_base_classes->items[i],idx_base_class)){
				return true;
			}
		}

		return false;
	}

	bool							ScriptClassFactory::isClassInstanceable(short idx_class){




		if(
				idx_class==IDX_TYPE_SCRIPT_OBJECT_STRING
				|| idx_class==IDX_TYPE_SCRIPT_OBJECT_VECTOR
				|| idx_class==IDX_TYPE_SCRIPT_OBJECT_OBJECT
				|| idx_class>=IDX_TYPE_MAX
		){
			return true;
		}

		return false;
	}

	ScriptClassFactory::~ScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < script_classes->count; i++) {

			delete (ScriptClass *)script_classes->get(i);
		}
		script_classes->clear();

		delete script_classes;
	}
}
