/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



/*
//	______           _     _                    _   _____ _
//	| ___ \         (_)   | |                  | | /  __ \ |
//	| |_/ /___  __ _ _ ___| |_ ___ _ __ ___  __| | | /  \/ | __ _ ___ ___
//	|    // _ \/ _` | / __| __/ _ \ '__/ _ \/ _` | | |   | |/ _` / __/ __|
//	| |\ \  __/ (_| | \__ \ ||  __/ | |  __/ (_| | | \__/\ | (_| \__ \__ \
//	\_| \_\___|\__, |_|___/\__\___|_|  \___|\__,_|  \____/_|\__,_|___/___/
//            __/ |
//           |___/
// _________________________________________________
//  __________________________________
*/



#include "CNativeFunction.h"
#include "../helpers/RegisterFunctionHelper.h"


#define register_C_Function(text,s) zetscript::CScriptClass::register_C_FunctionInt(text,s)
#define register_C_Variable(text,s) zetscript::CScriptClass::register_C_VariableInt(text,&s,typeid(decltype(&s)).name())

#define getIdxGlobalVariable(s)  zetscript::CScriptClass::register_C_FunctionInt(STR(s),s)
#define getIdxGlobalFunction(s)


#define register_C_Class 						zetscript::CScriptClass::register_C_ClassInt
#define register_C_SingletonClass				zetscript::CScriptClass::register_C_SingletonClassInt

#define register_C_VariableMember	 			zetscript::CScriptClass::register_C_VariableMemberInt
#define register_C_FunctionMember				zetscript::CScriptClass::register_C_FunctionMemberInt //<o>(s,&f)
#define class_C_baseof							zetscript::CScriptClass::class_C_baseofInt //<o>(s,&f)



#define register_C_StaticFunctionMember			zetscript::CScriptClass::register_C_StaticFunctionMemberInt



#define GET_MAIN_VARIABLE(idx_var)				zetscript::CScriptClass::getVariableClass(IDX_CLASS_MAIN,idx_var)
#define GET_MAIN_SCRIPT_FUNCTION_IDX			zetscript::CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)
#define GET_MAIN_FUNCTION_OBJECT				GET_SCRIPT_FUNCTION_OBJECT(GET_MAIN_SCRIPT_FUNCTION_IDX)


#define NEW_CLASS_VAR_BY_IDX(idx) 				(zetscript::CScriptClass::instanceScriptVariableByIdx(idx))

#define REGISTERED_CLASS_NODE(idx) 				(zetscript::CScriptClass::getScriptClassByIdx(idx))
#define MAIN_CLASS_NODE							(zetscript::CScriptClass::getScriptClassByIdx(IDX_CLASS_MAIN))    // 0 is the main class
#define GET_SCRIPT_CLASS_INFO(idx)				(zetscript::CScriptClass::getScriptClassByIdx(idx))    // 0 is the main class
#define GET_SCRIPT_CLASS_INFO_BY_NAME(s)		(zetscript::CScriptClass::getScriptClassByName(s))    // 0 is the main class
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(s)	(zetscript::CScriptClass::getScriptClassBy_C_ClassPtr(s))    // 0 is the main class

#define GET_IDX_2_CLASS_C_STR(idx) (CScriptClass::getScriptClassByIdx(idx)->classPtrType)

namespace zetscript{


/**
 * Stores the basic information to build a object through built AST structure
 */



	class  CScriptClass{


		//------------- VARIABLES STRUCT ---------------
	public:

		static string  *VOID_TYPE_STR;// 		typeid(void).name()
		static string  *INT_PTR_TYPE_STR;//	typeid(int *).name()
		static string  *FLOAT_PTR_TYPE_STR;//	typeid(float *).name()
		static string  *STRING_PTR_TYPE_STR;//	typeid(string *).name()
		static string  *CONST_CHAR_PTR_TYPE_STR;//	typeid(string *).name()
		static string  *BOOL_PTR_TYPE_STR;//	typeid(bool *).name()
		static string  *INT_TYPE_STR;//	typeid(int).name()
		static string  *FLOAT_TYPE_STR;//	typeid(int).name()
		static string  *BOOL_TYPE_STR;//	typeid(bool).name()
		static string  *STACK_ELEMENT_PTR;//	typeid(bool).name()
		static char     registered_metamethod[MAX_METAMETHOD_OPERATORS][50];

		CScriptFunctionObject	metadata_info;
		int idx_function_script_constructor;
		//int idxScriptClass;

		std::function<void * ()> 		* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		string classPtrType; // type_id().name();
		int idxClass;
		vector<CScriptClass *> baseClass; // in the case is and extension of class.

		vector<int> metamethod_operator[MAX_METAMETHOD_OPERATORS]; // overrided metamethod

		CScriptClass();

		bool is_c_class();


		//------------- STATIC METHODS ---------------

	private:
		static vector<CScriptClass *> 			* vec_script_class_node;
	public:

		// DEFINES



		// STRUCTS

		typedef struct{
			const char *   type_str;
			BASIC_CLASS_TYPE  id;
		}tPrimitiveType;

		typedef struct{
			tPrimitiveType 				*return_type;
			vector<tPrimitiveType*>		params;
		}tRegisterFunction;


		// HELPERS
		static tStackElement 						C_REF_InfoVariable_2_StackElement(tInfoVariableSymbol *ir_var, void *ptr_variable);


		// FUNCTIONS
		static void 								setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec);
		static vector<CScriptClass *> 		*		getVectorScriptClassNode();

		/**
		 * Class Manager
		 */
		/**
		 * This function registers a script class into factory.
		 */
		static CScriptClass 				* 		registerClass(const string & class_name, const string & base_class_name, PASTNode _ast);


		static CScriptClass 				* 		getScriptClassByIdx(int idx);
		static CScriptClass 				* 		getScriptClassByName(const string & name, bool print_msg=true);
		static CScriptClass 				* 		getScriptClassBy_C_ClassPtr(const string & class_type, bool print_msg=true);
		static int 									getIdxScriptClass_Internal(const string & class_name);
		static int 									getIdxScriptClass(const string & v, bool print_msg=true);
		static int 									getIdxClassFromIts_C_TypeInternal(const string & c_type_str);
		static int 									getIdxClassFromIts_C_Type(const string & s);


		static bool 								idxClassInstanceofIdxClass(int theClass,int class_idx);



		//static BASIC_CLASS_TYPE			getIdxPrimitiveFromIts_C_TypeInternal(const string & c_type_str);
		static bool 								isClassRegistered(const string & v);




		//static tPrimitiveType valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];
		static void registerPrimitiveTypes();


		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		static CScriptVariable 		 * instanceScriptVariableByClassName(const string & class_name);
		static CScriptVariable 		 * instanceScriptVariableByIdx(int idx_class, void * value_object = NULL);
		static CScriptVariable 		 * getScriptVariableByIdx(int idx_class, int idx_var);

		static bool updateReferenceSymbols();

		static tInfoVariableSymbol  * registerVariableSymbol(const string & class_name,const string & name,short  idxAstNode);
		static tInfoVariableSymbol *  getRegisteredVariableSymbol(const string & class_name,const string & varname);
		static int 							 getIdxRegisteredVariableSymbol(const string & class_name,const string & varname, bool show_msg=true);
		static int 							 getIdxRegisteredVariableSymbol(tFunctionInfo *irf,const string & var_name, bool show_msg=true);


		static CScriptFunctionObject *  registerFunctionSymbol(const string & class_name, const string & name,short idxAstNode);
		static int getIdxScriptFunctionObjectByClassFunctionName_Internal(CScriptClass *rc,const string & function_name, bool show_errors=true);
		static int getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors=true);
		static CScriptFunctionObject * getScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors=true);
		static CScriptFunctionObject * getScriptFunctionObjectByClassIdxFunctionName(int idxClassName,const string & function_name, bool show_errors=true);

		static tFunctionInfo *  getSuperClass(CScriptClass *irc, const string & fun_name);


		static bool addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);


		//CScriptClass * 	getRegisteredClass(const string & v, bool print_msg=true);
		//CScriptClass *	getRegisteredClassByIdx(unsigned index);
		//CScriptClass *  getRegisteredClassBy_C_ClassPtr(const string & v, bool print_msg=true);

		static int 					getIdxRegisteredClass(const string & v, bool print_msg=true);
		static fntConversionType getConversionType(string objectType, string conversionType, bool show_errors=true);

		static const char * getNameRegisteredClassByIdx(int idx);

		// internal var types ...
		//static CScriptClass *  getRegisteredClassVoid();
		static CScriptClass *  getRegisteredClassUndefined();
		//static CScriptClass *  getRegisteredClassInteger();
		//static CScriptClass *  getRegisteredClassNumber();
		static CScriptClass *  getRegisteredClassStruct();
		static CScriptClass *  getRegisteredClassString();
		//static CScriptClass *  getRegisteredClassBoolean();
		static CScriptClass *  getRegisteredClassVector();
		static CScriptClass *  getRegisteredClassFunctor();
		static CScriptClass *  getRegisteredClassNull();


		/**
		 * Register C function
		 */
		template <typename F>
		static bool register_C_FunctionInt(const string & function_name,F function_ptr)
		{
			int idx_return_type=-1;
			string return_type;
			vector<string> m_arg;
			vector<int> m_idxArg;
			intptr_t ref_ptr=-1;
			CScriptFunctionObject *irs=NULL;

			//tPrimitiveType *rt;
			//vector<tPrimitiveType *> pt;
			//CScope::tScopeVar  *rs;

			//CScriptVariable *sf=NULL;
			CScriptFunctionObject * mainFunctionInfo = getScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);
			//CScriptClass *rc = CScriptClass::getInstance()->getRegisteredClass(class_name);

			if(mainFunctionInfo == NULL){
				zs_print_error_cr("main function is not created");
				exit(EXIT_FAILURE);
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_ptr)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type))==-1){
				zs_print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name.c_str());
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type = getIdxClassFromIts_C_Type(m_arg[i]);
				if(idx_type ==-1){
					zs_print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name.c_str());
					return false;
				}

				m_idxArg.push_back(idx_type);
			}

			if(idx_return_type == IDX_CLASS_VOID_C){
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<void>(m_arg.size(),function_ptr))==0){//(int)function_ptr;
					return false;
				}
			}
			else{
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<int>(m_arg.size(),function_ptr))==0){//(int)function_ptr;
					return false;
				}
			}


			// init struct...
			irs = NEW_SCRIPT_FUNCTION_OBJECT;

			irs->m_arg = m_idxArg;
			irs->idx_return_type = idx_return_type;
			irs->object_info.symbol_info.ref_ptr = ref_ptr;

			irs->object_info.symbol_info.idxAstNode = -1;
			irs->object_info.symbol_info.symbol_name = function_name;
			irs->object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;

			irs->object_info.symbol_info.idxSymbol = (short)(mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size());
			mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);

			zs_print_debug_cr("Registered function name: %s",function_name.c_str());
			return true;
		}

		/**
		 * Register C variable
		 */
		static bool register_C_VariableInt(const string & var_str,void * var_ptr, const string & var_type);
		static int getIdx_C_RegisteredClass(const string & str_classPtr, bool print_msg=true);
		static int getIdx_C_RegisteredFunctionMemberClass(const string & str_classPtr, const string & str_functionName, bool print_msg=true);


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		static bool register_C_SingletonClassInt(const string & class_name){//, const string & base_class_name=""){

			if(!isClassRegistered(class_name)){

				string str_classPtr = typeid( _T *).name();

				if(getIdx_C_RegisteredClass(str_classPtr,false)!=-1){
					zs_print_error_cr("this %s is already registered",demangle(typeid( _T).name()).c_str());
					return false;
				}

				//zs_print_error_cr("CHECK AND TODOOOOOO!");
				CScriptClass *irc = new CScriptClass;

				CASTNode *ast =CASTNode::newASTNode();
				CASTNode *ast_var_symbols =CASTNode::newASTNode();
				CASTNode *ast_collection_var_symbols =CASTNode::newASTNode();
				CASTNode *ast_fun_symbols =CASTNode::newASTNode();

				// push var & fun symbols node...
				ast_var_symbols->children.push_back(ast_collection_var_symbols->idxAstNode);
				ast->children.push_back(ast_var_symbols->idxAstNode);
				ast->children.push_back(ast_fun_symbols->idxAstNode);

				irc->metadata_info.object_info.symbol_info.idxAstNode = ast->idxAstNode;
				//irc->metadata_info.object_info.symbol_info.idxScopeVar=-1;
				irc->metadata_info.object_info.symbol_info.symbol_name = class_name;
				//irc->baseClass = base_class; // identify extend class ?!?!!?
				// in C there's no script constructor ...
				irc->idx_function_script_constructor=-1;
				// allow dynamic constructor in function its parameters ...


				irc->metadata_info.object_info.symbol_info.idxScriptClass = (short)((*vec_script_class_node).size());
				irc->classPtrType=str_classPtr;
				irc->metadata_info.object_info.symbol_info.properties=PROPERTY_C_OBJECT_REF;


				irc->c_constructor = NULL;
				irc->c_destructor = NULL;
				(*vec_script_class_node).push_back(irc);

				irc->idxClass=(*vec_script_class_node).size()-1;
				zs_print_debug_cr("* C++ class \"%s\" registered as (%s).",class_name.c_str(),demangle(str_classPtr).c_str());

				return true;
			}
			else{
				zs_print_error_cr("%s already exist", class_name.c_str());
			}

			return false;
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		static bool register_C_ClassInt(const string & class_name){//, const string & base_class_name=""){
			if(register_C_SingletonClass<_T>(class_name)){
				CScriptClass *irc = CScriptClass::getScriptClassByName(class_name);

				//put the constructor/destructor...
				irc->c_constructor = new std::function<void *()>([](){return new _T;});
				irc->c_destructor = new std::function<void (void *)>([](void *p){delete (_T *)p;});

				return true;
			}

			return false;
		}


		template<class _T, class _B>
		static bool class_C_baseofInt(){

			string base_class_name=typeid(_B).name();
			string base_class_name_ptr=typeid(_B *).name();
			string class_name=typeid(_T).name();
			string class_name_ptr=typeid(_T *).name();

			int idxBaseClass = getIdxClassFromIts_C_Type(typeid(_B *).name());
			if(idxBaseClass == -1) return false;


			int register_class = getIdxClassFromIts_C_Type(typeid(_T *).name());
			if(register_class == -1) return false;


			// check whether is in fact base of ...
			if(!std::is_base_of<_B,_T>::value){
				zs_print_error_cr("C++ class \"%s\" is not base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
				return false;
			}

			for(unsigned i = 0; i < (*vec_script_class_node)[register_class]->baseClass.size(); i++){
				if((*vec_script_class_node)[register_class]->baseClass[i]->classPtrType ==base_class_name_ptr){
					zs_print_error_cr("C++ class \"%s\" already base of \"%s\" ",demangle(class_name).c_str(), demangle(base_class_name).c_str());
					return false;
				}
			}

			if(mapTypeConversion.count(class_name_ptr) == 1){ // create new map...
				if(mapTypeConversion[class_name_ptr].count(base_class_name_ptr)==1){
					zs_print_error_cr("Conversion type \"%s\" -> \"%s\" already inserted",demangle(class_name).c_str(),demangle(base_class_name).c_str());
					return false;
				}
			}

			mapTypeConversion[class_name_ptr][base_class_name_ptr]=[](CScriptVariable *s){ return (intptr_t)reinterpret_cast<_B *>(s);};

			CScriptClass *irc_base = (*vec_script_class_node)[idxBaseClass];
			CScriptClass *irc_class = (*vec_script_class_node)[register_class];
			irc_class->baseClass.push_back(irc_base);

			// register all symbols function from base ...
			// vars ...
			for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){

				tInfoVariableSymbol *irs_source = &irc_base->metadata_info.object_info.local_symbols.m_registeredVariable[i];

				tInfoVariableSymbol irs;
				// init struct...
				irs.idxScriptClass = idxBaseClass;//.class_info = (*vec_script_class_node)[base_class];
				irs.ref_ptr=irs_source->ref_ptr;
				irs.c_type = irs_source->c_type;
				//irs.
				irs.symbol_name=irs_source->symbol_name;
				irs.properties = PROPERTY_C_OBJECT_REF| PROPERTY_IS_DERIVATED;
				irs.idxSymbol = (short)(irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.size());
				irc_class->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);

			}

			// functions ...
			for(unsigned i = 0; i < irc_base->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){

				CScriptFunctionObject *irs_source = GET_SCRIPT_FUNCTION_OBJECT(irc_base->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i]);

				CScriptFunctionObject *irs=NEW_SCRIPT_FUNCTION_OBJECT;
				// init struct...
				irs->object_info.symbol_info.idxAstNode = -1;
				//irs.object_info.symbol_info.idxScopeVar = -1;
				irs->object_info.symbol_info.symbol_name=irs_source->object_info.symbol_info.symbol_name;


				irs->m_arg = irs_source->m_arg;
				irs->idx_return_type = irs_source->idx_return_type;

				irs->object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_IS_DERIVATED;

				// ignores special type cast C++ member to ptr function
				// create binding function class
				irs->object_info.symbol_info.ref_ptr= irs_source->object_info.symbol_info.ref_ptr; // this is not correct due the pointer

				irs->object_info.symbol_info.idxSymbol = (short)(irc_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size());
				irc_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);


			}
			return true;

		}


		/**
		 * Register C Member function Class
		 */
		template < typename _R, class _T, typename..._A>
		static bool register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) )
		{
			string return_type;
			vector<string> params;
			CScriptFunctionObject *irs=NULL;
			vector<string> m_arg;
			vector<int> m_idxArg;
			int idx_return_type=-1;
			unsigned int ref_ptr=-1;
			string str_classPtr = typeid( _T *).name();

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
				return false;
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_type)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
				zs_print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type=getIdxClassFromIts_C_Type(m_arg[i]);
				if(idx_type==-1){
					zs_print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_idxArg.push_back(idx_type);

			}

			// ignores special type cast C++ member to ptr function
			// create binding function class
			if(idx_return_type == IDX_CLASS_VOID_C){
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_T, void>(m_arg.size(),function_type)))==0){
					return false;
				}
			}else{
				if((ref_ptr=((intptr_t)CNativeFunction::getInstance()->c_member_class_function_proxy<_T, intptr_t>(m_arg.size(),function_type)))==0){
					return false;
				}
			}

			// ok, function candidate to be added into class...
			irs = NEW_SCRIPT_FUNCTION_OBJECT;//CScriptFunctionObject::newScriptFunctionObject();

			// init struct...
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->symbol_value = function_name;
			// get ast symbols function member node...
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.symbol_info.idxAstNode)->children[1]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);



			//irs.object_info.symbol_info.idxScopeVar = -1;
			irs->object_info.symbol_info.symbol_name=function_name;
			irs->object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF;

			irs->object_info.symbol_info.ref_ptr = ref_ptr;
			irs->m_arg = m_idxArg;
			irs->idx_return_type = idx_return_type;

			irs->object_info.symbol_info.idxSymbol = (short)((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size());
			(*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);
			zs_print_debug_cr("Registered member function name %s::%s",demangle(typeid(_T).name()).c_str(), function_name);

			if(STRCMP(registered_metamethod[SET_METAMETHOD],==,function_name)){
				(*vec_script_class_node)[idxRegisterdClass]->metamethod_operator[SET_METAMETHOD].push_back(irs->object_info.idxScriptFunctionObject);
				zs_print_debug_cr("Registered metamethod %s::%s",demangle(typeid(_T).name()).c_str(), function_name);
			}


			return true;
		}


		/**
		 * Register C Member function Class
		 */
		template < class _T, typename _F>
		static bool register_C_StaticFunctionMemberInt(const char *function_name,_F function_type)
		{
			string return_type;
			vector<string> params;
			CScriptFunctionObject *irs=NULL;
			vector<string> m_arg;
			vector<int> m_idxArg;
			int idx_return_type=-1;
			unsigned int ref_ptr=-1;
			string str_classPtr = typeid( _T *).name();

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
				return false;
			}

			// 1. check all parameters ok.
			using Traits3 = function_traits<decltype(function_type)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


			// check valid parameters ...
			if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
				zs_print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
				return false;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				int idx_type = getIdxClassFromIts_C_Type(m_arg[i]);
				if(idx_type==-1){
					zs_print_error_cr("Argument (%i) type \"%s\" for function \"%s\" not registered",i,demangle(m_arg[i]).c_str(),function_name);
					return false;
				}

				m_idxArg.push_back(idx_type);

			}

			// ignores special type cast C++ member to ptr function
			// create binding function class
			if(idx_return_type == IDX_CLASS_VOID_C){
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<void>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}
			else{
				if((ref_ptr=(intptr_t)CNativeFunction::getInstance()->new_proxy_function<int>(m_arg.size(),function_type))==0){//(int)function_ptr;
					return false;
				}
			}

			// ok, function candidate to be added into class...
			irs = NEW_SCRIPT_FUNCTION_OBJECT;//CScriptFunctionObject::newScriptFunctionObject();

			// init struct...
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->symbol_value = function_name;
			// get ast symbols function member node...
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.symbol_info.idxAstNode)->children[1]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);



			//irs.object_info.symbol_info.idxScopeVar = -1;
			irs->object_info.symbol_info.symbol_name=function_name;
			irs->object_info.symbol_info.properties = PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;

			irs->object_info.symbol_info.ref_ptr = ref_ptr;
			irs->m_arg = m_idxArg;
			irs->idx_return_type = idx_return_type;
			irs->object_info.symbol_info.idxSymbol = (short)((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size());
			(*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);
			zs_print_debug_cr("Registered member function name %s::%s",demangle(typeid(_T).name()).c_str(), function_name);

			// check whether is static metamethod...
			if(STRCMP(registered_metamethod[SET_METAMETHOD],!=,function_name)){

				for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
					if(STRCMP(registered_metamethod[i],==,function_name)){

						// check if they are gte,gt,equ, not_equ, lt, lte
						if(  i == EQU_METAMETHOD //STRCMP(function_name, == ,"_equ")
						  || i == NOT_EQU_METAMETHOD //STRCMP(function_name, ==, "_nequ")
						  || i == LT_METAMETHOD//STRCMP(function_name, ==, "_lt")
						  || i == LTE_METAMETHOD//STRCMP(function_name, ==, "_lte")
						  || i == GT_METAMETHOD//STRCMP(function_name, ==, "_gt")
						  || i == GTE_METAMETHOD//STRCMP(function_name, ==, "_gte")
						  || i == NOT_METAMETHOD//STRCMP(function_name, ==, "_gte")
						  ){
							// return type must be bool...
							if(STRCMP(return_type.c_str(), != ,typeid(bool).name())){
								zs_print_error_cr("error registering metamethod %s::%s. Expected return bool but it was %s",
										demangle(typeid(_T).name()).c_str(),
										function_name,
										demangle(return_type.c_str()).c_str());
								return false;

							}
						}else if((return_type != str_classPtr) && (i!= SET_METAMETHOD)){

							zs_print_error_cr("error registering metamethod %s::%s. Expected return %s but it was %s",
									demangle(typeid(_T).name()).c_str(),
									function_name,
									demangle(str_classPtr.c_str()).c_str(),
									demangle(return_type.c_str()).c_str());
							return false;
						}

						(*vec_script_class_node)[idxRegisterdClass]->metamethod_operator[i].push_back(irs->object_info.idxScriptFunctionObject);

						zs_print_debug_cr("Registered metamethod %s::%s",demangle(typeid(_T).name()).c_str(), function_name);
						break;
					}
				}
			}else{
				zs_print_error_cr("error! cannot register metamethod set on static function. Must be member function!");
				return false;
			}

			return true;
		}

		/**
		 * Register C Member var
		 */
		//<o, decltype(o::s)>(STR(s),zetscript::offset_of(&o::s)) &CVar::mierda
		template <typename _R,typename _T>
		static bool register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer) //unsigned int offset)
		{
			string var_type = typeid(_R *).name(); // we need the pointer type ...
			string return_type;
			vector<string> params;
			tInfoVariableSymbol irs;
			string str_classPtr = typeid( _T *).name();
			unsigned int offset=zetscript::offset_of(var_pointer);

			int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

			if(idxRegisterdClass == -1){
				return false;
			}

			// 1. check all parameters ok.

			// check valid parameters ...
			if(getIdxClassFromIts_C_Type(var_type) == -1){
				zs_print_error_cr("%s::%s has not valid type (%s)"
						,(*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.symbol_info.symbol_name.c_str()
						,var_name
						,demangle(typeid(_R).name()).c_str());
				return false;
			}

			// init struct...
			irs.idxScriptClass = idxRegisterdClass;//(*vec_script_class_node)[idxRegisterdClass];
			irs.ref_ptr=offset;
			irs.c_type = var_type;
			//irs.
			irs.symbol_name=var_name;

			// init ast
			CASTNode *ast_symbol = CASTNode::newASTNode();
			ast_symbol->symbol_value = var_name;

			// get ast var symbol collection node ( because class has a var collection we need a children [0] )
			CASTNode *ast_symbol_node =AST_NODE(AST_NODE(AST_NODE((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.symbol_info.idxAstNode)->children[0])->children[0]);
			ast_symbol_node->children.push_back(ast_symbol->idxAstNode);


			irs.properties = PROPERTY_C_OBJECT_REF;
			irs.idxSymbol = (short)((*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.size());
			(*vec_script_class_node)[idxRegisterdClass]->metadata_info.object_info.local_symbols.m_registeredVariable.push_back(irs);
			//base_info->local_symbols.vec_idx_registeredFunction.push_back(irs);
			return true;

		}

		static bool init();
		static void destroySingletons();


	private:


		static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
		static map<string,map<string,fntConversionType>> mapTypeConversion;

		 template<typename _S, typename _D, typename _F>
		 static bool addPrimitiveTypeConversion(_F f){

			//bool valid_type = false;

			// check if any entry is int, *float, *bool , *string, *int or any from factory. Anyelese will be no allowed!
		 /*	valid_type|=valid_C_PrimitiveType[IDX_CLASS_VOID_C].type_str==string(typeid(_D).name()); ;//={typeid(void).name(),"void",IDX_CLASS_VOID_C};
			//valid_type|=valid_C_PrimitiveType[INT_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int).name(),"int",INT_TYPE};
			valid_type|=valid_C_PrimitiveType[IDX_CLASS_INT_PTR_C].type_str==string(typeid(_D).name()); ;//={typeid(int *).name(),"int *",IDX_CLASS_INT_PTR_C};
			valid_type|=valid_C_PrimitiveType[IDX_CLASS_FLOAT_PTR_C].type_str==string(typeid(_D).name()); ;//={typeid(float *).name(),"float *",IDX_CLASS_FLOAT_PTR_C};
			valid_type|=valid_C_PrimitiveType[IDX_CLASS_STRING_PTR_C].type_str==string(typeid(_D).name()); ;//={typeid(string *).name(),"string *",IDX_CLASS_STRING_PTR_C};
			valid_type|=valid_C_PrimitiveType[IDX_CLASS_BOOL_PTR_C].type_str==string(typeid(_D).name()); ;//={typeid(bool *).name(),"bool *",IDX_CLASS_BOOL_PTR_C};

			if(!valid_type){
				zs_print_error_cr("Conversion type \"%s\" not valid",typeid(_D).name());
				return false;
			}*/



			if(mapTypeConversion.count(typeid(_S).name()) == 1){ // create new map...
				if(mapTypeConversion[typeid(_S).name()].count(typeid(_D).name())==1){
					zs_print_error_cr("type conversion \"%s\" to \"%s\" already inserted",typeid(_S).name(),typeid(_D).name());
					return false;
				}
			}

			mapTypeConversion[typeid(_S).name()][typeid(_D).name()]=f;

			return true;
			//typeConversion["P7CNumber"]["Ss"](&n);
		 }

		//int getIdxRegisteredClass_Internal(const string & class_name);
		//int getidxScriptFunctionObject_Internal(const string & class_name,const string & function_name);
		//int getIdxRegisteredVariableSymbol_Internal(const string & class_name,const string & var_name);

		//vector<CScriptClass *>  	 (*vec_script_class_node);
		//CScriptVariable * createObjectFromPrimitiveType(tPrimitiveType *pt);

		 static bool searchVarFunctionSymbol(tFunctionInfo * info_function, tInfoAsmOp *iao, int current_idx_function,bool & symbol_not_found, unsigned int scope_type=0);

		 static void buildScopeVariablesBlock(CScriptFunctionObject *root_class_irfs );
		 static void unloadRecursiveFunctions(CScriptFunctionObject * info_function);

		 static bool updateFunctionSymbols(int idxSxriptFunctionObject, const string & parent_symbol, int n_function);// is_main_class, bool is_main_function);



		//CScriptClass();
		//~CScriptClass();



	};
}
