#include "zetscript.h"


#define MAIN_SCRIPT_CLASS_NAME 				"__MainClass__"
#define REGISTER_BUILT_IN_STRUCT(type_class, idx_class)\
	if(script_classes.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_strutils::format("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_Class<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
	if(script_classes.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_strutils::format("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_ClassBuiltIn<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(script_classes.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_strutils::format("Error initializing C built in type: %s",STR(type_class)));\
		return;\
	}else{\
		ScriptClass *sc=registerClass(__FILE__,__LINE__,STR(type_class),"");\
		sc->symbol.scope->is_c_node=true;\
		sc->symbol.symbol_properties=SYMBOL_PROPERTY_C_OBJECT_REF;\
		sc->str_class_ptr_type=(typeid(type_class).name());\
	}

namespace zetscript{

	static void  print(const char *s){

		printf("%s\n",s);
		fflush(stdout);

		/*if(print_out_callback){
			print_out_callback(s);
		}*/
	}


	ScriptClassFactory::ScriptClassFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		proxy_function_factory= this->zs->GetProxyFunctionFactory();
		register_c_base_symbols=false;
		main_function=NULL;
		main_object=NULL;
	}

	void ScriptClassFactory::init(){

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_object=registerClass(__FILE__, __LINE__,MAIN_SCRIPT_CLASS_NAME,""); // 0
		if(main_object->symbol.scope!=MAIN_SCOPE(this)){
			THROW_RUNTIME_ERROR("Error initializing global scope");
			return;
		}

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
		REGISTER_BUILT_IN_CLASS(ScriptVarFunctor,IDX_BUILTIN_TYPE_CLASS_FUNCTOR);
		REGISTER_BUILT_IN_CLASS(ScriptVarDictionary,IDX_BUILTIN_TYPE_CLASS_DICTIONARY);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		class_C_BaseOf<ScriptVarVector,ScriptVar>();
		class_C_BaseOf<ScriptVarFunctor,ScriptVar>();
		class_C_BaseOf<ScriptVarDictionary,ScriptVar>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		main_function=main_object->registerFunctionMember(__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);

		register_C_Function("print",print);
		//register_C_Function("error",internalPrintError);

		register_C_FunctionAsFunctionMember("size",&ScriptVarVector::sizeSf);
		register_C_FunctionAsFunctionMember("push",&ScriptVarVector::pushSf);
		register_C_FunctionAsFunctionMember("pop",&ScriptVarVector::popSf);


		register_C_FunctionAsFunctionMember("add",&ScriptVarDictionary::addAttrSf);
		register_C_FunctionAsFunctionMember("remove",&ScriptVarDictionary::removeAttrSf);
		register_C_FunctionAsFunctionMember("size",&ScriptVarDictionary::sizeSf);
	}

	void ScriptClassFactory::register_C_BaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}


	/**
	 * Register C variable
	 */
	 bool  ScriptClassFactory::register_C_Variable(
			 const std::string & var_name
			 ,void * var_ptr
			 , const std::string & var_type
			 , const char *registered_file
			 ,int registered_line)
	{
		//Scope *scope;
		Symbol *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("cannot register var \"%s\" with NULL reference value", var_name.c_str()));
			return false;
		}

		ScriptFunction *main_function=MAIN_FUNCTION(this);

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return false;
		}

		if(getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR(zs_strutils::format("%s has not valid type (%s)",var_name.c_str(),var_type.c_str()));
			return false;
		}


		if((irs = main_function->addSymbol(
				MAIN_SCOPE(this)
				,registered_file
				,registered_line
				,var_name
				,var_type
				,(intptr_t)var_ptr
				,SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_STATIC_REF)) != NULL
		){

			ZS_PRINT_DEBUG("Registered variable name: %s",var_name.c_str());

			return true;
		}

		return false;
	}
	void ScriptClassFactory::clear(){

		bool end=false;
		do{
			ScriptClass * sc = script_classes.at(script_classes.size()-1);
			end=(sc->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF;

			if(!end){

				delete sc;
				script_classes.pop_back();

			}

		}while(!end);

	}

	ScriptClass * ScriptClassFactory::registerClass(
			const std::string & file
			, short line
			,const std::string & class_name
			, const std::string & base_class_name
	){
		unsigned char  index;
		ScriptClass *sci=NULL;

		if(script_classes.size()>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR(zs_strutils::format("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES));
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
			//unsigned char idx_class=(unsigned char)script_classes.size()-1;

			Scope * scope = NEW_SCOPE(this,NULL);

			// register symbol on main scope...
			Symbol *symbol=scope->registerSymbol(file,line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			sci = new ScriptClass(this->zs,script_classes.size());

			scope->setScriptClass(sci);


			sci->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;
			sci->symbol=*symbol;

			script_classes.push_back(sci);

			if(base_class != NULL){
				sci->idx_base_classes->push_back((intptr_t)base_class->idx_class);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR(zs_strutils::format("class \"%s\" already registered",class_name.c_str()));
		}

		return NULL;
	}



	std::vector<ScriptClass *> * ScriptClassFactory::getScriptClasses(){
		return & script_classes;
	}

	std::map<unsigned char, std::map<unsigned char, ConversionType>>  *	 ScriptClassFactory::getConversionTypes() {
		return & conversion_types;
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("ScriptClass node out of bound");
			return NULL;
		}

		return script_classes.at(idx);
	}

	ScriptClass 	* ScriptClassFactory::getScriptClass(const std::string & class_name){
		unsigned char index;
		if((index = getIdxScriptClassInternal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
			return script_classes.at(index);
		}

		return NULL;
	}

	ScriptClass *ScriptClassFactory::getScriptClassBy_C_ClassPtr(const std::string & class_type){

		for(unsigned i = 0; i < script_classes.size(); i++){
			if(class_type == script_classes.at(i)->str_class_ptr_type){//metadata_info.object_info.symbol_info.c_type){
				return script_classes.at(i);
			}
		}
		return NULL;
	}

	unsigned char ScriptClassFactory::getIdxScriptClassInternal(const std::string & class_name){

		for(unsigned i = 0; i < script_classes.size(); i++){
			if(class_name == script_classes.at(i)->symbol.name){
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
		// ok check c_type
		for(unsigned i = 0; i < script_classes.size(); i++){
			if(script_classes[i]->str_class_ptr_type == str_classPtr){
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
			THROW_RUNTIME_ERROR(zs_strutils::format("There's no type src conversion class \"%s\".",zs_rtti::demangle(src_class->str_class_ptr_type).c_str()));
			return 0;
		}

		if((conversion_types)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR(zs_strutils::format("There's no dest conversion class \"%s\".",zs_rtti::demangle(convert_class->str_class_ptr_type).c_str()));
			return 0;
		}

		return (conversion_types)[idx_src_class][idx_convert_class](obj);
	}

	const char * ScriptClassFactory::getScriptClassName(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return script_classes[idx]->symbol.name.c_str();
		}
		 return "class_unknow";
	}


	unsigned char ScriptClassFactory::getIdxScriptInternalFrom_C_Type(const std::string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (std::string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < script_classes.size(); i++){
			 if(script_classes.at(i)->str_class_ptr_type==c_type_str)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			ScriptClassFactory::getIdxClassFromIts_C_Type(const std::string & c_type_str){
		return getIdxScriptInternalFrom_C_Type(c_type_str);
	}


	bool 	ScriptClassFactory::class_C_BaseOf(unsigned char idx_src_class, unsigned char idx_class){

		if(idx_src_class == idx_class){
			return true;
		}

		ScriptClass * the_class = script_classes.at(idx_src_class);

		for(unsigned i=0; i < the_class->idx_base_classes->count; i++){
			intptr_t idx_class_base=the_class->idx_base_classes->items[i];
			if(class_C_BaseOf(idx_class_base,idx_class)){
				return true;
			}
		}

		return false;
	}

	ScriptClassFactory::~ScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < script_classes.size(); i++) {

			delete script_classes.at(i);
		}

		script_classes.clear();
	}

}
