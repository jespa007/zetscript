/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_BY_TYPE_ID(script_types_factory,script_type_id)		((script_types_factory)->instanceObjectByScriptTypeId(script_type_id))
#define ZS_GET_OBJECT_TYPE(script_types_factory,idx_or_name)					((script_types_factory)->getScriptType(idx_or_name))
#define ZS_GET_TYPE_NAME(script_types_factory,script_type_id) 					((script_types_factory)->getScriptTypeName(script_type_id))
#define ZS_TYPE_MAIN(script_types_factory)										((script_types_factory)->getScriptType(SCRIPT_TYPE_ID_CLASS_MAIN))    // 0 is the main type

#define ZS_OBJECT_STRING(script_types_factory)									((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT))
#define ZS_STRING_ITERATOR_SCRIPT_OBJECT(script_types_factory)					((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT))
//#define SCRIPT_CLASS_DICTIONARY(script_types_factory)							((script_types_factory->script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_DICTIONARY))
#define ZS_OBJECT_ARRAY(script_types_factory)									((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT))
#define ZS_ARRAY_ITERATOR_SCRIPT_OBJECT(script_types_factory)					((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT))

#define ZS_OBJECT_SCRIPT_OBJECT(script_types_factory)							((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT))
#define ZS_OBJECT_ITERATOR_SCRIPT_OBJECT(script_types_factory)					((script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_OBJECT_ITERATOR_SCRIPT_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(script_types_factory)					((script_types_factory->script_types_factory)->getScriptType(ScriptTypeId::SCRIPT_TYPE_ID_SCRIPT_FUNCTION))
#define ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(script_types_factory,script_type_id) 		((script_types_factory)->getScriptType(script_type_id)->native_name.toConstChar())


namespace zetscript{

	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_int_type_ptr;			//	typeid(zs_int *).name()
	extern const char * k_str_const_int_type_ptr;	//	typeid(zs_int *).name()
	extern const char * k_str_float_type_ptr;			//	typeid(zs_float *).name()
	extern const char * k_str_const_float_type_ptr;		//	typeid(zs_float *).name()
	extern const char * k_str_string_type_ptr;			//	typeid(String *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(String *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(String *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char *	k_str_int_type;				//	typeid(zs_int).name()

	extern const char * k_str_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()

	class ObjectScriptObject;
	class ScriptFunction;
	class ScriptType;
	class ScriptEngine;

	typedef zs_int (*ConversionType)(zs_int);


	class ScriptTypesFactory{

	public:

		ScriptTypesFactory(ScriptEngine *_script_engine);
		void init();
		void setup();

		 /**
		  * Class management region
		  */
		ScriptType * 					registerScriptType(
											 const String & name
											,const String & base_class_name=""
											,uint16_t _properties=0
											,const char * file=""
											, short line=-1
										);

		ScriptType * 					getScriptType(ScriptTypeId _script_type_id);
		ScriptType * 					getScriptType(const String & _script_type_name);
		ScriptType * 					getScriptTypeFromNamePtr(const String & _script_type_name_ptr);

		ScriptTypeId					getScriptTypeId(const String & _script_type_name);
		ScriptTypeId		 			getScriptTypeIdFromNamePtr(const String & _script_type_name_ptr);

		const char 	* 					getScriptTypeName(ScriptTypeId _script_type_id);
		bool							scriptTypeInheritsFrom(ScriptTypeId _script_type_id,ScriptTypeId _script_type_id_inherits_from);
		bool							isTypeInstanceable(ScriptTypeId _script_type_id);

		Vector<ScriptType *>	*		getScriptTypes();

		bool 							isScriptTypeRegistered(const String & _script_type_name);

		inline ScriptType * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceObjectByScriptTypeName(const String & name);
		ScriptObject 			* 			instanceObjectByScriptTypeId(ScriptTypeId  _script_type_id, void * value_object = NULL);


		/**
		 * Register T function
		 */
		 template <typename F>
		 void registerFunction(
			const String &  name
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Instenceable Register T type
		 */
		template<typename T>
		ScriptType * registerScriptType(
			const String & name
			, T * (*_new_native_instance)(ScriptEngine *_script_engine)=NULL
			, void (*_delete_native_instance)(ScriptEngine *_script_engine,T *)=NULL
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
			const String & _property_name
			,F _ptr_function
			, const char *registered_file=NULL
			,short registered_line=-1
		);

		/*
		 * register member property metamethod
		 */

		template <typename T,typename F>
		void registerMemberPropertyMetamethod(
			const String & _property_name
			,const String & _metamethod_name
			,F _ptr_function
			, const char *registered_file=NULL
			,short registered_line=-1
		);


		/**
		 * Register T Member function static Class
		 */
		template <typename T, typename F>
		void registerStaticMemberFunction(
			const String & _function_name
			,F _ptr_function
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		/**
		 * Register T function as function member
		 */
		template <typename T,typename F>
		void	registerMemberFunction(
			const String & name
			,F ptr_function
			 , const char *registered_file=""
			,short registered_line=-1
		);

		void 	clear(short _idx_start=ZS_UNDEFINED_IDX);
		void 	saveState();


		~ScriptTypesFactory();

	private:

		typedef struct{
			const char *   type_str;
			ScriptTypeId  id;
		}PrimitiveType;

		Vector<ScriptType *>			*	types;
		ScriptEngine 					*	script_engine;
		VirtualMachine					*	vm;
		ScriptScopesFactory 			*	scope_factory;
		ScriptFunctionsFactory 			*	script_function_factory;
		ScriptType 						* 	main_object;
		ScriptFunction 					* 	main_function;

		int 								idx_clear_checkpoint;

		void								checkTypeName(const String & name);
		PrimitiveType *						getPrimitiveTypeFromStr(const String & str);


		void 								registerPrimitiveTypes();

		short								getIdxTypeFromTypeNameInternal(const String & _script_type_name);

		void 								internalPrintError(const char *s);

	};

}


