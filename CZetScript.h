#pragma once

#define REGISTER_C_FUNCTION(zs,text,s) 			(zs)->CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);(zs)->register_C_FunctionInt(text,s)
#define REGISTER_C_VARIABLE(zs,text,s) 			(zs)->CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);(zs)->register_C_VariableInt(text,&s,typeid(decltype(&s)).name())

#define REGISTER_C_CLASS 						(zs)->etFilenameLine(__FILE__, __LINE__);(zs)->register_C_ClassInt
#define REGISTER_C_SINGLETON_CLASS				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_SingletonClassInt
#define REGISTER_C_STATIC_FUNCTION_MEMBER		(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_StaticFunctionMemberInt

#define REGISTER_C_VARIABLE_MEMBER				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_VariableMemberInt
#define REGISTER_C_FUNCTION_MEMBER				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_FunctionMemberInt //<o>(s,&f)

namespace zs{

	class CZetScript{
	public:
		 void	set_callback_on_error(tPrintFunctionCallback _fun);

		 int eval_int_value(const std::string & str_to_eval);
		 bool eval_bool_value(const std::string & str_to_eval);
		 float eval_float_value(const std::string & str_to_eval);
		 std::string eval_string_value(const std::string & str_to_eval);

		/**
		 * Clear: Clear compile information.
		 */
		 void clear();
		 void execute();

		 CVirtualMachine *get_vm();

		 bool eval_string(const std::string & expresion, bool execute=true, const char *filename_ref=NULL,bool show_bytecode=false);
		 bool eval_file(const std::string & filename,bool execute=true,bool show_bytecode=false);

			/**
			 * REGISTER C Stuff
			 */

			/**
			 * Register C function
			 */
			template <typename F>
			bool register_C_FunctionInt( const char * function_name,F function_ptr);

			/**
			 * Register C variable
			 */
			 tVariableSymbolInfo * register_C_VariableInt(const std::string & var_str,void * var_ptr, const std::string & var_type);
			 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);


			/**
			 * Register C Class. Return index registered class
			 */
			template<class _T>
			 bool register_C_SingletonClassInt(const std::string & class_name);

			/**
			 * Register C Class. Return index registered class
			 */
			template<class _T>
			bool register_C_ClassInt(const std::string & class_name);


			template<class _T, class _B>
			bool class_C_BaseOfInt();

			/**
			 * Register C Member function Class
			 */
			template < typename _C, typename _R, class _T, typename..._A>
			bool register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) );

			/**
			 * Register C Member function Class
			 */
			template <typename _T, typename _F>
			bool register_C_StaticFunctionMemberInt(const char *function_name,_F function_type);


			/**
			 * Register C Member var
			 */
			template <typename _C, typename _R,typename _T>
			bool register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer);

		 void destroy();

		// eval
		// eval_file
	private:
		CEval _eval;
		CVirtualMachine _virtual_machine;
		CScriptClassFactory _script_class_factory;

	};

}

#include "CZetScript.inc"
#include "cpp_binding.h"
