#pragma once

#define REGISTER_C_FUNCTION(zs,text,s) 					(zs)->CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);(zs)->register_C_Function(text,s)
#define REGISTER_C_VARIABLE(zs,text,s) 					(zs)->CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);(zs)->register_C_Variable(text,&s,typeid(decltype(&s)).name())

#define REGISTER_C_CLASS(zs,s) 							(zs)->etFilenameLine(__FILE__, __LINE__);(zs)->register_C_Class(s)
#define REGISTER_C_SINGLETON_CLASS(zs,s)				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_SingletonClass(s)
#define REGISTER_C_STATIC_FUNCTION_MEMBER(zs,s,f)		(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_StaticFunctionMember(s,f)

#define REGISTER_C_VARIABLE_MEMBER(zs,s,v)				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_VariableMember(s,v)
#define REGISTER_C_FUNCTION_MEMBER(zs,s,f)				(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_FunctionMember(s,f)

#define REGISTER_C_CONSTANT(zs,s,v)						(zs)->setFilenameLine(__FILE__, __LINE__);(zs)->register_C_FunctionMember(s,f)


namespace zetscript{

	class CZetScript{
	public:
		 void	set_callback_on_error(tPrintFunctionCallback _fun);

		 int evalIntValue(const std::string & str_to_eval);
		 bool evalBoolValue(const std::string & str_to_eval);
		 float evalFloatValue(const std::string & str_to_eval);
		 std::string evalStringValue(const std::string & str_to_eval);

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
		bool register_C_Function( const char * function_name,F function_ptr);

		/**
		 * Register C variable
		 */
		 tVariableSymbolInfo * register_C_Variable(const std::string & var_str,void * var_ptr, const std::string & var_type);
		 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		 bool register_C_SingletonClass(const std::string & class_name);

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		bool register_C_Class(const std::string & class_name);


		template<class _T, class _B>
		bool class_C_BaseOfInt();

		/**
		 * Register C Member function Class
		 */
		template < typename _C, typename _R, class _T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,_R (_T:: *function_type)(_A...) );

		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		bool register_C_StaticFunctionMember(const char *function_name,_F function_type);


		/**
		 * Register C Member var
		 */
		template <typename _C, typename _R,typename _T>
		bool register_C_VariableMember(const char *var_name, _R _T::*var_pointer);

		 ~CZetScript();

	private:

		 std::map<std::string,tInfoConstantValue *> 	 *constant_pool;

		 CEval _eval;
		 CVirtualMachine _virtual_machine;
		 CScriptClassFactory _script_class_factory;
	};

}

#include "CZetScript.inc"
#include "cpp_binding.h"
