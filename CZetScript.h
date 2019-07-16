#pragma once

#include "CNativeFunctionFactory.h"
#include "helper/register_function.h"

#define 												MAX_REGISTER_CLASSES 100

#define ZS_INVALID_CLASS								((unsigned char)ZS_UNDEFINED_IDX)


#define CLASS_C_BASEOF(zs)								(zs)->class_C_BaseOf();


#define NEW_CLASS_VAR_BY_IDX(zs,idx) 					((zs)->instanceScriptVariableByIdx(idx))

#define GET_SCRIPT_CLASS(zs,idx) 						((zs)->getScriptClass(idx))
#define GET_SCRIPT_CLASS_NAME(zs,idx) 					((zs)->getScriptClassName(idx))


#define SCRIPT_CLASS_MAIN(zs)							((zs)->getScriptClass(IDX_CLASS_MAIN))    // 0 is the main class
#define SCRIPT_CLASS_STRING(zs)							((zs)->getScriptClass(IDX_CLASS_STRING))
#define SCRIPT_CLASS_STRUCT(zs)							((zs)->getScriptClass(IDX_CLASS_STRUCT))
#define SCRIPT_CLASS_VECTOR(zs)							((zs)->getScriptClass(IDX_CLASS_VECTOR))
#define SCRIPT_CLASS_FUNCTOR(zs)						((zs)->getScriptClass(IDX_CLASS_FUNCTOR))

#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(zs,s)		((zs)->getScriptClassBy_C_ClassPtr(s))    // 0 is the main class

#define GET_IDX_2_CLASS_C_STR(zs,idx) 					((zs)->getScriptClass(idx)->classPtrType)

#define REGISTER_C_BASE_SYMBOLS(zs,o)		   			((zs)->register_C_BaseSymbols(o))

#define GET_SCOPE(zs,idx)					 			((zs)->getScope(idx))
#define MAIN_SCOPE(zs)									((zs)->getScope(IDX_GLOBAL_SCOPE)
#define NEW_SCOPE(zs)									(zs)->newScope)

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(zs,idx)						((zs)->getScope(idx)->getIdxBaseScope()==IDX_GLOBAL_SCOPE)


//#define VOID_TYPE_STR									zetscript::CScriptClassFactory::getInstance()->m_VOID_TYPE_STR			// 	typeid(void).name()
//#define INT_PTR_TYPE_STR								zetscript::CScriptClassFactory::getInstance()->m_INT_PTR_TYPE_STR			//	typeid(int *).name()
//#define FLOAT_PTR_TYPE_STR								zetscript::CScriptClassFactory::getInstance()->m_FLOAT_PTR_TYPE_STR		//	typeid(float *).name()
//#define STRING_PTR_TYPE_STR								zetscript::CScriptClassFactory::getInstance()->m_STRING_PTR_TYPE_STR		//	typeid(std::string *).name()
//#define CONST_CHAR_PTR_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_CONST_CHAR_PTR_TYPE_STR	//	typeid(std::string *).name()
//#define BOOL_PTR_TYPE_STR								zetscript::CScriptClassFactory::getInstance()->m_BOOL_PTR_TYPE_STR		//	typeid(bool *).name()
//#define INT_TYPE_STR									zetscript::CScriptClassFactory::getInstance()->m_INT_TYPE_STR				//	typeid(int).name()
//#define UNSIGNED_INT_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_UNSIGNED_INT_TYPE_STR	//	typeid(unsigned int).name()
//#define INTPTR_T_TYPE_STR								zetscript::CScriptClassFactory::getInstance()->m_INTPTR_T_TYPE_STR		//	typeid(intptr_t).name()

//#define FLOAT_TYPE_STR									zetscript::CScriptClassFactory::getInstance()->m_FLOAT_TYPE_STR			//	typeid(int).name()
//#define BOOL_TYPE_STR									zetscript::CScriptClassFactory::getInstance()->m_BOOL_TYPE_STR			//	typeid(bool).name()
//#define STACK_ELEMENT_STR								zetscript::CScriptClassFactory::getInstance()->m_STACK_ELEMENT_STR			//	typeid(bool).name()

#define DO_CAST(zs)										(zs)->doCast
#define GET_IDX_CLASS_FROM_ITS_C_TYPE(zs)				(zs)->getIdxClassFromIts_C_Type
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX(zs)				(zs)->instanceScriptVariableByIdx
#define GET_METAMETHOD(zs)								(zs)->getMetamethod
#define IS_IDX_CLASS_INSTANCEOF(zs)						(zs)->isIdxClassInstanceOf

#define REGISTER_C_FUNCTION(zs,text,s) 					(zs)->register_C_Function(text,s,__FILE__, __LINE__)
#define REGISTER_C_VARIABLE(zs,text,s) 					(zs)->register_C_Variable(text,&s,typeid(decltype(&s)).name(),__FILE__, __LINE__)

#define REGISTER_C_CLASS(zs,s) 							(zs)->register_C_Class(s,__FILE__, __LINE__)
#define REGISTER_C_SINGLETON_CLASS(zs,s)				(zs)->register_C_SingletonClass(s,__FILE__, __LINE__)
#define REGISTER_C_STATIC_FUNCTION_MEMBER(zs,s,f)		(zs)->register_C_StaticFunctionMember(s,f,__FILE__, __LINE__)

#define REGISTER_C_VARIABLE_MEMBER(zs,s,v)				(zs)->register_C_VariableMember(s,v)
#define REGISTER_C_FUNCTION_MEMBER(zs,s,f)				(zs)->register_C_FunctionMember(s,f,__FILE__, __LINE__)

#define REGISTER_C_CONSTANT(zs,s,v)						(zs)->register_C_FunctionMember(s,f,__FILE__, __LINE__)

#define NEW_SCRIPT_FUNCTION(zs)							(zs)->newScriptFunction
#define GET_SCRIPT_FUNCTION(zs,idx) 					(zs)->getScriptFunction(idx)
#define MAIN_FUNCTION(zs)								GET_SCRIPT_FUNCTION(zs,IDX_MAIN_FUNCTION)


namespace zetscript{

	class CZetScript{
	public:
		 void	set_callback_on_error(tPrintFunctionCallback _fun);

		 int * evalIntValue(const std::string & str_to_eval);
		 bool * evalBoolValue(const std::string & str_to_eval);
		 float * evalFloatValue(const std::string & str_to_eval);
		 std::string * evalStringValue(const std::string & str_to_eval);

		/**
		 * Clear: Clear compile information.
		 */
		 void clear();
		 void execute();

		 CVirtualMachine *get_vm();

		 bool evalString(const std::string & expresion, bool execute=true, const char *filename_ref=NULL,bool show_bytecode=false);
		 bool evalFile(const std::string & filename,bool execute=true,bool show_bytecode=false);

		/**
		 * REGISTER C Stuff
		 */

		/**
		 * Register C function
		 */
		template <typename F>
		bool register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C variable
		 */
		 tVariableSymbolInfo * register_C_Variable(const std::string & var_str,void * var_ptr, const std::string & var_type, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		 bool register_C_SingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		bool register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1);


		template<class _T, class _B>
		bool class_C_BaseOf();

		/**
		 * Register C Member function Class
		 */
		template < typename _C, typename _R, class _T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,_R (_T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 );

		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		bool register_C_StaticFunctionMember(const char *function_name,_F function_type, const char *registered_file="",int registered_line=-1);


		/**
		 * Register C Member var
		 */
		template <typename _C, typename _R,typename _T>
		bool register_C_VariableMember(const char *var_name, _R _T::*var_pointer, const char *registered_file="",int registered_line=-1);

		//cpp binding
		// Helpers...
		inline tStackElement var_2_stk(intptr_t var_trans, int idx_type);

		inline bool stk_2_var(tStackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename _R,typename _T>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<_R,void>::value>::type;


		template <typename _R,typename _T>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<_R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename _R,typename _T,  typename... ArgTypes>
		auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename _R,typename _T,  typename... ArgTypes>
		auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename _R,typename _T, typename... ArgTypes>
		auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;



		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename _F, std::size_t... Is>
		 auto bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(_F::arity > 0)>::type;


		 template <typename _F, std::size_t... Is>
		 auto bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(_F::arity == 0)>::type;



		//CScriptFunction *getScriptObjectFromScriptFunctionAccessName(const std::string &function_access_expression)
		bool get_script_object(const std::string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj );


		template <  typename _F>
		std::function<_F> * bind_function(const std::string & function_access);

		template<typename _T>
		std::vector<_T> vscript2vector(CVectorScriptVariable *v_in);

		template<typename _T>
		CVectorScriptVariable * vector_2_vscript(const std::vector<_T> & v);



		 ~CZetScript();

	private:

		typedef struct{
			const char *   type_str;
			BUILT_IN_TYPE  id;
		}tPrimitiveType;

		typedef struct{
			tPrimitiveType 				*return_type;
			std::vector<tPrimitiveType*>		params;
		}tRegisterFunction;


		static void 				setFilenameLine(const char *file, short line);
		static tStackElement 		C_REF_InfoVariable_2_StackElement(tVariableSymbolInfo *ir_var, void *ptr_variable);

		//----

		std::string  m_VOID_TYPE_STR;			// 	typeid(void).name()
		std::string  m_INT_PTR_TYPE_STR;			//	typeid(int *).name()
		std::string  m_FLOAT_PTR_TYPE_STR;		//	typeid(float *).name()
		std::string  m_STRING_PTR_TYPE_STR;		//	typeid(std::string *).name()
		std::string  m_CONST_CHAR_PTR_TYPE_STR;	//	typeid(std::string *).name()
		std::string  m_BOOL_PTR_TYPE_STR;		//	typeid(bool *).name()
		std::string  m_INT_TYPE_STR;				//	typeid(int).name()
		std::string  m_UNSIGNED_INT_TYPE_STR;	//	typeid(unsigned int).name()
		std::string  m_INTPTR_T_TYPE_STR;		//	typeid(intptr_t).name()

		std::string  m_FLOAT_TYPE_STR;			//	typeid(int).name()
		std::string  m_BOOL_TYPE_STR;			//	typeid(bool).name()
		std::string  m_STACK_ELEMENT_STR;		//	typeid(bool).name()

		tPrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
		std::map<unsigned char, std::map<unsigned char, fntConversionType>> *  getMapTypeConversion();



		 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);
		 unsigned char				getIdxClassFromIts_C_Type(const std::string & s);
		 void 						setPrintOutCallback(void (*)(const char *));
		 const char * 				getMetamethod(METAMETHOD_OPERATOR op);
		 std::vector<CScriptClass *> * 	getVectorScriptClassNode();

		/// register script class
		CScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");


		 CScriptClass * 				getScriptClass(unsigned char idx);
		 CScriptClass * 				getScriptClass(const std::string & name);
		CScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
		const char 							* 				getScriptClassName(unsigned char idx);


		 bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		  bool 						isClassRegistered(const std::string & v);

		void 												registerPrimitiveTypes();
		void 												register_C_BaseSymbols(bool );

		void clear();

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		CScriptVariable 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
		CScriptVariable 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);

		intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);



		static 									void  print(const char *s);
		static 									void (* print_out_callback)(const char *);

		unsigned char							getIdxScriptClass_Internal(const std::string & class_name);
		unsigned char							getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str);

		void setup();


		// VARS

		std::map<std::string,tInfoConstantValue *> 	 constant_pool;

		CEval * _eval;
		CVirtualMachine * _virtual_machine;
		CNativeFunctionFactory * _native_function_factory;
		CScopeFactory * _scope_factory;
		CScriptFunctionFactory *_script_function_factory;
		CScriptFunction * _main_function;
		CScriptFunction * _main_object;

		float _eval_float;
		int _eval_int;
		bool _eval_bool;
		std::string _eval_string;

		std::vector<CScriptClass *> 			 		vec_script_class_node;

		/*
		 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
		 */
		bool register_c_base_symbols;

		std::map<unsigned char,std::map<unsigned char,fntConversionType>> 	mapTypeConversion;



	};

}

#include "CZetScript.inc"
#include "helper/cpp_binding.h"
