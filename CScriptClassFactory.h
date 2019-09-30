#pragma once

#include "CNativeFunctionFactory.h"




namespace zetscript{

	class CScriptVar;
	class CScriptClass;
	class CZetScript;
	class CScopeFactory;


	class CScriptClassFactory{

	public:



		CScriptClassFactory(CZetScript *_zs);


		 /**
		  * Class management region
		  */

			/// register script class
			CScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");

		 CScriptClass * 				getScriptClass(unsigned char idx);
		 CScriptClass * 				getScriptClass(const std::string & name);
		CScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
		const char 							* 				getScriptClassName(unsigned char idx);


		bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		unsigned char				getIdxClassFromIts_C_Type(const std::string & s);
		unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);

		 std::vector<CScriptClass *> * 	getVectorScriptClassNode();



		bool 						isClassRegistered(const std::string & v);



		intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);

		void clear();
		inline CScriptClass * getMainObject() { return main_object;}
		inline CScriptFunction * getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		CScriptVar 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
		CScriptVar 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);


		void register_C_BaseSymbols(bool _register);


		/**
		 * Register C variable
		 */
		 VariableSymbolInfo *  register_C_Variable(const std::string & var_name,void * var_ptr, const std::string & var_type, const char *registered_file,int registered_line);


		/**
		 * Register C function
		 */
		template <typename F>
		bool register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1);



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


		template<typename T>
		bool register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1);


		template<class T, class B>
		bool class_C_BaseOf();

		/**
		 * Register C Member function Class
		 */
		template < typename C, typename R, class T, typename..._A>
		bool register_C_MemberFunction(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 );

		/**
		 * Register C Member function Class
		 */
		template <typename T, typename F>
		bool register_C_StaticMemberFunction(const char *function_name,F function_type, const char *registered_file="",int registered_line=-1);


		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool register_C_MemberVariable(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1);


		~CScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			BUILT_IN_TYPE  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			std::vector<PrimitiveType*>		params;
		}tRegisterFunction;

		std::vector<CScriptClass *> 			 		vec_script_class_node;
		CZetScript *zs;
		CScopeFactory *scope_factory;
		CScriptFunctionFactory *script_function_factory;
		CNativeFunctionFactory *native_function_factory;
		CScriptClass * main_object;
		CScriptFunction * main_function;

		/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
			bool register_c_base_symbols;



			PrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
			std::map<unsigned char, std::map<unsigned char, fntConversionType>> *  getMapTypeConversion();








			void 												registerPrimitiveTypes();





			unsigned char							getIdxScriptClass_Internal(const std::string & class_name);
			unsigned char							getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str);

			void setup();

			void internal_print_error(const char *s);



			std::map<unsigned char,std::map<unsigned char,fntConversionType>> 	mapTypeConversion;
	};

}
#include "helper/register_function.h"
#include "CScriptClassFactory.inc"
