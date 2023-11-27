/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define NEW_OBJECT_VAR_BY_TYPE_ID(type_factory,type_id)		((type_factory)->instanceObjectByTypeId(type_id))
#define GET_OBJECT_TYPE(type_factory,idx_or_name)							((type_factory)->getType(idx_or_name))
#define GET_TYPE_NAME(type_factory,type_id) 					((type_factory)->getTypeName(type_id))
#define TYPE_MAIN(type_factory)										((type_factory)->getType(TYPE_ID_CLASS_MAIN))    // 0 is the main type

#define OBJECT_STRING(type_factory)									((type_factory)->getType(TypeId::TYPE_ID_OBJECT_STRING))
#define OBJECT_ITERATOR_ASSIGNRING(type_factory)						((type_factory)->getType(TypeId::TYPE_ID_OBJECT_ITERATOR_STRING))
//#define SCRIPT_CLASS_DICTIONARY(type_factory)								((type_factory->type_factory)->getType(TypeId::TYPE_ID_DICTIONARY))
#define OBJECT_ARRAY(type_factory)									((type_factory)->getType(TypeId::TYPE_ID_OBJECT_ARRAY))
#define OBJECT_ITERATOR_ARRAY(type_factory)							((type_factory)->getType(TypeId::TYPE_ID_OBJECT_ITERATOR_ARRAY))

#define OBJECT_OBJECT(type_factory)									((type_factory)->getType(TypeId::TYPE_ID_OBJECT_OBJECT))
#define OBJECT_ITERATOR_OBJECT(type_factory)							((type_factory)->getType(TypeId::TYPE_ID_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(type_factory)									((type_factory->type_factory)->getType(TypeId::TYPE_ID_FUNCTION))
#define TYPE_ID_TO_NATIVE_NAME(type_factory,type_id) 			((type_factory)->getType(type_id)->native_name.toConstChar())


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

	class ObjectObject;
	class Function;
	class Type;
	class ZetScript;

	typedef zs_int (*ConversionType)(zs_int);


	class TypeFactory{

	public:

		TypeFactory(ZetScript *_zs);
		void init();
		void setup();

		 /**
		  * Class management region
		  */
		Type * 					registerType(
											 const String & name
											,const String & base_class_name=""
											,uint16_t _properties=0
											,const char * file=""
											, short line=-1
										);

		Type * 					getType(TypeId _type_id);
		Type * 					getType(const String & _type_name);
		Type * 					getTypeFromTypeNamePtr(const String & _type_name_ptr);

		TypeId					getTypeId(const String & _type_name);
		TypeId		 			getTypeIdFromTypeNamePtr(const String & _type_name_ptr);

		const char 	* 					getTypeName(TypeId _type_id);
		bool							scriptTypeInheritsFrom(TypeId _type_id,TypeId _type_id_inherits_from);
		bool							isTypeInstanceable(TypeId _type_id);

		Vector<Type *>	*		getTypes();

		bool 							isTypeRegistered(const String & _type_name);

		inline Type * 			getMainObject() { return main_object;}
		inline Function * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		Object 			* 			instanceObjectByTypeName(const String & name);
		Object 			* 			instanceObjectByTypeId(TypeId  _type_id, void * value_object = NULL);


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
		Type * registerType(
			const String & name
			, T * (*_new_native_instance)(ZetScript *_zs)=NULL
			, void (*_delete_native_instance)(ZetScript *_zs,T *)=NULL
			, const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Built in register T Class, like Object,StringObject...
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


		~TypeFactory();

	private:

		typedef struct{
			const char *   type_str;
			TypeId  id;
		}PrimitiveType;

		Vector<Type *>			*	types;
		ZetScript 						*	zs;
		VirtualMachine					*	vm;
		ScopeFactory 					*	scope_factory;
		FunctionFactory 			*	script_function_factory;
		Type 						* 	main_object;
		Function 					* 	main_function;

		int 							idx_clear_checkpoint;

		void							checkTypeName(const String & name);
		PrimitiveType *					getPrimitiveTypeFromStr(const String & str);


		void 							registerPrimitiveTypes();

		short							getIdxTypeFromTypeNameInternal(const String & _type_name);

		void 							internalPrintError(const char *s);

	};

}


