/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define NEW_OBJECT_VAR_BY_TYPE_IDX(script_type_factory,idx)				((script_type_factory)->instanceScriptObjectByTypeIdx(idx))
#define GET_SCRIPT_TYPE(script_type_factory,idx_or_name)				((script_type_factory)->getScriptType(idx_or_name))
#define GET_SCRIPT_TYPE_NAME(script_type_factory,idx) 					((script_type_factory)->getScriptTypeName(idx))
#define SCRIPT_TYPE_MAIN(script_type_factory)							((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_CLASS_MAIN))    // 0 is the main type

#define SCRIPT_OBJECT_STRING(script_type_factory)						((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING))
#define SCRIPT_OBJECT_ITERATOR_ASSIGNRING(script_type_factory)				((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_STRING))
//#define SCRIPT_CLASS_DICTIONARY(script_type_factory)					((script_type_factory->script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_DICTIONARY))
#define SCRIPT_OBJECT_ARRAY(script_type_factory)						((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ARRAY))
#define SCRIPT_OBJECT_ITERATOR_ARRAY(script_type_factory)				((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_ARRAY))

#define SCRIPT_OBJECT_OBJECT(script_type_factory)						((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_ITERATOR_OBJECT(script_type_factory)				((script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(script_type_factory)						((script_type_factory->script_type_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_FUNCTION))
#define GET_IDX_2_CLASS_C_STR(script_type_factory,idx) 					((script_type_factory)->getScriptType(idx)->native_name.c_str())


namespace zetscript{

	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_int_type_ptr;			//	typeid(zs_int *).name()
	extern const char * k_str_const_int_type_ptr;	//	typeid(zs_int *).name()
	extern const char * k_str_float_type_ptr;			//	typeid(zs_float *).name()
	extern const char * k_str_const_float_type_ptr;		//	typeid(zs_float *).name()
	extern const char * k_str_string_type_ptr;			//	typeid(zs_string *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(zs_string *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(zs_string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char *	k_str_int_type;				//	typeid(zs_int).name()

	extern const char * k_str_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()

	class ObjectScriptObject;
	class ScriptFunction;
	class ScriptType;
	class ZetScript;

	typedef zs_int (*ConversionType)(zs_int);


	class ScriptTypeFactory{

	public:

		ScriptTypeFactory(ZetScript *_zs);
		void init();
		void setup();

		 /**
		  * Class management region
		  */
		ScriptType * 					registerScriptType(
											 const zs_string & name
											,const zs_string & base_class_name=""
											,uint16_t _properties=0
											,const char * file=""
											, short line=-1
										);

		ScriptType * 					getScriptType(short _idx_type);
		ScriptType * 					getScriptType(const zs_string & _type_name);
		ScriptType * 					getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr);

		short							getScriptTypeId(const zs_string & _type_name);
		short		 					getScriptTypeIdFromTypeNamePtr(const zs_string & _type_name_ptr);

		const char 	* 					getScriptTypeName(short _idx_type);
		bool							scriptTypeInheritsFrom(short _idx_class_type,short _idx_class_type_inherits_from);
		bool							isScriptTypeInstanceable(short _idx_type);

		zs_vector<ScriptType *>	*		getScriptTypes();

		bool 							isScriptTypeRegistered(const zs_string & _type_name);

		inline ScriptType * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByTypeName(const zs_string & name);
		ScriptObject 			* 			instanceScriptObjectByTypeIdx(short  id, void * value_object = NULL);


		/**
		 * Register T function
		 */
		 template <typename F>
		 void registerFunction(
			const zs_string &  name
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Instenceable Register T type
		 */
		template<typename T>
		ScriptType * registerType(
			const zs_string & name
			, T * (*_new_native_instance)(ZetScript *_zs)=NULL
			, void (*_delete_native_instance)(ZetScript *_zs,T *)=NULL
			, const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Built in register T Class, like ScriptObject,StringScriptObject...
		 */
		template<class T, class B>
		void extends();

		void registerMemberSymbols();

		/**
		 * Register T Member constructor
		 */
		template <typename T,typename F>
		void registerConstructor(
				F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register static member property metamethod
		 */
		template <typename T,typename F>
		void registerConstMemberProperty(
			const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file=NULL
			,short registered_line=-1
		);

		/*
		 * register member property metamethod
		 */

		template <typename T,typename F>
		void registerMemberPropertyMetamethod(
			const zs_string & _property_name
			,const zs_string & _metamethod_name
			,F _ptr_function
			, const char *registered_file=NULL
			,short registered_line=-1
		);


		/**
		 * Register T Member function static Class
		 */
		template <typename T, typename F>
		void registerStaticMemberFunction(
			const zs_string & _function_name
			,F _ptr_function
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		/**
		 * Register T function as function member
		 */
		template <typename T,typename F>
		void	registerMemberFunction(
			const zs_string & name
			,F ptr_function
			 , const char *registered_file=""
			,short registered_line=-1
		);

		void 	clear(short _idx_start=ZS_UNDEFINED_IDX);
		void 	saveState();


		~ScriptTypeFactory();

	private:

		typedef struct{
			const char *   type_str;
			ScriptTypeId  id;
		}PrimitiveType;

		zs_vector<ScriptType *>			*	script_types;
		ZetScript 						*	zs;
		VirtualMachine					*	vm;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		ScriptType 						* 	main_object;
		ScriptFunction 					* 	main_function;

		int 							idx_clear_checkpoint;

		void							checkScriptTypeName(const zs_string & name);
		PrimitiveType *					getPrimitiveTypeFromStr(const zs_string & str);


		void 							registerPrimitiveTypes();

		short							getIdxTypeFromTypeNameInternal(const zs_string & _type_name);

		void 							internalPrintError(const char *s);

	};

}


