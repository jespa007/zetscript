/*
 *  This _file is distributed under the MIT License.
 *  See LICENSE _file for details.
 */
#pragma once

#define ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(class_type) \
 class_type * class_type##_New(ScriptEngine *_script_engine){ return new class_type(_script_engine);} \
 void class_type##_Delete(ScriptEngine *_script_engine,class_type *ptr){ ZS_UNUSUED_PARAM(_script_engine);delete  (class_type *)(ptr);}


namespace zetscript{

	struct VirtualMachine;
	class ScriptEval;
	class ScriptEngine{

	public:

		//===================================================================================================
		//
		// PRINT INFO

		static void printAllStructSizes();


		void printGeneratedCode(bool show_system_code=false);

		// PRINT  INFO
		//---------------------------------------------------------------------------------------------------------------------------------------

		ScriptEngine(int _vm_stack_size=ZS_VM_DEFAULT_STACK_SIZE);

		inline VirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline ScriptScopesFactory * getScriptScopesFactory() { return scope_factory;}
		inline ScriptFunctionsFactory *getScriptFunctionsFactory() { return script_function_factory;}
		inline ScriptTypesFactory *getScriptTypesFactory() { return script_types_factory;}




		StackElement	compileFileAndRun(const String & _filename, const char *__invoke_file__="", int __invoke_line__=-1);
		void			compileFile(const String & _filename, CompilerData *_compiler_data=NULL);

		void 			compile(const String & expresion);
		StackElement	compileAndRun(const String & expresion, const char *__invoke_file__="", int __invoke_line__=-1);

		StackElement 	run(const char *__invoke_file__="", int __invoke_line__=-1);

		inline MapString * getCompiledSymbolName(){
			return compiled_symbol_name;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
		//
		// STACKELEMENT
		//
		// it gives stackelement as string (the result should be deleted)
		void 			setStackElementUndefined();
		StackElement   	intToStackElement(zs_int);
		StackElement    floatToStackElement(zs_float);
		StackElement    boolToStackElement(bool);

		bool			stackElementTo(
			StackElement * _stack_element
			, ScriptTypeId _script_type_id
			, zs_int *_ptr_var
			, String  & _error
		);

		template<typename _C>
		_C 				stackElementTo(StackElement * _stk);

		template<typename _C>
		_C 				stackElementTo(StackElement   _stk);

		String			stackElementValueToString(StackElement *_stk,const String & _format="");
		const char	*	stackElementValueToString(char *_str_out,int _str_out_len, StackElement *_stk,const String & _format="");
		String			stackElementTypeToString(StackElement *_stk);
		const char	*	stackElementTypeToString(char *_str_out, StackElement *_stk);


		template<typename _C>
		bool 			canStackElementCastTo(StackElement * _stack_element);
		bool			canStackElementCastTo(StackElement * _stack_element,ScriptTypeId _script_type_id, bool _strict = false);

		template<typename _C>
		bool 			isStackElementInstanceOf(StackElement * _stack_element);



		StackElement	unwrapStackElement(StackElement _stk);


		void			stackElementAssign(StackElement *_stk_dst, const StackElement *_stk_src);
		StackElement 	toStackElement(zs_int ptr_var, ScriptTypeId _script_type_id);

		

		template<typename _C>
		StackElement	toStackElement( _C _val);


		void			pushStackElement(StackElement _stk);	

		//
		// STACKELEMENT
		//
		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		//
		bool isFilenameAlreadyParsed(const String & filename);
		//
		ObjectScriptObject * newObjectScriptObject();
		StringScriptObject * newStringScriptObject(const String & _str="");
		ArrayScriptObject * newArrayScriptObject();

		template<typename _C>
		ClassScriptObject * newClassScriptObject(_C  *_instance=NULL);

		//-----------------------------------------------
		/**
		 * Register constants
		 */

		void registerConstantInteger(const String & var_name, zs_int value, const char *registered_file="", short registered_line=-1);
		void registerConstantFloat(const String & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void registerConstantBoolean(const String & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void registerConstantString(const String & var_name, const String & v, const char *registered_file="", short registered_line=-1);

		// register object
		StackElement * registerStkConstantStringScriptObject(const String & _key,const String & _value);
		StackElement * getStkConstantStringScriptObject(const String & _key);

		/**
		 * Register T Class. Return index registered class
		 */
		template<typename T>
		ScriptType * registerType(
				const String & name
				, T  * (*_new_native_instance)(ScriptEngine *_script_engine)=NULL
				, void (*_delete_native_instance)(ScriptEngine *_script_engine,T *)=NULL
				, const char *_registered_file="",short _registered_line=-1
		){
			try{
				return script_types_factory->registerType<T>(name, _new_native_instance, _delete_native_instance, _registered_file,_registered_line);
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
				script_types_factory->registerFunction( _name_script_function,ptr_function, _registered_file,_registered_line);
			}catch(Exception & _ex){
				ZS_THROW_RUNTIME_ERROR("Exception in '%s(\"%s\")': %s",__func__,_name_script_function.toConstChar(),_ex.what());
			}
		}

		template<class T, class B>
		void extends(){
			script_types_factory->extends<T,B>();
		}

		template<typename T,typename F>
		void registerConstructor(
				 F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_types_factory->registerConstructor<T>(function_type, _registered_file,_registered_line );
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
				script_types_factory->registerMemberFunction<T>(_name_script_function,function_type, _registered_file,_registered_line );
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
				script_types_factory->registerConstMemberProperty<T>(
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
				script_types_factory->registerMemberPropertyMetamethod<T>(
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
		 * Register Static ScriptFunction Member Class
		 */
		template <typename T,typename F>
		void registerStaticMemberFunction(const String & _name_script_function,F _fun, const char *_registered_file="",short _registered_line=-1){
			try{
				script_types_factory->registerStaticMemberFunction<T>(_name_script_function,_fun, _registered_file, _registered_line);
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
		void unrefLifetimeObject(ScriptObject *so);
		void makeObjectShared(ScriptObject *so);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 8)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 8)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 9)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 9)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 10)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 10)>::type;


		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getObject(const String &function_access,ScriptObject **calling_obj,ScriptFunction **fun_obj );

		template <  typename F>
		std::function<F> bindScriptFunction(const String & function_access, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(MemberFunctionScriptObject *_sf, const char *_file="", int _line=-1);


		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *_sf, ScriptObject *_calling_object, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *_sf, const char *_file="", int _line=-1);


		//-----------------------------------------------------------------------------------------------------------------------
		/**
		* clear: clear compile information.
		*/
		void clear();
		void saveState();

		void addUnresolvedSymbol(ScriptFunction *_sf, zs_int _idx_instruction);
		void link();

		 ~ScriptEngine();

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
		Vector<ScriptFunction *>				functions_with_unresolved_symbols;

		VirtualMachine * virtual_machine;
		ScriptScopesFactory * scope_factory;
		ScriptFunctionsFactory *script_function_factory;
		ScriptTypesFactory *script_types_factory;

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
		//StackElement evalInternal(const char * _code, unsigned short _options=0, const char  *_filename="",CompilerData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);
		//StackElement	eval(const String & expresion, const char * _script_file_by_ref="", const char *__invoke_file__="", int __invoke_line__=-1);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

		//void setClearGlobalVariablesCheckpoint();
		void resetParsedFiles();
		const char *getFilenameByRef(const char * _filename_by_ref);
		void clearGlobalVariables(int _idx_start_variable=ZS_UNDEFINED_IDX, int _idx_start_function=ZS_UNDEFINED_IDX);
		bool getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf);

		template<typename R>
		R checkAndConvertStackElement(StackElement *_stk, ScriptTypeId _return_script_type_id);


	};

}

#include 	"ScriptEngine.tcc"


