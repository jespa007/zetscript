/*
 *  This _file is distributed under the MIT License.
 *  See LICENSE _file for details.
 */
#pragma once

#include "util/@util.h"

#include "StackElement.h"
#include "ByteCode.h"
#include "ByteCodeHelper.h"
#include "Metamethod.h"
#include "MetamethodHelper.h"
#include "Instruction.h"
#include "Symbol.h"


#include "scope/zs_scope.h"
#include "module/zs_module.h"
#include <script/zs_script.h>
#include "eval/eval.h"
#include "vm/vm.h"


#define ZETSCRIPT_VERSION_MAJOR 2
#define ZETSCRIPT_VERSION_MINOR 0
#define ZETSCRIPT_VERSION_PATCH 0

#define ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(class_type) \
 class_type * class_type##_New(ZetScript *_zs){ return new class_type(_zs);} \
 void class_type##_Delete(ZetScript *_zs,class_type *ptr){ ZS_UNUSUED_PARAM(_zs);delete  (class_type *)(ptr);}


namespace zetscript{

	enum:uint16_t{
		 EVAL_OPTION_NO_EXECUTE				=0x1
		,EVAL_OPTION_PRINT_BYTE_CODE			=0x2
		,EVAL_OPTION_PRINT_ALL_BYTE_CODE		=0x4

	}EvalOption;

	struct VirtualMachine;
	class ScriptEval;
	class ZetScript{

	public:


		//===================================================================================================
		//
		// PRINT INFO

		static void printAllStructSizes();


		void printGeneratedCode(bool show_system_code=false);

		// PRINT  INFO
		//---------------------------------------------------------------------------------------------------------------------------------------

		ZetScript();

		inline VirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline FunctionFactory *getFunctionFactory() { return script_function_factory;}
		inline TypeFactory *getTypeFactory() { return type_factory;}

		StackElement	eval(const String & expresion, const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	eval(const String & expresion,unsigned short _eval_options, const char * _script_file_by_ref="", const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	evalFile(const String & _filename,uint16_t _eval_options=0, EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);


		inline MapString * getCompiledSymbolName(){
			return compiled_symbol_name;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
		//
		// STACKELEMENT
		//
		// it gives stackelement as string (the result should be deleted)
		void 					setStackElementUndefined();
		StackElement   			intToStackElement(zs_int);
		StackElement    		floatToStackElement(zs_float);
		StackElement    		boolToStackElement(bool);

		template<typename _C>
		_C stackElementTo(StackElement * _stk);

		template<typename _C>
		bool canStackElementCastTo(StackElement * _stack_element);

		template<typename _C>
		bool stackElementInstanceOf(StackElement * _stack_element);


		template<typename _C>
		_C stackElementTo(StackElement   _stk);

		String		stackElementToString(StackElement *_stk,const String & _format="");
		const char		*stackElementToString(char *_str_out,int _str_out_len, StackElement *_stk,const String & _format="");
		StackElement	unwrapStackElement(StackElement _stk);

		String		stackElementToStringTypeOf(StackElement *_stk);
		const char		*stackElementToStringTypeOf(char *_str_out, StackElement *_stk);

		void			stackElementAssign(StackElement *_stk_dst, const StackElement *_stk_src);
		StackElement 	toStackElement(zs_int ptr_var, TypeId _type_id);
		bool			stackElementTo(StackElement * _stack_element, TypeId _type_id, zs_int *_ptr_var, String  & _error);
		bool			canStackElementCastTo(StackElement * _stack_element,TypeId _type_id, bool _strict = false);

		template<typename _C>
		StackElement	toStackElement( _C _val);

		//
		// STACKELEMENT
		//
		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		//
		bool isFilenameAlreadyParsed(const String & filename);
		//
		ObjectObject * newObjectObject();
		StringObject * newStringObject(const String & _str="");
		ArrayObject * newArrayObject();

		template<typename _C>
		ClassObject * newClassObject(_C  *_instance=NULL);

		//-----------------------------------------------
		/**
		 * Register constants
		 */

		void registerConstant(const String & var_name, int value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const String & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const String & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const String & var_name, const String & v, const char *registered_file="", short registered_line=-1);
		void registerConstant(const String & var_name, const char * v, const char *registered_file="", short registered_line=-1);

		// register object
		StackElement * registerStkConstantStringObject(const String & _key,const String & _value);
		StackElement * getStkConstantStringObject(const String & _key);

		/**
		 * Register T Class. Return index registered class
		 */
		template<typename T>
		Type * registerType(
				const String & name
				, T  * (*_new_native_instance)(ZetScript *_zs)=NULL
				, void (*_delete_native_instance)(ZetScript *_zs,T *)=NULL
				, const char *_registered_file="",short _registered_line=-1
		){
			try{
				return type_factory->registerType<T>(name, _new_native_instance, _delete_native_instance, _registered_file,_registered_line);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\")': %s",__func__,Rtti::demangle(typeid(T).name()).toConstChar(),name.toConstChar(),_ex.what());
				return NULL;
			}
		}

		/**
		 * Register T function
		 */
		template <typename F>
		void registerFunction( const String & _name_script_function,F ptr_function, const char *_registered_file="",short _registered_line=-1){
			try{
				type_factory->registerFunction( _name_script_function,ptr_function, _registered_file,_registered_line);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR("Exception in '%s(\"%s\")': %s",__func__,_name_script_function.toConstChar(),_ex.what());
			}
		}

		template<class T, class B>
		void extends(){
			type_factory->extends<T,B>();
		}

		template<typename T,typename F>
		void registerConstructor(
				 F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				type_factory->registerConstructor<T>(function_type, _registered_file,_registered_line );
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(...)': %s"
					,__func__
					,Rtti::demangle(typeid(T).name()).toConstChar()
					,_ex.what()
				);
			}
		}

		template <typename T,typename F>
		void	registerMemberFunction(
				const String & _name_script_function
				,F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				type_factory->registerMemberFunction<T>(_name_script_function,function_type, _registered_file,_registered_line );
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,Rtti::demangle(typeid(T).name()).toConstChar()
					,_name_script_function.toConstChar()
					,_ex.what()
				);
			}
		}

		template <typename T,typename F>
		void	registerConstMemberProperty(
				const String & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				type_factory->registerConstMemberProperty<T>(
					_property_name
					,ptr_function
					, _registered_file
					,_registered_line
				);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,Rtti::demangle(typeid(T).name()).toConstChar()
					,_property_name.toConstChar()
					,_ex.what()
				);
			}
		}


		template <typename T,typename F>
		void	registerMemberPropertyMetamethod(
				const String & _property_name
				,const String & _metamethod_name
				,F _ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				type_factory->registerMemberPropertyMetamethod<T>(
					_property_name
					,_metamethod_name
					,_ptr_function
					,_registered_file
					,_registered_line
				);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,Rtti::demangle(typeid(T).name()).toConstChar()
					,_property_name.toConstChar()
					,_ex.what()
				);
			}
		}



		/**
		 * Register Static Function Member Class
		 */
		template <typename T,typename F>
		void registerStaticMemberFunction(const String & _name_script_function,F _fun, const char *_registered_file="",short _registered_line=-1){
			try{
				type_factory->registerStaticMemberFunction<T>(_name_script_function,_fun, _registered_file, _registered_line);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,Rtti::demangle(typeid(T).name()).toConstChar()
					,_name_script_function.toConstChar()
					,_ex.what()
				);
			}
		}

		//cpp binding
		void unrefLifetimeObject(Object *so);
		void makeObjectShared(Object *so);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line,void **ptr_fun ,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line,void **ptr_fun ,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 8)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 8)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 9)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 9)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 10)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindFunctionBuilder(const char *_file,int _line, void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 10)>::type;




		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, Object *calling_obj,Function *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, Object *calling_obj,Function *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getObject(const String &function_access,Object **calling_obj,Function **fun_obj );

		template <  typename F>
		std::function<F> bindFunction(const String & function_access, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindFunction(MemberFunctionObject *_sf, const char *_file="", int _line=-1);


		template <  typename F>
		std::function<F> bindFunction(Function *_sf, Object *_calling_object, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindFunction(Function *_sf, const char *_file="", int _line=-1);



		//-----------------------------------------------------------------------------------------------------------------------
		/**
		* clear: clear compile information.
		*/
		void clear();
		void saveState();

		void addUnresolvedSymbol(Function *_sf, zs_int _idx_instruction);
		void link();

		 ~ZetScript();

	private:

		typedef struct {
			String filename;
			//unsigned char *data;
		} ParsedFile;


		//--------
		// VARS
		MapString 	 								*script_filenames_by_ref;
		MapString 	 							   	*stk_constants;//,*stk_objects;
		Vector<ParsedFile *>					parsed_files;
		Vector<Function *>				functions_with_unresolved_symbols;

		//ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		ScopeFactory * scope_factory;
		FunctionFactory *script_function_factory;
		TypeFactory *type_factory;

		zs_float eval_float;
		zs_int eval_int;
		bool eval_bool;
		String eval_string;
		int idx_current_global_variable_checkpoint;
		int idx_current_global_function_checkpoint;
		int idx_current_types_checkpoint;

		MapString	*compiled_symbol_name;

		//===================================================================================================
		//
		// PRINT ASM INFO
		//----
		void clearMainFunction();
		StackElement evalInternal(const char * _code, unsigned short _options=0, const char  *_filename="",EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

		//void setClearGlobalVariablesCheckpoint();
		void resetParsedFiles();
		const char *getFilenameByRef(const char * _filename_by_ref);
		void clearGlobalVariables(int _idx_start_variable=ZS_UNDEFINED_IDX, int _idx_start_function=ZS_UNDEFINED_IDX);
		bool getFunctionWithUnresolvedSymbolExists(Function *_sf);

		template<typename R>
		R checkAndConvertStackElement(StackElement *_stk, TypeId _return_type_id);


	};

}

#include 	"zetscript.tcc"


