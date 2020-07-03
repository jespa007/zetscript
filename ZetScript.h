#pragma once

#define ZS_CLASS_C_BASEOF(zs)										(zs)->class_C_BaseOf();
#define ZS_REGISTER_C_FUNCTION(zs,text,s) 							(zs)->Register_C_Function(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE(zs,text,s) 							(zs)->Register_C_Variable(text,&s,typeid(decltype(&s)).name(),__FILE__, __LINE__)
#define ZS_REGISTER_C_CLASS(zs,class_type,s) 						(zs)->Register_C_Class<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_SINGLETON_CLASS(zs,class_type,s)				(zs)->register_C_SingletonClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE_MEMBER(zs,class_type,s,v)			(zs)->Register_C_VariableMember<class_type>(s,v)
#define ZS_REGISTER_C_STATIC_FUNCTION_MEMBER(zs,class_type,s,f)		(zs)->Register_C_FunctionMemberStatic<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_FUNCTION_MEMBER(zs,class_type,s,f)			(zs)->register_C_FunctionMember<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_CONSTANT_INT(zs,constant_name,v)				(zs)->registerConstantIntValue(constant_name,v)


namespace Zetscript{
	class CVirtualMachine;
	class ScriptEval;
	class CZetScript{



	public:

		std::string  STR_VOID_TYPE;				// 	typeid(void).name()
		std::string  STR_INT_TYPE_PTR;			//	typeid(int *).name()
		std::string  STR_FLOAT_TYPE_PTR;		//	typeid(float *).name()
		std::string  STR_STRING_TYPE_PTR;		//	typeid(std::string *).name()
		std::string  STR_CONST_CHAR_TYPE_PTR;	//	typeid(std::string *).name()
		std::string  STR_BOOL_TYPE_PTR;			//	typeid(bool *).name()
		std::string  STR_INT_TYPE;				//	typeid(int).name()
		std::string  STR_UNSIGNED_INT_TYPE;		//	typeid(unsigned int).name()
		std::string  STR_INTPTR_T_TYPE;			//	typeid(intptr_t).name()

		std::string  STR_FLOAT_TYPE;			//	typeid(int).name()
		std::string  STR_BOOL_TYPE;				//	typeid(bool).name()
		std::string  STR_STACK_ELEMENT_TYPE;	//	typeid(bool).name()

		//===================================================================================================
		//
		// PRINT ASM INFO
		void printGeneratedCode();

		// PRINT ASM INFO
		//---------------------------------------------------------------------------------------------------------------------------------------



		CZetScript();

		inline CVirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline NativeFunctionFactory * getNativeFunctionFactory() { return native_function_factory;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline ScriptClassFactory *getScriptClassFactory() { return script_class_factory;}


		void	set_callback_on_error(PrintFunctionCallback _fun);

		int * evalIntValue(const std::string & str_to_eval);
		bool * evalBoolValue(const std::string & str_to_eval);
		float * evalFloatValue(const std::string & str_to_eval);
		std::string * evalStringValue(const std::string & str_to_eval);

		bool eval(const std::string & expresion, bool execute=true,bool show_bytecode=false, const char * filename="");
		bool evalFile(const std::string & filename,bool execute=true,bool show_bytecode=false);

		ConstantValueInfo 	* 		registerConstantIntValue(const std::string & const_name, int value);

		// CONSTANT TOOLS
		ConstantValueInfo * getRegisteredConstantValue(const std::string & const_name);
		ConstantValueInfo * registerConstantValue(const std::string & const_name, void *obj, unsigned short properties);

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

		void 						setPrintOutCallback(void (*)(const char *));


		StackElement 					C_REF_InfoVariable_2_StackElement(VariableSymbolInfo *ir_var, void *ptr_variable);

		int getIdxClassFromIts_C_Type(const std::string & str_type){
			return script_class_factory->getIdxClassFromIts_C_Type(str_type);
		}


		const char * getMetamethod(METAMETHOD_OP_CODE op);
		void 												register_C_BaseSymbols(bool r){
			script_class_factory->register_C_BaseSymbols(r);
		}

		/**
		 * Register C function
		 */
		template <typename F>
		bool Register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_Function( function_name,function_ptr, registered_file,registered_line);
		}

		/**
		 * Register C variable
		 */
		template <typename V>
		 VariableSymbolInfo * Register_C_Variable(const std::string & var_str,V var_ptr, const char *registered_file="",int registered_line=-1){
			 return script_class_factory->Register_C_Variable(var_str,var_ptr, typeid(V).name(), registered_file, registered_line);
		 }

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		 bool register_C_SingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_SingletonClass<C>(class_name, registered_file, registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		bool Register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_Class<C>(class_name, registered_file,registered_line);
		}


		template<typename C>
		bool Register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1){
			return script_class_factory->Register_C_ClassBuiltIn<C>(class_name, registered_file, registered_line);
		}


		template<class C, class B>
		bool class_C_BaseOf(){
			return script_class_factory->class_C_BaseOf<C,B>();
		}

		/**
		 * Register Function Member Class
		 */
		template < typename C, typename R, class T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 ){
			return script_class_factory->register_C_FunctionMember<C>(function_name,function_type, registered_file,registered_line );
		}


		/**
		 * Register Static Function Member Class
		 */
		template <typename _F>
		bool Register_C_FunctionMemberStatic(const char *function_name,_F fun, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_FunctionMemberStatic(function_name,fun, registered_file, registered_line);

		}

		/**
		 * Register C function as function member
		 */
		template <typename F>
		bool Register_C_FunctionAsFunctionMember( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_FunctionAsFunctionMember( function_name,function_ptr, registered_file,registered_line);
		}


		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool Register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_VariableMember<C>(var_name,var_pointer,registered_file,registered_line);
		}

		//cpp binding
		// Helpers...
		inline StackElement varToStk(intptr_t var_trans, int idx_type);
		inline bool stkToVar(StackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error);


		template<typename T>
		static CScriptVarVector * stdVectorToScriptVarVector(const std::vector<T> & v,CZetScript *zs_instance){
			CScriptVarVector *vsv = new CScriptVarVector(zs_instance);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->push();
				//intptr_t uvar = (intptr_t)(v[i]);
				*stk = zs_instance->varToStk((intptr_t)(v[i]),zs_instance->getIdxClassFromIts_C_Type(typeid(T).name()));
			}

			return vsv;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,CScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;



		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, CScriptVar *calling_obj,ScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, CScriptVar *calling_obj,ScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;



		bool getScriptObject(const std::string &function_access,CScriptVar **calling_obj,ScriptFunction **fun_obj );


		template <  typename F>
		std::function<F> * bindScriptFunction(const std::string & function_access);





		 ~CZetScript();

	private:




		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedSourceInfo;




		//--------
		// VARS
		std::map<std::string,ConstantValueInfo *> 	 m_contantPool;
		std::vector<ParsedSourceInfo> 			 m_parsedSource;
		std::map<std::string,ConstantValueInfo *> 	 constant_pool;
		ScriptEval * eval_obj;
		CVirtualMachine * virtual_machine;
		NativeFunctionFactory * native_function_factory;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory *script_class_factory;


		CZetScript 		*zs;

		float eval_float;
		int eval_int;
		bool eval_bool;
		std::string eval_string;


		//===================================================================================================
		//
		// PRINT ASM INFO
		std::string getStrMovVar(ByteCode * iao);
		std::string getStrTypeLoadValue(ScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction);
		void printGeneratedCode(ScriptFunction *sfo);

		//----




		// FUNCTIONS
		static 									void  print(const char *s);
		static 									void (* print_out_callback)(const char *);





	};

}

#include "CVirtualMachine.h"
#include "CZetScript.inc"
#include "helper/cpp_binding.h"
