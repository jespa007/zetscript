#pragma once



#include "ScriptDefinesStructs.h"
#include "RegisterFunctionHelper.h"
#include "ast/ast.h"

#define register_C_Function(s) CScriptClassFactory::register_C_FunctionInt(STR(s),s)
#define register_C_Variable(s) CScriptClassFactory::register_C_VariableInt(STR(s),&s,typeid(decltype(s) *).name())

#define getIdxGlobalVariable(s)  CScriptClassFactory::register_C_FunctionInt(STR(s),s)
#define getIdxGlobalFunction(s)



#define register_C_VariableMember(o,s) 		register_C_VariableMemberInt<o, decltype(o::s)>(STR(s),offsetOf(&o::s))
#define register_C_FunctionMember(o,s)		register_C_FunctionMemberInt<o>(STR(s),&o::s)



#define NEW_CLASS_VAR_BY_IDX(idx) (CScriptClassFactory::getInstance()->newClassByIdx(idx))

class CScopeInfo;

class CScriptClassFactory{


	int idxClassInteger, idxClassNumber, idxClassString, idxClassBoolean, idxClassVector, idxClassFunctor, idxClassUndefined, idxClassVoid, idxClassNull;


public:


	enum C_TYPE_VALID_PRIMITIVE_VAR{
		VOID_TYPE,
		INT_TYPE,
		INT_PTR_TYPE,
		FLOAT_PTR_TYPE,
		STRING_PTR_TYPE,
		BOOL_PTR_TYPE,
		MAX_C_TYPE_VALID_PRIMITIVE_VAR
	};



	typedef struct{
		string 	    type_str;
		C_TYPE_VALID_PRIMITIVE_VAR  id;
	}tPrimitiveType;

	typedef struct{
		tPrimitiveType 				*return_type;
		vector<tPrimitiveType*>		params;
	}tRegisterFunction;

	static tPrimitiveType valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];
	static void registerPrimitiveTypes();
	int getIdxClassFromIts_C_Type(const string & c_type_str);

	static CScriptClassFactory*  getInstance();
	static void destroySingletons();

	/**
	 * This function registers a script class into factory.
	 */
	tInfoRegisteredClass * registerScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast);

	/**
	 * Class name given this function creates the object and initializes all variables.
	 */
	CScriptVariable 		 * newClass(const string & class_name);
	CScriptVariable 		 * newClassByIdx(unsigned idx, void * value_object = NULL);



	bool updateReferenceSymbols();



	tInfoRegisteredVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,PASTNode  node);
	tInfoRegisteredVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
	int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname, bool show_msg=true);
	int 							 getIdxRegisteredVariableSymbol(tScriptFunctionInfo *irf,const string & var_name, bool show_msg=true);


	tInfoRegisteredFunctionSymbol *  registerFunctionSymbol(const string & class_name, const string & name,PASTNode  node);
	tInfoRegisteredFunctionSymbol *  getRegisteredFunctionSymbol(const string & class_name,const string & function_name, bool show_errors=true);
	int 							 getIdxRegisteredFunctionSymbol(tScriptFunctionInfo *irf,const string & function_name, bool show_msg=true);


	tScriptFunctionInfo *  getSuperClass(tInfoRegisteredClass *irc, const string & fun_name);


	bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


	tInfoRegisteredClass * 	getRegisteredClass(const string & v, bool print_msg=true);
	tInfoRegisteredClass *	getRegisteredClassByIdx(unsigned index);
	tInfoRegisteredClass *  getRegisteredClassBy_C_ClassPtr(const string & v, bool print_msg=true);

	int 					getIdxRegisteredClass(const string & v);
	bool isClassRegistered(const string & v);
	fntConversionType getConversionType(string objectType, string conversionType, bool show_errors=true);

	const char * getNameRegisteredClassByIdx(int idx);


	// internal var types ...
	int getIdxClassVoid(){return idxClassVoid;}
	tInfoRegisteredClass *  getRegisteredClassVoid(){return m_registeredClass[idxClassVoid];}

	int getIdxClassUndefined(){return idxClassUndefined;}
	tInfoRegisteredClass *  getRegisteredClassUndefined(){return m_registeredClass[idxClassUndefined];}

	int getIdxClassInteger(){return idxClassInteger;}
	tInfoRegisteredClass *  getRegisteredClassInteger(){return m_registeredClass[idxClassInteger];}

	int getIdxClassNumber(){return idxClassNumber;}
	tInfoRegisteredClass *  getRegisteredClassNumber(){return m_registeredClass[idxClassNumber];}

	int getIdxClassString(){return idxClassString;}
	tInfoRegisteredClass *  getRegisteredClassString(){return m_registeredClass[idxClassString];}

	int getIdxClassBoolean(){return idxClassBoolean;}
	tInfoRegisteredClass *  getRegisteredClassBoolean(){return m_registeredClass[idxClassBoolean];}

	int getIdxClassVector(){return idxClassVector;}
	tInfoRegisteredClass *  getRegisteredClassVector(){return m_registeredClass[idxClassVector];}

	int getIdxClassFunctor(){return idxClassFunctor;}
	tInfoRegisteredClass *  getRegisteredClassFunctor(){return m_registeredClass[idxClassFunctor];}

	int getIdxClassNull(){return idxClassNull;}
	tInfoRegisteredClass *  getRegisteredClassNull(){return m_registeredClass[idxClassNull];}

	void printGeneratedCodeAllClasses();

	template <class _R, typename _F>
	static void * new_proxy_function(unsigned int n_args, _F fun_obj){
		using namespace std::placeholders;
		void *proxy_function=NULL;

		switch(n_args){
		case 0:
			proxy_function=(void *)(new std::function<_R ()>(std::bind((_R (*)())fun_obj)));
			break;
		case 1:
			proxy_function=(void *)( new std::function<_R (int)>(std::bind((_R (*)(int))fun_obj, _1)));
			break;
		case 2:
			proxy_function=(void *)( new std::function<_R (int,int)>(std::bind((_R (*)(int,int))fun_obj, 1,_2)));
			break;
		case 3:
			proxy_function= (void *)(new std::function<_R (int,int,int)>(std::bind((_R (*)(int,int,int))fun_obj, 1,_2,_3)));
			break;
		case 4:
			proxy_function=(void *)( new std::function<_R (int,int,int,int)>(std::bind((_R (*)(int,int,int,int))fun_obj, 1,_2,_3,_4)));
			break;
		case 5:
			proxy_function=(void *)( new std::function<_R (int,int,int,int,int)>(std::bind((_R (*)(int,int,int,int,int))fun_obj, 1,_2,_3,_4,_5)));
			break;
		case 6:
			proxy_function=(void *)( new std::function<_R (int,int,int,int,int,int)>(std::bind((_R (*)(int,int,int,int,int,int))fun_obj, 1,_2, _3, _4, _5, _6)));
			break;
		default:
			print_error_cr("Max argyments reached!");
			break;

		}

		return proxy_function;
	}

	template <class _R>
	static bool delete_proxy_function(unsigned int n_args, void *obj){
		using namespace std::placeholders;


		switch(n_args){
		case 0:
			delete (std::function<_R ()>*)obj;
			break;
		case 1:
			delete (std::function<_R (int)>*)obj;
			break;
		case 2:
			delete (std::function<_R (int,int)>*)obj;
			break;
		case 3:
			delete (std::function<_R (int,int,int)>*)obj;
			break;
		case 4:
			delete (std::function<_R (int,int,int,int)>*)obj;
			break;
		case 5:
			delete (std::function<_R (int,int,int,int,int)>*)obj;
			break;
		case 6:
			delete (std::function<_R (int,int,int,int,int,int)>*)obj;
			break;
		default:
			print_error_cr("Max argyments reached!");
			return false;
		}

		return true;
	}


	/**
	 * Register C function
	 */
	template <typename F>
	bool register_C_FunctionInt(const string & function_name,F function_ptr)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;

		//tPrimitiveType *rt;
		//vector<tPrimitiveType *> pt;
		//CScopeInfo::tInfoScopeVar  *rs;

		//CScriptVariable *sf=NULL;
		tInfoRegisteredFunctionSymbol * mainFunctionInfo = getRegisteredFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);
		//tInfoRegisteredClass *rc = CScriptClassFactory::getInstance()->getRegisteredClass(class_name);

		if(mainFunctionInfo == NULL){
			print_error_cr("main function is not created");
			exit(EXIT_FAILURE);
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_ptr)>;
		getParamsFunction<Traits3>(0,return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((irs.idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
			print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name.c_str());
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(getIdxClassFromIts_C_Type(irs.m_arg[i])==-1){
				print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(irs.m_arg[i]).c_str(),function_name.c_str());
				return false;
			}
		}

		// init struct...

		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;
		irs.object_info.symbol_info.symbol_name = function_name;

		irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;
		if(irs.idx_return_type == getIdxClassVoid()){
			if((irs.object_info.symbol_info.ref_ptr=(int)new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}
		else{
			if((irs.object_info.symbol_info.ref_ptr=(int)new_proxy_function<int>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;
				return false;
			}
		}

		irs.object_info.symbol_info.index = mainFunctionInfo->object_info.local_symbols.m_registeredFunction.size();
		mainFunctionInfo->object_info.local_symbols.m_registeredFunction.push_back(irs);

		print_info_cr("Registered function name: %s",function_name.c_str());

		return true;
	}


	/**
	 * Register C variable
	 */
	bool register_C_VariableInt(const string & var_str,void * var_ptr, const string & var_type);



	int getIdx_C_RegisteredClass(const string & str_classPtr, bool print_msg=true){
			// ok check c_type
			for(unsigned i = 0; i < m_registeredClass.size(); i++){
				if(m_registeredClass[i]->classPtrType == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}

	int getIdx_C_RegisteredFunctionMemberClass(const string & str_classPtr, const string & str_functionName, bool print_msg=true){

			int index_class = getIdx_C_RegisteredClass(str_classPtr,print_msg);

			if(index_class == -1){
				return -1;
			}


			vector<tInfoRegisteredFunctionSymbol> * vec_irfs = &m_registeredClass[index_class]->metadata_info.object_info.local_symbols.m_registeredFunction;

			// ok check c_type
			for(unsigned i = 0; i < vec_irfs->size(); i++){
				if(vec_irfs->at(i).object_info.symbol_info.ast->value_symbol == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}


	/**
	 * Register C Class. Return index registered class
	 */
	template<class _T>
	bool register_C_Class(const string & class_name){//, const string & base_class_name=""){

		/*tInfoRegisteredClass *base_class = NULL;

		// get base class
		if(base_class_name != ""){
			if((base_class = this->getRegisteredClass(base_class_name)) == NULL){
				return false;
			}
		}*/

		if(!isClassRegistered(class_name)){

			string str_classPtr = typeid( _T *).name();

			if(getIdx_C_RegisteredClass(str_classPtr,false)!=-1){
				print_error_cr("this %s is already registered",demangle(typeid( _T).name()).c_str());
				return false;
			}


			//print_error_cr("CHECK AND TODOOOOOO!");
			tInfoRegisteredClass *irc = new tInfoRegisteredClass;

			irc->metadata_info.object_info.symbol_info.ast = new tASTNode;
			irc->metadata_info.object_info.symbol_info.info_var_scope=NULL;
			irc->metadata_info.object_info.symbol_info.symbol_name = class_name;
			//irc->baseClass = base_class; // identify extend class ?!?!!?

			// in C there's no script constructor ...
			irc->idx_function_script_constructor=-1;
			irc->c_constructor = new std::function<void *()>([](){return new _T;});
			irc->c_destructor = new std::function<void (void *)>([](void *p){delete (_T *)p;});

			irc->class_idx = m_registeredClass.size();

			irc->classPtrType=str_classPtr;
			irc->class_idx=m_registeredClass.size();
			irc->metadata_info.object_info.symbol_info.properties=PROPERTY_C_OBJECT_REF;

			irc->metadata_info.object_info.symbol_info.index=m_registeredClass.size();
			m_registeredClass.push_back(irc);

			print_info_cr("* class \"%10s\" registered as (%s).",class_name.c_str(),demangle(str_classPtr).c_str());

			return true;
		}
		else{
			print_error_cr("%s already exist", class_name.c_str());
		}

		return false;
	}

	template<class _T, class _B>
	bool class_C_baseof(){

		string base_class_name=typeid(_B).name();
		string base_class_name_ptr=typeid(_B *).name();
		string class_name=typeid(_T).name();
		string class_name_ptr=typeid(_T *).name();

		int base_class = this->getIdxClassFromIts_C_Type(typeid(_B *).name());
		if(base_class == -1) return false;


		int register_class = this->getIdxClassFromIts_C_Type(typeid(_T *).name());
		if(register_class == -1) return false;


		// check whether is in fact base of ...
		if(!std::is_base_of<_B,_T>::value){
			print_error_cr("class \"%s\" is not base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
			return false;
		}


		for(unsigned i = 0; i < m_registeredClass[register_class]->baseClass.size(); i++){
			if(m_registeredClass[register_class]->baseClass[i]->classPtrType ==base_class_name_ptr){
				print_error_cr("class \"%s\" already base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
				return false;
			}
		}


		/*if(!addPrimitiveTypeConversion<_T *,_B *>( [] (CScriptVariable *obj){return (int)reinterpret_cast<_B *>(obj);})){
			return false;
		}*/
	 	if(mapTypeConversion.count(class_name_ptr) == 1){ // create new map...
	 		if(mapTypeConversion[class_name_ptr].count(base_class_name_ptr)==1){
	 			print_error_cr("Conversion type \"%s\" -> \"%s\" already inserted",demangle(class_name).c_str(),demangle(base_class_name).c_str());
	 			return false;
	 		}
	 	}

	 	mapTypeConversion[class_name_ptr][base_class_name_ptr]=[](CScriptVariable *s){ return (int)reinterpret_cast<_B *>(s);};

	 	tInfoRegisteredClass *irc_base = m_registeredClass[base_class];
	 	tInfoRegisteredClass *irc_class = m_registeredClass[register_class];
	 	irc_class->baseClass.push_back(irc_base);

		// register all symbols function from base ...
		// vars ...
		for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){

			tInfoRegisteredVariableSymbol *irs_source = &irc_base->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			tInfoRegisteredVariableSymbol irs;
			// init struct...
			irs.class_info = m_registeredClass[base_class];
			irs.ref_ptr=irs_source->ref_ptr;
			irs.c_type = irs_source->c_type;
			//irs.
			irs.symbol_name=irs_source->symbol_name;
			irs.properties = PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
			irs.index = irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.size();
			irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);

		}

		// functions ...
		for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.m_registeredFunction.size(); i++){

			tInfoRegisteredFunctionSymbol *irs_source = &irc_base->metadata_info.object_info.local_symbols.m_registeredFunction[i];

			tInfoRegisteredFunctionSymbol irs;
			// init struct...
			irs.object_info.symbol_info.ast = NULL;
			irs.object_info.symbol_info.info_var_scope = NULL;
			irs.object_info.symbol_info.symbol_name=irs_source->object_info.symbol_info.symbol_name;


			irs.m_arg = irs_source->m_arg;
			irs.idx_return_type = irs_source->idx_return_type;

			irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_IS_DERIVATED;

			// ignores special type cast C++ member to ptr function
			// create binding function class
			irs.object_info.symbol_info.ref_ptr= irs_source->object_info.symbol_info.ref_ptr; // this is not correct due the pointer

			irs.object_info.symbol_info.index = irc_class->metadata_info.object_info.local_symbols.m_registeredFunction.size();
			irc_class->metadata_info.object_info.local_symbols.m_registeredFunction.push_back(irs);


		}
		return true;

	}


	template <class _T, class _R,typename _F>
	void * c_member_class_function_proxy(unsigned int n_args, _F fun_obj){
		using namespace std::placeholders;
		std::function<void *(void *,PROXY_CREATOR)> *c_function_builder=NULL;



		switch(n_args){
		case 0:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val= (void *)(new std::function<_R ()>(std::bind((_R (_T::*)())(fun_obj), (_T *)obj)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R ()> *)obj;
				}
				return return_val;
			});
			break;
		case 1:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int)>(std::bind((_R (_T::*)(int))(fun_obj), (_T *)obj, _1)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int)> *)obj;
				}
				return return_val;
			});
			break;
		case 2:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int)>(std::bind((_R (_T::*)(int,int))(fun_obj), (_T *)obj, _1,_2)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int)> *)obj;
				}
				return return_val;
			});
			break;
		case 3:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int)>(std::bind((_R (_T::*)(int,int,int))(fun_obj), (_T *)obj, _1,_2,_3)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int)> *)obj;
				}
				return return_val;
			});
			break;
		case 4:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int,int)> *)obj;
				}
				return return_val;
			});

			break;
		case 5:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val=  (void *)(new std::function<_R (int,int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4,_5)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<_R (int,int,int,int,int)> *)obj;
				}
				return return_val;
			});

			break;
		case 6:
			c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

				void *return_val=NULL;

				if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
					return_val= (void *)(new std::function<_R (int,int,int,int,int,int)>(std::bind((_R (_T::*)(int,int,int,int,int,int))(fun_obj), (_T *)obj, _1,_2,_3,_4,_5,_6)));
				}
				else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
					delete (std::function<int (int,int,int,int,int,int)> *)obj;
				}
				return return_val;
			});
			break;
		default:
			print_error_cr("Max argyments reached!");

		}



		return c_function_builder;
	}


	/**
	 * Register C Member function Class
	 */
	template <class _T, typename F>
	bool register_C_FunctionMemberInt(const char *function_name,F function_type)
	{
		string return_type;
		vector<string> params;
		tInfoRegisteredFunctionSymbol irs;
		string str_classPtr = typeid( _T *).name();

		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.
		using Traits3 = function_traits<decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});


		// check valid parameters ...
		if((irs.idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
			print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
			return false;
		}

		for(unsigned int i = 0; i < irs.m_arg.size(); i++){
			if(getIdxClassFromIts_C_Type(irs.m_arg[i])==-1){
				print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(irs.m_arg[i]).c_str(),function_name);
				return false;
			}

		}

		// init struct...
		irs.object_info.symbol_info.ast = NULL;
		irs.object_info.symbol_info.info_var_scope = NULL;
		irs.object_info.symbol_info.symbol_name=function_name;


		irs.object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF;

		// ignores special type cast C++ member to ptr function
		// create binding function class
		if(irs.idx_return_type == getIdxClassVoid()){
			if((irs.object_info.symbol_info.ref_ptr=((int)c_member_class_function_proxy<_T, void>(irs.m_arg.size(),function_type)))==0){
				return false;
			}
		}else{
			if((irs.object_info.symbol_info.ref_ptr=((int)c_member_class_function_proxy<_T, int>(irs.m_arg.size(),function_type)))==0){
				return false;
			}
		}

		irs.object_info.symbol_info.index = m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredFunction.size();
		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredFunction.push_back(irs);
		//base_info->local_symbols.m_registeredFunction.push_back(irs);
		print_info_cr("Registered member function name %s::%s",demangle(typeid(_T).name()).c_str(), function_name);

		return true;
	}

	/**
	 * Register C Member var
	 */


	template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
	{
	    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
	}

	template <class _T, typename _V>
	bool register_C_VariableMemberInt(const char *var_name, unsigned int offset)
	{
		string var_type = typeid(_V *).name(); // we need the pointer type ...
		//decltype(var_type) var;
		//print_info_cr("%s",typeid(var).name());
		//string str_type = typeid(decltype(var_type)).name();

		//unsigned int offset = offsetOf(var_type);


		string return_type;
		vector<string> params;
		tInfoRegisteredVariableSymbol irs;
		string str_classPtr = typeid( _T *).name();



		int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

		if(idxRegisterdClass == -1){
			return false;
		}

		// 1. check all parameters ok.

		// check valid parameters ...
		if(getIdxClassFromIts_C_Type(var_type) == -1){
			print_error_cr("%s::%s has not valid type (%s)",m_registeredClass[idxRegisterdClass]->metadata_info.object_info.symbol_info.symbol_name.c_str(),var_name,demangle(typeid(_V).name()).c_str());
			return false;
		}



		// init struct...
		irs.class_info = m_registeredClass[idxRegisterdClass];
		irs.ref_ptr=offset;
		irs.c_type = var_type;
		//irs.
		irs.symbol_name=var_name;


		irs.properties = PROPERTY_C_OBJECT_REF;


		irs.index = m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.size();
		m_registeredClass[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);
		//base_info->local_symbols.m_registeredFunction.push_back(irs);

		return true;


	}


private:

	static CScriptClassFactory *scriptClassFactory;
	static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
	static map<string,map<string,fntConversionType>> mapTypeConversion;

	 template<typename _S, typename _D, typename _F>
	 static bool addPrimitiveTypeConversion(_F f){

	 	bool valid_type = false;

	 	// check if any entry is int, *float, *bool , *string, *int or any from factory. Anyelese will be no allowed!
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::VOID_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(void).name(),"void",VOID_TYPE};
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::INT_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int).name(),"int",INT_TYPE};
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::INT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int *).name(),"int *",INT_PTR_TYPE};
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::FLOAT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::STRING_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(string *).name(),"string *",STRING_PTR_TYPE};
	 	valid_type|=CScriptClassFactory::valid_C_PrimitiveType[CScriptClassFactory::BOOL_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};

	 	if(!valid_type){
	 		print_error_cr("Conversion type \"%s\" not valid",typeid(_D).name());
	 		return false;
	 	}



	 	if(mapTypeConversion.count(typeid(_S).name()) == 1){ // create new map...
	 		if(mapTypeConversion[typeid(_S).name()].count(typeid(_D).name())==1){
	 			print_error_cr("type conversion \"%s\" to \"%s\" already inserted",typeid(_S).name(),typeid(_D).name());
	 			return false;
	 		}
	 	}

	 	mapTypeConversion[typeid(_S).name()][typeid(_D).name()]=f;

	 	return true;
	 	//typeConversion["P7CNumber"]["Ss"](&n);
	 }

	int getIdxRegisteredClass_Internal(const string & class_name);
	//int getIdxRegisteredFunctionSymbol_Internal(const string & class_name,const string & function_name);
	//int getIdxRegisteredVariableSymbol_Internal(const string & class_name,const string & var_name);

	vector<tInfoRegisteredClass *>  	 m_registeredClass;
	//CScriptVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

	bool searchVarFunctionSymbol(tScriptFunctionInfo *script_info, tInfoAsmOp *iao, int current_idx_function);
	bool registerBase();
	void buildInfoScopeVariablesBlock(tInfoRegisteredFunctionSymbol *irfs );

	//---------------
	// PRINT ASM INFO
	char print_aux_load_value[1024*8];
	const char * getStrMovVar(tInfoAsmOp * iao);
	const char * getStrTypeLoadValue(vector<tInfoStatementOp> * m_listStatements,int current_statment, int current_instruction);
	void printGeneratedCode_Recursive(tScriptFunctionInfo *fs);
	void printGeneratedCode(tScriptFunctionInfo *fs);
	// PRINT ASM INFO
	//---------------

	CScriptClassFactory();
	~CScriptClassFactory();



};
