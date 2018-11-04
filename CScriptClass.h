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
#include "helper/RegisterFunctionHelper.h"


#define MAX_REGISTER_CLASSES 200

#define ZS_INVALID_CLASS	((unsigned char)ZS_UNDEFINED_IDX)

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



#define MAIN_VARIABLE(idx_var)					zetscript::CScriptClass::getVariableClass(IDX_CLASS_MAIN,idx_var)
#define MAIN_SCRIPT_FUNCTION_IDX				zetscript::CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)
#define MAIN_SCRIPT_FUNCTION_OBJECT				GET_SCRIPT_FUNCTION(MAIN_SCRIPT_FUNCTION_IDX)


#define NEW_CLASS_VAR_BY_IDX(idx) 				(zetscript::CScriptClass::instanceScriptVariableByIdx(idx))

#define REGISTERED_CLASS_NODE(idx) 				(zetscript::CScriptClass::getScriptClassByIdx(idx))
#define MAIN_CLASS_NODE							(zetscript::CScriptClass::getScriptClassByIdx(IDX_CLASS_MAIN))    // 0 is the main class

#define GET_SCRIPT_CLASS_INFO(idx)				(zetscript::CScriptClass::getScriptClassByIdx(idx))    // 0 is the main class
#define GET_SCRIPT_CLASS_INFO_BY_NAME(s)		(zetscript::CScriptClass::getScriptClassByName(s))    // 0 is the main class
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(s)	(zetscript::CScriptClass::getScriptClassBy_C_ClassPtr(s))    // 0 is the main class

#define GET_IDX_2_CLASS_C_STR(idx) 				(CScriptClass::getScriptClassByIdx(idx)->classPtrType)

#define register_C_BaseSymbols(o)		   		(zetscript::CScriptClass::register_C_BaseSymbolsInt(o))


namespace zetscript{


/**
 * Stores the basic information to build a object through built AST structure
 */



	class  CScriptClass:public CScriptFunction{

	private:

		static 	bool 	register_c_base_symbols;
		static 			vector<CScriptClass *> 			* vec_script_class_node;
		ZETSCRIPT_MODULE_EXPORT static vector<CScriptClass *> * getVecScriptClassNode();
		ZETSCRIPT_MODULE_EXPORT static map<int, map<int, fntConversionType>>  *	 getMapTypeConversion();
		static void  print(const char *s);
		static void (* print_out_callback)(const char *);
		//------------- VARIABLES STRUCT ---------------
	public:

		static string  *VOID_TYPE_STR;			// 	typeid(void).name()
		static string  *INT_PTR_TYPE_STR;		//	typeid(int *).name()
		static string  *FLOAT_PTR_TYPE_STR;		//	typeid(float *).name()
		static string  *STRING_PTR_TYPE_STR;	//	typeid(string *).name()
		static string  *CONST_CHAR_PTR_TYPE_STR;//	typeid(string *).name()
		static string  *BOOL_PTR_TYPE_STR;		//	typeid(bool *).name()
		static string  *INT_TYPE_STR;			//	typeid(int).name()
		static string  *UNSIGNED_INT_TYPE_STR;	//	typeid(unsigned int).name()
		static string  *INTPTR_T_TYPE_STR;		//	typeid(intptr_t).name()

		static string  *FLOAT_TYPE_STR;			//	typeid(int).name()
		static string  *BOOL_TYPE_STR;			//	typeid(bool).name()
		static string  *STACK_ELEMENT_PTR;		//	typeid(bool).name()


		ZETSCRIPT_MODULE_EXPORT static void setPrintOutCallback(void (*)(const char *));

		ZETSCRIPT_MODULE_EXPORT static const char * getMetamethod(METAMETHOD_OPERATOR op);

		//tScopeInfo			scope_info;
		//tVariableSymbolInfo symbol_info;
		unsigned char  idx_function_script_constructor;
		//int idxScriptClass;

		std::function<void * ()> 			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		string classPtrType; // type_id().name();
		unsigned char  idxClass;

		// vector<int> is set because C can have multiple herited classes (and we have to check whether an attempt C class to base of is not already set) where as script class only have one.
		vector<unsigned char > idxBaseClass;

		vector<int > metamethod_operator[MAX_METAMETHOD_OPERATORS]; // overrided metamethod

		ZETSCRIPT_MODULE_EXPORT CScriptClass();

		bool is_c_class();

		//------------- STATIC METHODS ---------------


	public:

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
		static tStackElement 									C_REF_InfoVariable_2_StackElement(tVariableSymbolInfo *ir_var, void *ptr_variable);

		static void 											register_C_BaseSymbolsInt(bool );
		// FUNCTIONS
		static void 											setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec);
		static vector<CScriptClass *> 		*					getVectorScriptClassNode();

		/**
		 * Class Manager
		 */
		static CScriptClass 						* 			registerClass(const string & class_name, const string & base_class_name="", PASTNode _ast=NULL);


		static CScriptClass 						* 			getScriptClass(unsigned char idx);
		ZETSCRIPT_MODULE_EXPORT static CScriptClass * 			getScriptClass(const string & name);
		ZETSCRIPT_MODULE_EXPORT static CScriptClass * 			getScriptClassIdx(const string & name);
		static CScriptClass 						* 			getScriptClassBy_C_ClassPtr(const string & class_type, bool throw_if_not_found=true);
		ZETSCRIPT_MODULE_EXPORT static bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		ZETSCRIPT_MODULE_EXPORT  static bool 					isClassRegistered(const string & v);

		static void registerPrimitiveTypes();

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		static CScriptVariable 		 				* 			instanceScriptVariableByClassName(const string & class_name);
		static CScriptVariable 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);
		static CScriptVariable 		 				* 			getScriptVariableByIdx(unsigned char idx_class, int  idx_var);

		static bool 											updateReferenceSymbols();

		//static tVariableSymbolInfo  				* 			registerVariableSymbol(const string & class_name,const string & name,short  idxAstNode);
		//static bool 				  							variableSymbolExist(CScriptClass *rc,const string & symbolname_ref);
		//static tVariableSymbolInfo 					*  			getRegisteredVariableSymbol(const string & class_name,const string & varname);




		static CScriptFunction 				*  			registerFunctionSymbol(const string & class_name, const string & name,short idxAstNode);
		virtual int									  			registerFunctionMember(const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		static int 												getIdxScriptFunctionObjectByClassFunctionName_Internal(CScriptClass *rc,const string & function_name);
		ZETSCRIPT_MODULE_EXPORT static int 						getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name);
		ZETSCRIPT_MODULE_EXPORT static CScriptFunction * 	getScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name);
		static bool 				  							existFunctionMember(CScriptClass *rc,const string & symbolname_ref, int n_params);


		static bool 											addArgumentFunctionSymbol(const string & class_name,const string & function_name,const string & arg_name);

		static const char 							* 			getNameRegisteredClassByIdx(unsigned char idx);

		// internal var types ...
		static CScriptClass *  									getRegisteredClassUndefined();
		static CScriptClass *  									getRegisteredClassStruct();
		static CScriptClass *  									getRegisteredClassString();
		static CScriptClass *  									getRegisteredClassVector();
		static CScriptClass *  									getRegisteredClassFunctor();
		static CScriptClass *  									getRegisteredClassNull();


		static intptr_t 										doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);



		static bool init();
		static void destroySingletons();

		/**
		 * REGISTER C Stuff
		 */

		/**
		 * Register C function
		 */
		template <typename F>
		static bool register_C_FunctionInt(const char * function_name,F function_ptr);

		/**
		 * Register C variable
		 */
		ZETSCRIPT_MODULE_EXPORT static bool register_C_VariableInt(const string & var_str,void * var_ptr, const string & var_type);
		ZETSCRIPT_MODULE_EXPORT static unsigned char getIdx_C_RegisteredClass(const string & str_classPtr, bool throw_if_not_found=true);


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		ZETSCRIPT_MODULE_EXPORT static bool register_C_SingletonClassInt(const string & class_name);

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		static bool register_C_ClassInt(const string & class_name);


		template<class _T, class _B>
		static bool class_C_baseofInt();


		/**
		 * Register C Member function Class
		 */
		template < typename _C, typename _R, class _T, typename..._A>
		static bool register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) );

		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		static bool register_C_StaticFunctionMemberInt(const char *function_name,_F function_type);


		/**
		 * Register C Member var
		 */
		template <typename _C, typename _R,typename _T>
		static bool register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer);



	private:

		static unsigned char									getIdxScriptClass_Internal(const string & class_name);
		static unsigned char									getIdxClassFromIts_C_TypeInternal(const string & c_type_str);
		ZETSCRIPT_MODULE_EXPORT static unsigned char			getIdxClassFromIts_C_Type(const string & s);


		static tPrimitiveType *getPrimitiveTypeFromStr(const string & str);
		static map<int,map<int,fntConversionType>> * mapTypeConversion;
#if 0
		 static bool searchVarFunctionSymbol(tScopeInfo * scope_info, tInstruction *iao, int current_idx_function,bool & symbol_not_found, unsigned int scope_type=0);

		 static void buildScopeVariablesBlock(CScriptFunction *root_class_irfs );
		 static void unloadRecursiveFunctions(CScriptFunction * info_function);

		 static bool updateFunctionSymbols(int idxSxriptFunctionObject, const string & parent_symbol, int n_function);// is_main_class, bool is_main_function);
#endif


	};

	#include "CScriptClass.inc"
}

