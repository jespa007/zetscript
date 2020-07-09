#pragma once

#include "NativeFunctionFactory.h"




namespace zetscript{

	class ScriptVar;
	class ScriptClass;
	class ZetScript;
	class ScopeFactory;


	class ScriptClassFactory{

	public:



		ScriptClassFactory(ZetScript *_zs);


		 /**
		  * Class management region
		  */

			/// register script class
			ScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");

		 ScriptClass * 				getScriptClass(unsigned char idx);
		 ScriptClass * 				getScriptClass(const std::string & name);
		ScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
		const char 							* 				getScriptClassName(unsigned char idx);


		bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		unsigned char				GetIdxClassFromIts_C_Type(const std::string & s);
		unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);

		 std::vector<ScriptClass *> * 	getVectorScriptClassNode();



		bool 						isClassRegistered(const std::string & v);



		intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);

		void clear();
		inline ScriptClass * getMainObject() { return main_object;}
		inline ScriptFunction * getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptVar 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
		ScriptVar 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);


		void register_C_BaseSymbols(bool _register);


		/**
		 * Register C variable
		 */
		 SymbolInfo *  Register_C_Variable(const std::string & var_name,void * var_ptr, const std::string & var_type, const char *registered_file,int registered_line);


		/**
		 * Register C function
		 */
		template <typename F>
		bool Register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1);



		/**
		 * Register C Class. Return index registered class
		 */
		template<typename T>
		 bool register_C_SingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1);

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename T>
		bool Register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1);


		template<typename T>
		bool Register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1);


		template<class T, class B>
		bool class_C_BaseOf();

		/**
		 * Register C Member function Class
		 */
		template < typename C, typename R, class T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 );

		/**
		 * Register C Member function static Class
		 */
		template <typename R, class T, typename..._A>
		bool Register_C_FunctionMemberStatic(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1);

		/**
		 * Register C function as function member
		 */
		template <typename F>
		bool Register_C_FunctionAsFunctionMember(const char *function_name,F function_type, const char *registered_file,int registered_line);

		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool Register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1);


		~ScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			IdxBuiltInType  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			std::vector<PrimitiveType*>		params;
		}tRegisterFunction;

		std::vector<ScriptClass *> 			 		script_classes;
		ZetScript *zs;
		ScopeFactory *scope_factory;
		ScriptFunctionFactory *script_function_factory;
		NativeFunctionFactory *native_function_factory;
		ScriptClass * main_object;
		ScriptFunction * main_function;

		/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
			bool register_c_base_symbols;



			PrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
			std::map<unsigned char, std::map<unsigned char, ConversionType>> *  GetConversionTypes();


			void 												RegisterPrimitiveTypes();

			unsigned char							GetIdxScriptClassInternal(const std::string & class_name);
			unsigned char							GetIdxScriptInternalFrom_C_Type(const std::string & c_type_str);

			void Setup();

			void InternalPrintError(const char *s);



			std::map<unsigned char,std::map<unsigned char,ConversionType>> 	conversion_types;
	};

}
#include "helper/register_function.h"
#include "ScriptClassFactory.inc"
