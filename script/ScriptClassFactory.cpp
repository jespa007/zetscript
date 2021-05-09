#include "zetscript.h"


#define MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define REGISTER_BUILT_IN_STRUCT(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeClassStatic<type_class>(ZS_STR(type_class),NULL,NULL);


#define REGISTER_BUILT_IN_CLASS(name_class, type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	if(idx_class >= IDX_BUILTIN_TYPE_MAX){\
		THROW_RUNTIME_ERROR("The class to register \"%s\" should be a built in class",ZS_STR(type_class));\
		return;\
	}\
	registerNativeClassStatic<type_class>(name_class,type_class##_new,type_class##_delete);

#define REGISTER_BUILT_IN_CLASS_SINGLETON(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",ZS_STR(type_class));\
		return;\
	}\
	registerNativeSingletonClass<type_class>(ZS_STR(type_class));

#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: %s",ZS_STR(type_class));\
		return;\
	}else{\
		ScriptClass *sc=registerClass(ZS_STR(type_class));\
		sc->symbol_class.scope->is_c_node=true;\
		sc->symbol_class.properties=SYMBOL_PROPERTY_C_OBJECT_REF;\
		sc->str_class_ptr_type=(typeid(type_class).name());\
	}

namespace zetscript{

	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectObject);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectObjectIterator);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectString);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectStringIterator);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectVector);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectVectorIterator);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectClass);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectVarRef);
	ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(ScriptObjectDateTime);

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
		std::string error;
		// ScriptFunctionFactory has to be created
		main_object=registerClass(MAIN_SCRIPT_CLASS_NAME); // 0
		MAIN_SCOPE(this)->script_class=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(error,__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);
		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;

		idx_clear_checkpoint=1; // by default restore till main class.
	}

	ZetScript *ptrToZetScriptPtr(zs_int ptr){
		return (ZetScript *)ptr;
	}

	zs_float parseFloat(zs_int number){

		return (zs_float)(number);
	}

	zs_float parseFloat(std::string  *number_str){
		zs_float result=0;
		zs_float *result_ptr=zs_strutils::parse_float(*number_str);

		if(result_ptr != NULL){
			result=*result_ptr;
			delete result_ptr;
		}

		return result;
	}

	zs_int parseInteger(zs_float *number){

		return (zs_int)(*number);
	}

	zs_int parseInteger(std::string  *number_str){
		zs_int result=0;
		zs_int *result_ptr=zs_strutils::parse_int(*number_str);
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
		REGISTER_BUILT_IN_TYPE(void,IDX_BUILTIN_TYPE_VOID_C);
		REGISTER_BUILT_IN_TYPE(zs_int,IDX_BUILTIN_TYPE_ZS_INT_C);
		REGISTER_BUILT_IN_TYPE(zs_int *,IDX_BUILTIN_TYPE_ZS_INT_PTR_C);
		REGISTER_BUILT_IN_TYPE(char *,IDX_BUILTIN_TYPE_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(const char *,IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(std::string *,IDX_BUILTIN_TYPE_STRING_PTR_C);
		REGISTER_BUILT_IN_TYPE(bool,IDX_BUILTIN_TYPE_BOOL_C);
		REGISTER_BUILT_IN_TYPE(bool *,IDX_BUILTIN_TYPE_BOOL_PTR_C);
		REGISTER_BUILT_IN_TYPE(zs_float,IDX_BUILTIN_TYPE_FLOAT_C);
		REGISTER_BUILT_IN_TYPE(zs_float *,IDX_BUILTIN_TYPE_FLOAT_PTR_C);
		REGISTER_BUILT_IN_TYPE(const zs_float *,IDX_BUILTIN_TYPE_CONST_FLOAT_PTR_C);

		// estructures
		REGISTER_BUILT_IN_STRUCT(StackElement,IDX_BUILTIN_TYPE_STACK_ELEMENT);

		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		REGISTER_BUILT_IN_CLASS_SINGLETON(ScriptFunction,IDX_BUILTIN_TYPE_FUNCTION);
		REGISTER_BUILT_IN_CLASS("VarRef",ScriptObjectVarRef,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF);
		REGISTER_BUILT_IN_CLASS("String",ScriptObjectString,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING);
		REGISTER_BUILT_IN_CLASS("Vector",ScriptObjectVector,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR);
		REGISTER_BUILT_IN_CLASS("DateTime",ScriptObjectDateTime,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME);

		// Script object iterators
		REGISTER_BUILT_IN_CLASS("StringIterator",ScriptObjectStringIterator,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR);
		REGISTER_BUILT_IN_CLASS("VectorIterator",ScriptObjectVectorIterator,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR);
		REGISTER_BUILT_IN_CLASS("ObjectIterator",ScriptObjectObjectIterator,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		REGISTER_BUILT_IN_CLASS("Object",ScriptObjectObject,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT);
		REGISTER_BUILT_IN_CLASS("Class",ScriptObjectClass,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS);
		// it needs script object class to have zetscript reference
		REGISTER_BUILT_IN_CLASS_SINGLETON(ZetScript,IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS_ZETSCRIPT);
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
		ZS_REGISTER_FUNCTION(zs,"parseFloat",static_cast<zs_float (*)(std::string *)>(parseFloat));
		ZS_REGISTER_FUNCTION(zs,"parseInteger",static_cast<zs_int (*)(zs_float *)>(parseInteger));
		ZS_REGISTER_FUNCTION(zs,"parseInteger",static_cast<zs_int (*)(std::string *)>(parseInteger));


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

		// VectorIterator
		registerNativeMemberFunction<ScriptObjectVectorIterator>("_next",ScriptObjectVectorIteratorWrap_next);
		registerNativeMemberFunction<ScriptObjectVectorIterator>("end",ScriptObjectVectorIteratorWrap_end);
		registerNativeMemberFunction<ScriptObjectVectorIterator>("get",ScriptObjectVectorIteratorWrap_get);


		//---------------------------------------------
		// String
		registerNativeMemberFunctionStatic<ScriptObjectString>("formatNative",ScriptObjectString::format);
		registerNativeMemberFunction<ScriptObjectString>("eraseAt",ScriptObjectStringWrap_eraseAt);
		registerNativeMemberFunction<ScriptObjectString>("insertAt",ScriptObjectStringWrap_insertAt);
		registerNativeMemberFunction<ScriptObjectString>("clear",ScriptObjectStringWrap_clear);
		registerNativeMemberFunction<ScriptObjectString>("replace",ScriptObjectStringWrap_replace);
		registerNativeMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ScriptObjectString *so, std::string *)>(ScriptObjectStringWrap_split));
		registerNativeMemberFunction<ScriptObjectString>("split",static_cast<ScriptObjectVector * (*)(ScriptObjectString *so, zs_int )>(ScriptObjectStringWrap_split));
		registerNativeMemberFunction<ScriptObjectString>("size",ScriptObjectStringWrap_size);
		registerNativeMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ScriptObjectString *so, std::string *)>(&ScriptObjectStringWrap_contains));
		registerNativeMemberFunction<ScriptObjectString>("contains",static_cast<bool (*)(ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_contains));

		registerNativeMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ScriptObjectString *so, std::string *)>(&ScriptObjectStringWrap_indexOf));
		registerNativeMemberFunction<ScriptObjectString>("indexOf",static_cast<zs_int (*)(ScriptObjectString *so, zs_int )>(&ScriptObjectStringWrap_indexOf));

		registerNativeMemberFunction<ScriptObjectString>("startsWith",ScriptObjectStringWrap_startsWith);
		registerNativeMemberFunction<ScriptObjectString>("endsWith",ScriptObjectStringWrap_endsWith);
		registerNativeMemberFunction<ScriptObjectString>("substring",ScriptObjectStringWrap_substring);
		registerNativeMemberFunction<ScriptObjectString>("iter",ScriptObjectStringWrap_iter);

		// StringIterator
		registerNativeMemberFunction<ScriptObjectStringIterator>("_next",ScriptObjectStringIteratorWrap_next);
		registerNativeMemberFunction<ScriptObjectStringIterator>("end",ScriptObjectStringIteratorWrap_end);
		registerNativeMemberFunction<ScriptObjectStringIterator>("get",ScriptObjectStringIteratorWrap_get);

		//---------------------------------------------
		// Object
		registerNativeMemberFunctionStatic<ScriptObjectObject>("clear",&ScriptObjectObjectWrap_clear);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("erase",&ScriptObjectObjectWrap_erase);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("contains",&ScriptObjectObjectWrap_contains);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("concat",ScriptObjectObject::concat);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("keys",ScriptObjectObjectWrap_keys);
		registerNativeMemberFunctionStatic<ScriptObjectObject>("iter",ScriptObjectObjectWrap_iter);

		// ObjectIterator
		registerNativeMemberFunction<ScriptObjectObjectIterator>("_next",ScriptObjectObjectIteratorWrap_next);
		registerNativeMemberFunction<ScriptObjectObjectIterator>("end",ScriptObjectObjectIteratorWrap_end);
		registerNativeMemberFunction<ScriptObjectObjectIterator>("get",ScriptObjectObjectIteratorWrap_get);

		//---------------------------------------------
		// DateTime
		registerNativeMemberFunctionStatic<ScriptObjectDateTime>("nowSf",ScriptObjectDateTimeWrap_now);



		zs->saveState();
	}



	void ScriptClassFactory::registerNativeBaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}

	void ScriptClassFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_classes->count-1;
			v >= idx_start; // avoid main class
			v--
		){
			ScriptClass * sc = (ScriptClass *)script_classes->get(v);
			delete sc;
			script_classes->pop_back();
		}
	}

	void ScriptClassFactory::setClearCheckpoint(){
		idx_clear_checkpoint = script_classes->count-1;
	}

	ScriptClass * ScriptClassFactory::registerClass(
			const std::string & class_name
			 ,const std::string & base_class_name
			 ,const std::string & file
			 , short line
	){
		int  index;
		ScriptClass *sci=NULL;

		if(script_classes->count>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
			return NULL;
		}

		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		if((index = getIdxScriptClassInternal(class_name))==ZS_IDX_UNDEFINED){ // check whether is local var registered scope ...

			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope = NEW_SCOPE(this,ZS_IDX_UNDEFINED,NULL);

			// register symbol on main scope...
			Symbol *symbol=scope->registerSymbol(file,line,class_name, NO_PARAMS_SYMBOL_ONLY);

			sci = new ScriptClass(this->zs,script_classes->count,symbol);
			scope->setScriptClass(sci);

			sci->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;
			//sci->symbol_class=*symbol;

			script_classes->push_back((zs_int)sci);

			if(base_class_name != ""){

				ScriptClass *base_class=NULL;

				if(sci->idx_base_classes->count > 0){
					ScriptClass *match_class=getScriptClass(sci->idx_base_classes->items[0]);
					THROW_RUNTIME_ERROR("Class \"%s\" already is inherited from \"%s\"",class_name.c_str(),match_class->symbol_class.name.c_str());
				}

				if((base_class = getScriptClass(base_class_name)) == NULL){
					THROW_RUNTIME_ERROR("Class %s not registered",base_class_name.c_str());
				}

				if(base_class->isNativeSingletonClass()){
					THROW_RUNTIME_ERROR("Class \"%s\" cannot extend from \"%s\" because is singleton. To allow extension register class \"%\" with registerNativeClass instead of registerNativeSingletonClass",class_name.c_str(),base_class_name.c_str(),base_class_name.c_str());
				}


				// 1. extend all symbols from base class
				for(int i=0; i < base_class->symbol_members->count; i++){
					Symbol *symbol=(Symbol *)base_class->symbol_members->items[i];
					Symbol *new_symbol=new Symbol();
					*new_symbol = *symbol;
					sci->symbol_members->push_back((zs_int)new_symbol);
					sci->symbol_members_built_in->push_back((zs_int)new_symbol);
				}

				// set idx starting member
				sci->idx_starting_this_members=sci->symbol_members->count;

				// 2. set idx base class...
				sci->idx_base_classes->push_back(base_class->idx_class);
			}

			if(sci->idx_class != IDX_SCRIPT_CLASS_MAIN){ // main class has no field initializers and reserve first function as main function
				std::string error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=sci->registerMemberFunction(
						error
						,__FILE__
						,__LINE__
						,zs_strutils::format("__@field_initializer_%s_@__",sci->symbol_class.name.c_str())
						,{}
				);

				sci->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;

			}

			return sci;
		}else{
			THROW_RUNTIME_ERROR("class \"%s\" already registered",class_name.c_str());
		}
		return NULL;
	}

	zs_vector * ScriptClassFactory::getScriptClasses(){
		return script_classes;
	}

	std::map<short, std::map<short, ConversionType>>  *	 ScriptClassFactory::getConversionTypes() {
		return & conversion_types;
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(short idx){
		if(idx == ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR("ScriptClass node out of bound");
			return NULL;
		}
		return (ScriptClass *)script_classes->get(idx);
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(const std::string & class_name){
		int idx;
		if((idx = getIdxScriptClassInternal(class_name))!=ZS_IDX_UNDEFINED){ // check whether is local var registered scope ...
			return (ScriptClass *)script_classes->get(idx);
		}
		return NULL;
	}

	ScriptClass *ScriptClassFactory::getScriptClassByNativeClassPtr(const std::string & class_type){

		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(class_type == sc->str_class_ptr_type){//metadata_info.object_info.symbol_info.str_native_type){
				return sc;
			}
		}
		return NULL;
	}

	int ScriptClassFactory::getIdxScriptClassInternal(const std::string & class_name){

		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(class_name == sc->symbol_class.name){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	bool ScriptClassFactory::isClassRegistered(const std::string & v){
		return getIdxScriptClassInternal(v) != ZS_IDX_UNDEFINED;
	}

	ScriptObjectClass *		ScriptClassFactory::instanceScriptObjectClassByClassName(const std::string & class_name){
		 // 0. Search class info ...
		 ScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptObjectClassByIdx(rc->idx_class);
		 }
		 return NULL;
	 }

	 ScriptObjectClass 		 * ScriptClassFactory::instanceScriptObjectClassByIdx(short idx_class, void * value_object){

		 ScriptObjectClass *so=NULL;

		 // 0. Search class info ...
		 ScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){
			 // Is a primitive ?
			if(rc->idx_class > IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS){
				 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
				 so = ScriptObjectClass::newScriptObjectClass(zs,rc->idx_class, value_object);
			}else{
				 THROW_RUNTIME_ERROR("Internal error: An idx class was expected but it was %i",rc->idx_class);
				 return NULL;
			 }
		 }
		 return so;
	 }

	short ScriptClassFactory::getIdxNativeRegisteredClass(const std::string & str_classPtr){
		// ok check str_native_type
		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(sc->str_class_ptr_type == str_classPtr){
				return i;
			}
		}
		return ZS_IDX_UNDEFINED;
	}

	zs_int ScriptClassFactory::doCast(zs_int obj, short idx_class_src, short idx_class_dst/*, std::string & error*/){//c_class->idx_class,idx_return_type){

		ScriptClass *class_src = getScriptClass(idx_class_src);
		ScriptClass *class_dst = getScriptClass(idx_class_dst);

		//local_map_type_conversion
		if(conversion_types.count(idx_class_src) == 0){
			//error=zs_strutils::format("There's no type src conversion class \"%s\".",zs_rtti::demangle(class_src->str_class_ptr_type).c_str());
			return 0;
		}

		if((conversion_types)[idx_class_src].count(idx_class_dst) == 0){
			//error=zs_strutils::format("There's no dest conversion class \"%s\".",zs_rtti::demangle(class_dst->str_class_ptr_type).c_str());
			return 0;
		}

		return (conversion_types)[idx_class_src][idx_class_dst](obj);
	}

	const char * ScriptClassFactory::getScriptClassName(short idx){
		if(idx != ZS_IDX_UNDEFINED){
			ScriptClass *sc=(ScriptClass *)script_classes->get(idx);
			return sc->symbol_class.name.c_str();
		}
		 return "class_unknow";
	}

	int ScriptClassFactory::getIdxScriptInternalFrom_C_Type(const std::string & str_native_type){

		// 1. we have to handle primitives like void, (int *), (bool *),(zs_float *) and (std::string *).
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

	short 			ScriptClassFactory::getIdxClassFromItsNativeType(const std::string & str_native_type){
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

	ScriptClassFactory::~ScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < script_classes->count; i++) {

			delete (ScriptClass *)script_classes->get(i);
		}
		script_classes->clear();

		delete script_classes;
	}
}
