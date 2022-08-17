/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define NEW_OBJECT_VAR_BY_TYPE_IDX(script_type_factory,idx)				((script_type_factory)->instanceScriptObjectByTypeIdx(idx))
#define GET_SCRIPT_TYPE(script_type_factory,idx_or_name)				((script_type_factory)->getScriptType(idx_or_name))
#define GET_SCRIPT_TYPE_NAME(script_type_factory,idx) 					((script_type_factory)->getScriptTypeName(idx))
#define SCRIPT_TYPE_MAIN(script_type_factory)							((script_type_factory)->getScriptType(IDX_TYPE_CLASS_MAIN))    // 0 is the main type

#define SCRIPT_OBJECT_STRING(script_type_factory)						((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_STRING))
#define SCRIPT_OBJECT_ITERATOR_STRING(script_type_factory)				((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING))
//#define SCRIPT_CLASS_DICTIONARY(script_type_factory)					((script_type_factory->script_type_factory)->getScriptType(IDX_TYPE_DICTIONARY))
#define SCRIPT_OBJECT_VECTOR(script_type_factory)						((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_VECTOR))
#define SCRIPT_OBJECT_ITERATOR_VECTOR(script_type_factory)				((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR))

#define SCRIPT_OBJECT_OBJECT(script_type_factory)						((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_ITERATOR_OBJECT(script_type_factory)				((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(script_type_factory)						((script_type_factory->script_type_factory)->getScriptType(IDX_TYPE_FUNCTION))
#define GET_IDX_2_CLASS_C_STR(script_type_factory,idx) 					((script_type_factory)->getScriptType(idx)->str_script_type_ptr.c_str())


namespace zetscript{

	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_zs_int_type_ptr;			//	typeid(zs_int *).name()
	extern const char * k_str_const_zs_int_type_ptr;	//	typeid(zs_int *).name()
	extern const char * k_str_zs_float_type_ptr;		//	typeid(zs_float *).name()
	extern const char * k_str_const_zs_float_type_ptr;	//	typeid(zs_float *).name()
	extern const char * k_str_string_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(std::string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char *	k_str_zs_int_type;				//	typeid(zs_int).name()

	extern const char * k_str_zs_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()

	class ScriptObjectObject;
	class ScriptType;
	class ZetScript;

	typedef zs_int (*ConversionType)(zs_int);


	class ScriptTypeFactory{

	public:

		ScriptTypeFactory(ZetScript *_zs);
		void init();
		void registerSystem();

		 /**
		  * Class management region
		  */
		ScriptType * 					registerScriptType(
											 const std::string & str_script_type
											,const std::string & base_class_name=""
											,uint16_t _properties=0
											,const char * file=""
											, short line=-1
										);

		ScriptType * 					getScriptType(short _idx_script_type);
		ScriptType * 					getScriptType(const std::string & _type_name);
		ScriptType * 					getScriptTypeFromTypeNamePtr(const std::string & _type_name_ptr);

		short							getIdxScriptType(const std::string & _type_name);
		short		 					getIdxScriptTypeFromTypeNamePtr(const std::string & _type_name_ptr);

		const char 	* 					getScriptTypeName(short _idx_script_type);
		bool							scriptTypeInheritsFrom(short _idx_class_type,short _idx_class_type_inherits_from);
		bool							isScriptTypeInstanceable(short _idx_script_type);

		std::vector<ScriptType *>	*	getScriptTypes();

		bool 							isScriptTypeRegistered(const std::string & _type_name);

		inline ScriptType * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}


		// register object
		StackElement * registerStkConstantStringObject(const std::string & _key,const std::string & _value);
		StackElement * getStkConstantStringObject(const std::string & _key);

		StackElement * registerStkObject(const std::string & _key, zs_int _value);
		StackElement * getStkObject(const std::string & _key);

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByTypeName(const std::string & str_script_type);
		ScriptObject 			* 			instanceScriptObjectByTypeIdx(short  idx_script_type, void * value_object = NULL);

		/**
		 * Register C variable
		 */
		template <typename V>
		void bindGlobalVariable(
			 const std::string & var_name
			 ,V var_ptr
			 , const char *registered_file=""
			 ,short registered_line=-1
		);

		void bindConstantVariable(const std::string & var_name, int value, const char *registered_file="", short registered_line=-1);
		void bindConstantVariable(const std::string & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void bindConstantVariable(const std::string & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void bindConstantVariable(const std::string & var_name, const std::string & v, const char *registered_file="", short registered_line=-1);
		void bindConstantVariable(const std::string & var_name, const char * v, const char *registered_file="", short registered_line=-1);

		/**
		 * Register C function
		 */
		 template <typename F>
		 void bindFunction(
			const std::string &  name_script_function
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Instenceable Register C type
		 */
		template<typename C>
		ScriptType * bindType(
			const std::string & str_script_type
			, C * (*_constructor)(ZetScript *_zs)=NULL
			, void (*_destructor)(ZetScript *_zs,C *)=NULL
			, const char *registered_file=""
			,short registered_line=-1
		);


		/**
		 * Built in register C Class, like ScriptObject,ScriptObjectString...
		 */
		template<class C, class B>
		void extends();

		void bindMemberSymbols();

		/**
		 * Register C Member constructor
		 */
		template <typename C,typename F>
		void bindConstructor(
				F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		template <typename C, typename R>
		void bindStaticConstMember(
				const std::string & var_name
				, const R var_pointer
				 , const char *registered_file=""
				,short registered_line=-1
		);

		template <typename C,typename F>
		void bindMemberPropertySetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register property getter
		 */
		template <typename C,typename F>
		void bindMemberPropertyGetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_inc
		 */
		template <typename C,typename F>
		void bindMemberPropertyPostIncrement(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_dec
		 */
		template <typename C,typename F>
		void bindMemberPropertyPostDecrement(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_inc
		 */
		template <typename C,typename F>
		void bindMemberPropertyPreIncrement(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_dec
		 */
		template <typename C,typename F>
		void bindMemberPropertyPreDecrement(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property add set operation
		template <typename C,typename F>
		void bindMemberPropertyAddSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property sub set operation
		template <typename C,typename F>
		void bindMemberPropertySubSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mul set operation
		template <typename C,typename F>
		void bindMemberPropertyMulSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register div set operation
		template <typename C,typename F>
		void bindMemberPropertyDivSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mod set operation
		template <typename C,typename F>
		void bindMemberPropertyModSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register and set operation
		template <typename C,typename F>
		void bindMemberPropertyAndSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register or set operation
		template <typename C,typename F>
		void bindMemberPropertyOrSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register xor set operation
		template <typename C,typename F>
		void bindMemberPropertyXorSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shl set operation
		template <typename C,typename F>
		void bindMemberPropertyShlSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shr set operation
		template <typename C,typename F>
		void bindMemberPropertyShrSetter(
				const std::string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/**
		 * Register C Member function static Class
		 */
		template <typename C, typename F>
		void bindStaticMemberFunction(
				const std::string & _function_name
				,F _ptr_function
				, const char *_registered_file=""
				,short _registered_line=-1);



		/**
		 * Register C function as function member
		 */
		template <typename C,typename F>
		void	bindMemberFunction(
				const std::string & name_script_function
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		);

		void 	clear(short _idx_start=ZS_IDX_UNDEFINED);
		void 	saveState();


		~ScriptTypeFactory();

	private:

		typedef struct{
			const char *   type_str;
			Type  id;
		}PrimitiveType;

		std::map<std::string,StackElement*>		*stk_constants,*stk_objects;
		std::vector<ScriptType *>				*script_types;
		ZetScript 								*	zs;
		VirtualMachine							*	vm;
		ScopeFactory 							*	scope_factory;
		ScriptFunctionFactory 					*	script_function_factory;
		ScriptType 								* 	main_object;
		ScriptFunction 							* 	main_function;

		int 							idx_clear_checkpoint;

		void							checkScriptTypeName(const std::string & str_script_type);
		PrimitiveType *					getPrimitiveTypeFromStr(const std::string & str);


		void 							registerPrimitiveTypes();

		short							getIdxTypeFromTypeNameInternal(const std::string & _type_name);

		void 							setup();
		void 							internalPrintError(const char *s);


	};

}
#include "ScriptTypeFactory.tcc"

