#include "zetscript.h"


#define MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define REGISTER_BUILT_IN_STRUCT(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",STR(type_class));\
		return;\
	}\
	registerNativeClass<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error: class built in type %s doesn't match its id",STR(type_class));\
		return;\
	}\
	registerNativeClassBuiltIn<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(script_classes->count!=idx_class){\
		THROW_RUNTIME_ERROR("Error initializing C built in type: %s",STR(type_class));\
		return;\
	}else{\
		ScriptClass *sc=registerClass(__FILE__,__LINE__,STR(type_class),"");\
		sc->symbol.scope->is_c_node=true;\
		sc->symbol.symbol_properties=SYMBOL_PROPERTY_C_OBJECT_REF;\
		sc->str_class_ptr_type=(typeid(type_class).name());\
	}

namespace zetscript{

	void  print(const char *s){
		printf("%s\n",s);
		fflush(stdout);
	}

	void  print(int s){
		printf("print integer test: %i\n",s);
		fflush(stdout);
	}


	ScriptClassFactory::ScriptClassFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		function_proxy_factory= this->zs->getFunctionProxyFactory();
		register_c_base_symbols=false;
		main_function=NULL;
		main_object=NULL;
		script_classes=new zs_vector;
	}

	void ScriptClassFactory::init(){

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_object=registerClass(__FILE__, __LINE__,MAIN_SCRIPT_CLASS_NAME,""); // 0

		// init main scope...
		MAIN_SCOPE(this)->script_class=main_object;

		/*if(main_object->symbol.scope!=MAIN_SCOPE(this)){
			THROW_RUNTIME_ERROR("Error initializing global scope");
			return;
		}*/

		// REGISTER BUILT IN C TYPES
		REGISTER_BUILT_IN_TYPE(void,IDX_BUILTIN_TYPE_VOID_C);
		REGISTER_BUILT_IN_TYPE(int *,IDX_BUILTIN_TYPE_INT_PTR_C);
		REGISTER_BUILT_IN_TYPE(float *,IDX_BUILTIN_TYPE_FLOAT_PTR_C);
		REGISTER_BUILT_IN_TYPE(const char *,IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(std::string *,IDX_BUILTIN_TYPE_STRING_PTR_C);
		REGISTER_BUILT_IN_TYPE(bool *,IDX_BUILTIN_TYPE_BOOL_PTR_C);
		REGISTER_BUILT_IN_TYPE(int,IDX_BUILTIN_TYPE_INT_C);
		REGISTER_BUILT_IN_TYPE(unsigned int,IDX_BUILTIN_TYPE_UNSIGNED_INT_C);
		REGISTER_BUILT_IN_TYPE(intptr_t,IDX_BUILTIN_TYPE_INTPTR_T_C);
		REGISTER_BUILT_IN_TYPE(float,IDX_BUILTIN_TYPE_FLOAT_C);
		REGISTER_BUILT_IN_TYPE(bool,IDX_BUILTIN_TYPE_BOOL_C);

		// REGISTER BUILT IN CLASS TYPES
		REGISTER_BUILT_IN_STRUCT(StackElement,IDX_BUILTIN_TYPE_STACK_ELEMENT);
		REGISTER_BUILT_IN_CLASS(ScriptVar,IDX_BUILTIN_TYPE_CLASS_SCRIPT_VAR);
		REGISTER_BUILT_IN_CLASS(ScriptVarString,IDX_BUILTIN_TYPE_CLASS_STRING);
		REGISTER_BUILT_IN_CLASS(ScriptVarVector,IDX_BUILTIN_TYPE_CLASS_VECTOR);
		REGISTER_BUILT_IN_CLASS(ScriptVarFunction,IDX_BUILTIN_TYPE_CLASS_FUNCTION);
		REGISTER_BUILT_IN_CLASS(ScriptVarDictionary,IDX_BUILTIN_TYPE_CLASS_DICTIONARY);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		nativeClassBaseOf<ScriptVarVector,ScriptVar>();
		nativeClassBaseOf<ScriptVarFunction,ScriptVar>();
		nativeClassBaseOf<ScriptVarDictionary,ScriptVar>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's
		Symbol *symbol_main_function=main_object->registerMemberFunction(__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);
		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;

		ZS_REGISTER_GLOBAL_FUNCTION(this,"print",static_cast<void (*)(const char *)>(print));
		ZS_REGISTER_GLOBAL_FUNCTION(this,"print",static_cast<void (*)(int)>(print));


		//registerNativeGlobalFunction("error",internalPrintError);

		//ZS_REGISTER_FUNCTION_MEMBER(this->zs,ScriptVarVector,"size",&ScriptVarVector::size);

		registerNativeMemberFunction("push",&ScriptVarVector::pushSf);
		registerNativeMemberFunction("pop",&ScriptVarVector::popSf);


		registerNativeMemberFunction("add",&ScriptVarDictionary::addAttrSf);
		registerNativeMemberFunction("remove",&ScriptVarDictionary::removeAttrSf);
		//registerNativeMemberFunction("size",&ScriptVarDictionary::sizeSf);

		//-------------------------
		// Register built in extra
		// Math
		registerNativeSingletonClass<MathBuiltIn>("MathBuiltIn");
		registerNativeStaticConstMember<MathBuiltIn>("PI",&MathBuiltIn::PI);
		registerNativeMemberFunctionStatic<MathBuiltIn>("sin",MathBuiltIn::sin);
		registerNativeMemberFunctionStatic<MathBuiltIn>("cos",MathBuiltIn::cos);
		registerNativeMemberFunctionStatic<MathBuiltIn>("abs",MathBuiltIn::abs);
		registerNativeMemberFunctionStatic<MathBuiltIn>("pow",MathBuiltIn::pow);
		registerNativeMemberFunctionStatic<MathBuiltIn>("degToRad",MathBuiltIn::degToRad);
		ZS_REGISTER_GLOBAL_VARIABLE(zs,"Math",&math_built_in);

		// Io
		registerNativeSingletonClass<IoBuiltIn>("IoBuiltIn");
		registerNativeMemberFunctionStatic<IoBuiltIn>("clock",IoBuiltIn::clock);
		ZS_REGISTER_GLOBAL_VARIABLE(zs,"IO",&io_built_in);
	}

	void ScriptClassFactory::registerNativeBaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}

	void ScriptClassFactory::clear(){

		bool end=false;
		for(int v=script_classes->count-1;
				v>=1; // avoid main class
				v--){

			ScriptClass * sc = (ScriptClass *)script_classes->get(v);
			if((sc->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF){ //script class
				delete sc;
				script_classes->pop_back();
			}else{
				break;
			}
		}
	}

	ScriptClass * ScriptClassFactory::registerClass(
			const std::string & file
			, short line
			,const std::string & class_name
			, const std::string & base_class_name
	){
		unsigned char  index;
		ScriptClass *sci=NULL;

		if(script_classes->count>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
			return NULL;
		}

		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		ScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClass(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClassInternal(class_name))==ZS_INVALID_CLASS){ // check whether is local var registered scope ...

			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope = NEW_SCOPE(this,NULL);

			// register symbol on main scope...
			Symbol *symbol=scope->registerSymbol(file,line,class_name, NO_PARAMS_SYMBOL_ONLY);
			if(symbol == NULL){
				return NULL;
			}

			sci = new ScriptClass(this->zs,script_classes->count);

			scope->setScriptClass(sci);

			sci->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;
			sci->symbol=*symbol;

			script_classes->push_back((intptr_t)sci);

			if(base_class != NULL){
				sci->idx_base_classes->push_back((intptr_t)base_class->idx_class);
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

	std::map<unsigned char, std::map<unsigned char, ConversionType>>  *	 ScriptClassFactory::getConversionTypes() {
		return & conversion_types;
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("ScriptClass node out of bound");
			return NULL;
		}
		return (ScriptClass *)script_classes->get(idx);
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(const std::string & class_name){
		unsigned char idx;
		if((idx = getIdxScriptClassInternal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
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

	unsigned char ScriptClassFactory::getIdxScriptClassInternal(const std::string & class_name){

		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(class_name == sc->symbol.name){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	bool ScriptClassFactory::isClassRegistered(const std::string & v){
		return getIdxScriptClassInternal(v) != ZS_INVALID_CLASS;
	}

	ScriptVar *		ScriptClassFactory::instanceScriptVariableByClassName(const std::string & class_name){
		 // 0. Search class info ...
		 ScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->idx_class);
		 }
		 return NULL;
	 }

	 ScriptVar 		 * ScriptClassFactory::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

		 ScriptVar *class_object=NULL;

		 // 0. Search class info ...
		 ScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->idx_class){

			 case IDX_BUILTIN_TYPE_VOID_C:
			 case IDX_BUILTIN_TYPE_INT_PTR_C:
			 case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
			 case IDX_BUILTIN_TYPE_STRING_PTR_C:
			 case IDX_BUILTIN_TYPE_BOOL_PTR_C:
				 THROW_RUNTIME_ERROR("Internal error");
				 return NULL;
				 break;

			 case IDX_BUILTIN_TYPE_CLASS_VECTOR:
			 case IDX_BUILTIN_TYPE_CLASS_DICTIONARY:
				 class_object = (ScriptVar *)value_object;
				 break;
			 default:
				 class_object = new ScriptVar(zs);
				 class_object->init(rc, value_object);
				 break;
			 }
		 }
		 return class_object;
	 }

	unsigned char ScriptClassFactory::getIdx_C_RegisteredClass(const std::string & str_classPtr){
		// ok check str_native_type
		for(unsigned i = 0; i < script_classes->count; i++){
			ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			if(sc->str_class_ptr_type == str_classPtr){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	intptr_t ScriptClassFactory::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idx_class,idx_return_type){

		ScriptClass *src_class = getScriptClass(idx_src_class);
		ScriptClass *convert_class = getScriptClass(idx_convert_class);

		//local_map_type_conversion
		if(conversion_types.count(idx_src_class) == 0){
			THROW_RUNTIME_ERROR("There's no type src conversion class \"%s\".",zs_rtti::demangle(src_class->str_class_ptr_type).c_str());
			return 0;
		}

		if((conversion_types)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR("There's no dest conversion class \"%s\".",zs_rtti::demangle(convert_class->str_class_ptr_type).c_str());
			return 0;
		}

		return (conversion_types)[idx_src_class][idx_convert_class](obj);
	}

	const char * ScriptClassFactory::getScriptClassName(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return ((ScriptClass *)script_classes->get(idx))->symbol.name.c_str();
		}
		 return "class_unknow";
	}

	unsigned char ScriptClassFactory::getIdxScriptInternalFrom_C_Type(const std::string & str_native_type){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (std::string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < script_classes->count; i++){
			 ScriptClass * sc=(ScriptClass *)script_classes->get(i);
			 if(sc->str_class_ptr_type==str_native_type)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			ScriptClassFactory::getIdxClassFromItsNativeType(const std::string & str_native_type){
		return getIdxScriptInternalFrom_C_Type(str_native_type);
	}

	bool 	ScriptClassFactory::nativeClassBaseOf(unsigned char idx_src_class, unsigned char idx_class){

		if(idx_src_class == idx_class){
			return true;
		}

		ScriptClass * the_class = (ScriptClass *)script_classes->get(idx_src_class);

		for(unsigned i=0; i < the_class->idx_base_classes->count; i++){
			intptr_t idx_class_base=the_class->idx_base_classes->items[i];
			if(nativeClassBaseOf(idx_class_base,idx_class)){
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
