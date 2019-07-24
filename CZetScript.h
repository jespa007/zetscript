#pragma once

#include "CNativeFunctionFactory.h"
#include "helper/register_function.h"

#define 												MAX_REGISTER_CLASSES 100

#define ZS_INVALID_CLASS								((unsigned char)ZS_UNDEFINED_IDX)


#define CLASS_C_BASEOF(zs)								(zs)->class_C_BaseOf();

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


#define REGISTER_C_FUNCTION(zs,text,s) 					(zs)->register_C_Function(text,s,__FILE__, __LINE__)
#define REGISTER_C_VARIABLE(zs,text,s) 					(zs)->register_C_Variable(text,&s,typeid(decltype(&s)).name(),__FILE__, __LINE__)

#define REGISTER_C_CLASS(zs,class_type,s) 				(zs)->register_C_Class<class_type>(s,__FILE__, __LINE__)
#define REGISTER_C_SINGLETON_CLASS(zs,class_type,s)		(zs)->register_C_SingletonClass<class_type>(s,__FILE__, __LINE__)

#define REGISTER_C_VARIABLE_MEMBER(zs,s,v)				(zs)->register_C_VariableMember(s,v)
#define REGISTER_C_STATIC_FUNCTION_MEMBER(zs,s,f)		(zs)->register_C_StaticFunctionMember(s,f,__FILE__, __LINE__)
#define REGISTER_C_FUNCTION_MEMBER(zs,s,f)				(zs)->register_C_FunctionMember(s,f,__FILE__, __LINE__)

#define REGISTER_C_CONSTANT(zs,s,v)						(zs)->register_C_FunctionMember(s,f,__FILE__, __LINE__)



namespace zetscript{
	class CVirtualMachine;
	class CEval;
	class CZetScript{



	public:

		std::string  VOID_TYPE_STR;				// 	typeid(void).name()
		std::string  INT_PTR_TYPE_STR;			//	typeid(int *).name()
		std::string  FLOAT_PTR_TYPE_STR;		//	typeid(float *).name()
		std::string  STRING_PTR_TYPE_STR;		//	typeid(std::string *).name()
		std::string  CONST_CHAR_PTR_TYPE_STR;	//	typeid(std::string *).name()
		std::string  BOOL_PTR_TYPE_STR;			//	typeid(bool *).name()
		std::string  INT_TYPE_STR;				//	typeid(int).name()
		std::string  UNSIGNED_INT_TYPE_STR;		//	typeid(unsigned int).name()
		std::string  INTPTR_T_TYPE_STR;			//	typeid(intptr_t).name()

		std::string  FLOAT_TYPE_STR;			//	typeid(int).name()
		std::string  BOOL_TYPE_STR;				//	typeid(bool).name()
		std::string  STACK_ELEMENT_STR;			//	typeid(bool).name()

		CZetScript();

		inline CVirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline CNativeFunctionFactory * getNativeFunctionFactory() { return native_function_factory;}
		inline CScopeFactory * getScopeFactory() { return scope_factory;}
		inline CScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline CScriptFunction * getMainFunction() { return main_function;}
		inline CScriptFunction * getMainObject() { return main_object;}

		 void	set_callback_on_error(PrintFunctionCallback _fun);

		 int * evalIntValue(const std::string & str_to_eval);
		 bool * evalBoolValue(const std::string & str_to_eval);
		 float * evalFloatValue(const std::string & str_to_eval);
		 std::string * evalStringValue(const std::string & str_to_eval);

		 bool evalString(const char * expresion, bool execute=true,bool show_bytecode=false, const char * filename="");
		 bool evalFile(const char * filename,bool execute=true,bool show_bytecode=false);


				//===================================================================================================
				//
				// PRINT ASM INFO
				void printGeneratedCode();

				// PRINT ASM INFO
				//---------------------------------------------------------------------------------------------------------------------------------------
		//		 static std::string 				getSymbolNameFromSymbolRef(const std::string & ref_symbol);
		//		 static std::string 				makeSymbolRef(const std::string & symbol_var, short idxScope, char n_params=NO_PARAMS_IS_VARIABLE);
				 ConstantValueInfo 	* 		addConstant(const std::string & const_name, int value);

				 // CONSTANT TOOLS

				 ConstantValueInfo * getConstant(const std::string & const_name);
				 ConstantValueInfo * addConstant(const std::string & const_name, void *obj, unsigned short properties);


			//===================================================================================================



			//---------------------------------------------------------------------------------------------------------------------------------------
			// FILE MANAGEMENT
			 bool isFilenameAlreadyParsed(const std::string & filename);
			 const char * getParsedFilenameFromIdx(unsigned idx);

			//-----------------------------------------------

		/**
		 * Clear: Clear compile information.
		 */
		 void clear();
		 void execute();




		 /**
		  * Class management region
		  */

			/// register script class
			CScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");

		 CScriptClass * 				getScriptClass(unsigned char idx);
		 CScriptClass * 				getScriptClass(const std::string & name);
		CScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
		const char 							* 				getScriptClassName(unsigned char idx);
		const char * 				getMetamethod(METAMETHOD_OPERATOR op);

		bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		StackElement 					C_REF_InfoVariable_2_StackElement(VariableSymbolInfo *ir_var, void *ptr_variable);

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		CScriptVariable 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
		CScriptVariable 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);


		/**
		 * Register C region
		 */
		template<class T, class B>
		bool class_C_Baseof();

		/**
		 * Register C function
		 */
		template <typename F>
		bool register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C variable
		 */
		 VariableSymbolInfo * register_C_Variable(const std::string & var_str,void * var_ptr, const std::string & var_type, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename T>
		 bool register_C_SingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename T>
		bool register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1);


		template<class T, class B>
		bool class_C_BaseOf();

		/**
		 * Register C Member function Class
		 */
		template < typename C, typename R, class T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 );

		/**
		 * Register C Member function Class
		 */
		template <typename T, typename F>
		bool register_C_StaticFunctionMember(const char *function_name,F function_type, const char *registered_file="",int registered_line=-1);


		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1);

		//cpp binding
		// Helpers...
		inline StackElement var_2_stk(intptr_t var_trans, int idx_type);
		inline bool stk_2_var(StackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;



		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;



		//CScriptFunction *getScriptObjectFromScriptFunctionAccessName(const std::string &function_access_expression)
		bool getScriptObject(const std::string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj );


		template <  typename F>
		std::function<F> * bind_function(const std::string & function_access);

		template<typename T>
		std::vector<T> vscript2vector(CVectorScriptVariable *v_in);

		template<typename T>
		CVectorScriptVariable * vector_2_vscript(const std::vector<T> & v);



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


		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedSourceInfo;


		//===================================================================================================
			//
			// PRINT ASM INFO
			std::string getStrMovVar(OpCodeInstruction * iao);
			std::string getStrTypeLoadValue(CScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction);
			 void printGeneratedCode(CScriptFunction *sfo);

		//----



		tPrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
		std::map<unsigned char, std::map<unsigned char, fntConversionType>> *  getMapTypeConversion();
		// singleton
		 std::map<std::string,ConstantValueInfo *> 	 m_contantPool;
		 std::vector<ParsedSourceInfo> 			 m_parsedSource;

		template<typename T>
		bool register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1);



		 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);
		 unsigned char				getIdxClassFromIts_C_Type(const std::string & s);
		 void 						setPrintOutCallback(void (*)(const char *));

		 std::vector<CScriptClass *> * 	getVectorScriptClassNode();



		bool 						isClassRegistered(const std::string & v);

		void 												registerPrimitiveTypes();
		void 												register_C_BaseSymbols(bool );




		intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);



		static 									void  print(const char *s);
		static 									void (* print_out_callback)(const char *);

		unsigned char							getIdxScriptClass_Internal(const std::string & class_name);
		unsigned char							getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str);

		void setup();

		void internal_print_error(const char *s);

		// VARS

		std::map<std::string,ConstantValueInfo *> 	 constant_pool;

		CEval * eval;
		CVirtualMachine * virtual_machine;
		CNativeFunctionFactory * native_function_factory;
		CScopeFactory * scope_factory;
		CScriptFunctionFactory *script_function_factory;
		CScriptFunction * main_function;
		CScriptFunction * main_object;
		CZetScript 		*zs;

		float eval_float;
		int eval_int;
		bool eval_bool;
		std::string eval_string;

		std::vector<CScriptClass *> 			 		vec_script_class_node;

		/*
		 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
		 */
		bool register_c_base_symbols;

		std::map<unsigned char,std::map<unsigned char,fntConversionType>> 	mapTypeConversion;

	};

}

#include "CVirtualMachine.h"
#include "CZetScript.inc"
#include "helper/cpp_binding.h"
