#include "zetscript.h"


#define MAIN_SCRIPT_CLASS_NAME 				"__MainClass__"
#define REGISTER_BUILT_IN_STRUCT(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_string::sformat("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_Class<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_string::sformat("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_ClassBuiltIn<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(zs_string::sformat("Error initializing C built in type: %s",STR(type_class)));\
		return;\
	}else{\
		CScriptClass *sc=registerClass(__FILE__,__LINE__,STR(type_class),"");\
		GET_SCOPE(sc->symbol_info.symbol->idxScope)->is_c_node=true;\
		sc->symbol_info.symbol_info_properties=SYMBOL_INFO_PROPERTY_C_OBJECT_REF;\
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


	CScriptClassFactory::CScriptClassFactory(CZetScript *_zs){
		zs = _zs;
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getScriptFunctionFactory();
		native_function_factory= this->zs->getNativeFunctionFactory();
		register_c_base_symbols=false;
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_object=registerClass(__FILE__, __LINE__,MAIN_SCRIPT_CLASS_NAME,""); // 0
		if(main_object->symbol_info.symbol->idxScope!=IDX_GLOBAL_SCOPE){
			THROW_RUNTIME_ERROR("Error initializing global scope");
			return;
		}

		// REGISTER BUILT IN C TYPES
		REGISTER_BUILT_IN_TYPE(void,IDX_TYPE_VOID_C);
		REGISTER_BUILT_IN_TYPE(int *,IDX_TYPE_INT_PTR_C);
		REGISTER_BUILT_IN_TYPE(float *,IDX_TYPE_FLOAT_PTR_C);
		REGISTER_BUILT_IN_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(std::string *,IDX_TYPE_STRING_PTR_C);
		REGISTER_BUILT_IN_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		REGISTER_BUILT_IN_TYPE(int,IDX_TYPE_INT_C);
		REGISTER_BUILT_IN_TYPE(unsigned int,IDX_TYPE_UNSIGNED_INT_C);
		REGISTER_BUILT_IN_TYPE(intptr_t,IDX_TYPE_INTPTR_T_C);
		REGISTER_BUILT_IN_TYPE(float,IDX_TYPE_FLOAT_C);
		REGISTER_BUILT_IN_TYPE(bool,IDX_TYPE_BOOL_C);

		// REGISTER BUILT IN CLASS TYPES
		REGISTER_BUILT_IN_STRUCT(StackElement,IDX_STACK_ELEMENT);
		REGISTER_BUILT_IN_CLASS(CScriptVar,IDX_CLASS_SCRIPT_VAR);
		REGISTER_BUILT_IN_CLASS(CScriptVarString,IDX_CLASS_STRING);
		REGISTER_BUILT_IN_CLASS(CScriptVarVector,IDX_CLASS_VECTOR);
		REGISTER_BUILT_IN_CLASS(CScriptVarFunctor,IDX_CLASS_FUNCTOR);
		REGISTER_BUILT_IN_CLASS(CScriptVarDictionary,IDX_CLASS_DICTIONARY);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		class_C_BaseOf<CScriptVarVector,CScriptVar>();
		class_C_BaseOf<CScriptVarFunctor,CScriptVar>();
		class_C_BaseOf<CScriptVarDictionary,CScriptVar>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		main_object->registerFunctionMember(__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);

		register_C_Function("print",print);
		//register_C_Function("error",internal_print_error);

		register_C_FunctionMember<CScriptVarVector>("size",&CScriptVarVector::size);
		register_C_FunctionMember<CScriptVarVector>("push",static_cast<void (CScriptVarVector:: *)(StackElement *)>(&CScriptVarVector::push));
		register_C_FunctionMember<CScriptVarVector>("pop",&CScriptVarVector::pop);


		register_C_FunctionMember<CScriptVarDictionary>("add",&CScriptVarDictionary::add_attr);
		register_C_FunctionMember<CScriptVarDictionary>("remove",&CScriptVarDictionary::remove_attr);
		register_C_FunctionMember<CScriptVarDictionary>("size",&CScriptVarDictionary::size);
	}

	void CScriptClassFactory::register_C_BaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}


	/**
	 * Register C variable
	 */
	 VariableSymbolInfo *  CScriptClassFactory::register_C_Variable(const std::string & var_name,void * var_ptr, const std::string & var_type, const char *registered_file,int registered_line)
	{
		//CScope *scope;
		VariableSymbolInfo *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR(zs_string::sformat("cannot register var \"%s\" with NULL reference value", var_name.c_str()));
			return NULL;
		}

		CScriptFunction *main_function=MAIN_FUNCTION;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return  NULL;
		}

		if(getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR(zs_string::sformat("%s has not valid type (%s)",var_name.c_str(),var_type.c_str()));
			return  NULL;
		}


		if((irs = main_function->registerVariable(registered_file,registered_line,var_name,var_type,(intptr_t)var_ptr,SYMBOL_INFO_PROPERTY_C_OBJECT_REF | SYMBOL_INFO_PROPERTY_STATIC_REF)) != NULL){

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());

			return irs;
		}

		return NULL;
	}
	void CScriptClassFactory::clear(){

		bool end=false;
		do{
			CScriptClass * sc = vec_script_class_node.at(vec_script_class_node.size()-1);
			end=(sc->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF;

			if(!end){

				delete sc;
				vec_script_class_node.pop_back();

			}

		}while(!end);

	}

	CScriptClass * CScriptClassFactory::registerClass(const std::string & file, short line,const std::string & class_name, const std::string & base_class_name){
		unsigned char  index;
		CScriptClass *sci=NULL;


		if(vec_script_class_node.size()>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR(zs_string::sformat("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES));
			return NULL;
		}


		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		CScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClass(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClass_Internal(class_name))==ZS_INVALID_CLASS){ // check whether is local var registered scope ...

			// NEW SCOPE C and register ...
			//unsigned char idx_class=(unsigned char)vec_script_class_node.size()-1;

			CScope * scope = NEW_SCOPE();

			Symbol *symbol=scope->registerSymbol(file,line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			sci = new CScriptClass(this->zs,vec_script_class_node.size());

			scope->setScriptClass(sci);


			sci->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;

			sci->symbol_info.symbol=symbol;

			vec_script_class_node.push_back(sci);

			if(base_class != NULL){
				sci->idx_base_class.push_back(base_class->idx_class);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR(zs_string::sformat("class \"%s\" already registered",class_name.c_str()));
		}

		return NULL;
	}



	std::vector<CScriptClass *> * CScriptClassFactory::getVectorScriptClassNode(){
		return & vec_script_class_node;
	}

	std::map<unsigned char, std::map<unsigned char, fntConversionType>>  *	 CScriptClassFactory::getMapTypeConversion() {
		return & mapTypeConversion;
	}

	CScriptClass 	* CScriptClassFactory::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("CScriptClass node out of bound");
			return NULL;
		}

		return vec_script_class_node.at(idx);
	}

	CScriptClass 	* CScriptClassFactory::getScriptClass(const std::string & class_name){
		unsigned char index;
		if((index = getIdxScriptClass_Internal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
			return vec_script_class_node.at(index);
		}

		return NULL;
	}

	CScriptClass *CScriptClassFactory::getScriptClassBy_C_ClassPtr(const std::string & class_type){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_type == vec_script_class_node.at(i)->str_class_ptr_type){//metadata_info.object_info.symbol_info.c_type){
				return vec_script_class_node.at(i);
			}
		}
		return NULL;
	}

	unsigned char CScriptClassFactory::getIdxScriptClass_Internal(const std::string & class_name){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_name == vec_script_class_node.at(i)->symbol_info.symbol->name){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	bool CScriptClassFactory::isClassRegistered(const std::string & v){
		return getIdxScriptClass_Internal(v) != ZS_INVALID_CLASS;
	}

	CScriptVar *		CScriptClassFactory::instanceScriptVariableByClassName(const std::string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->idx_class);
		 }

		 return NULL;
	 }

	 CScriptVar 		 * CScriptClassFactory::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

		 CScriptVar *class_object=NULL;

		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->idx_class){

			 case IDX_TYPE_VOID_C:
			 case IDX_TYPE_INT_PTR_C:
			 case IDX_TYPE_FLOAT_PTR_C:
			 case IDX_TYPE_STRING_PTR_C:
			 case IDX_TYPE_BOOL_PTR_C:
				 THROW_RUNTIME_ERROR("Internal error");
				 return NULL;
				 break;

			 case IDX_CLASS_VECTOR:
			 case IDX_CLASS_DICTIONARY:
				 class_object = (CScriptVar *)value_object;
				 break;
			 default:
				 class_object = new CScriptVar(zs);
				 class_object->init(rc, value_object);
				 break;
			 }
		 }
		 return class_object;
	 }

	unsigned char CScriptClassFactory::getIdx_C_RegisteredClass(const std::string & str_classPtr){
		// ok check c_type
		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(vec_script_class_node[i]->str_class_ptr_type == str_classPtr){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}


	intptr_t CScriptClassFactory::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idx_class,idx_return_type){

		CScriptClass *src_class = getScriptClass(idx_src_class);
		CScriptClass *convert_class = getScriptClass(idx_convert_class);

		//local_map_type_conversion
		if(mapTypeConversion.count(idx_src_class) == 0){
			THROW_RUNTIME_ERROR(zs_string::sformat("There's no type src conversion class \"%s\".",zs_rtti::demangle(src_class->str_class_ptr_type).c_str()));
			return 0;
		}

		if((mapTypeConversion)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR(zs_string::sformat("There's no dest conversion class \"%s\".",zs_rtti::demangle(convert_class->str_class_ptr_type).c_str()));
			return 0;
		}

		return (mapTypeConversion)[idx_src_class][idx_convert_class](obj);
	}

	const char * CScriptClassFactory::getScriptClassName(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return vec_script_class_node[idx]->symbol_info.symbol->name.c_str();
		}
		 return "class_unknow";
	}


	unsigned char CScriptClassFactory::getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (std::string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			 if(vec_script_class_node.at(i)->str_class_ptr_type==c_type_str)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			CScriptClassFactory::getIdxClassFromIts_C_Type(const std::string & c_type_str){
		return getIdxClassFromIts_C_TypeInternal(c_type_str);
	}


	bool 	CScriptClassFactory::isIdxClassInstanceOf(unsigned char idxSrcClass, unsigned char class_idx){

		if(idxSrcClass == class_idx){
			return true;
		}

		CScriptClass * theClass = vec_script_class_node.at(idxSrcClass);

		for(unsigned i=0; i < theClass->idx_base_class.size(); i++){
			if(isIdxClassInstanceOf(theClass->idx_base_class[i],class_idx)){
				return true;
			}
		}

		return false;
	}

	CScriptClassFactory::~CScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node.size(); i++) {

			delete vec_script_class_node.at(i);
		}

		vec_script_class_node.clear();
	}

}
