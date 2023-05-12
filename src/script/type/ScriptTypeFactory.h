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
#define SCRIPT_OBJECT_ARRAY(script_type_factory)						((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ARRAY))
#define SCRIPT_OBJECT_ITERATOR_ARRAY(script_type_factory)				((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY))

#define SCRIPT_OBJECT_OBJECT(script_type_factory)						((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_ITERATOR_OBJECT(script_type_factory)				((script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(script_type_factory)						((script_type_factory->script_type_factory)->getScriptType(IDX_TYPE_FUNCTION))
#define GET_IDX_2_CLASS_C_STR(script_type_factory,idx) 					((script_type_factory)->getScriptType(idx)->str_script_type_ptr.c_str())


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
											 const zs_string & str_script_type
											,const zs_string & base_class_name=""
											,uint16_t _properties=0
											,const char * file=""
											, short line=-1
										);

		ScriptType * 					getScriptType(short _idx_script_type);
		ScriptType * 					getScriptType(const zs_string & _type_name);
		ScriptType * 					getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr);

		short							getIdxScriptType(const zs_string & _type_name);
		short		 					getIdxScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr);

		const char 	* 					getScriptTypeName(short _idx_script_type);
		bool							scriptTypeInheritsFrom(short _idx_class_type,short _idx_class_type_inherits_from);
		bool							isScriptTypeInstanceable(short _idx_script_type);

		zs_vector<ScriptType *>	*		getScriptTypes();

		bool 							isScriptTypeRegistered(const zs_string & _type_name);

		inline ScriptType * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByTypeName(const zs_string & str_script_type);
		ScriptObject 			* 			instanceScriptObjectByTypeIdx(short  idx_script_type, void * value_object = NULL);


		/**
		 * Register C function
		 */
		 template <typename F>
		 void registerFunction(
			const zs_string &  name_script_function
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Instenceable Register C type
		 */
		template<typename C>
		ScriptType * registerClass(
			const zs_string & str_script_type
			, C * (*_constructor)(ZetScript *_zs)=NULL
			, void (*_destructor)(ZetScript *_zs,C *)=NULL
			, const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Built in register C Class, like ScriptObject,StringScriptObject...
		 */
		template<class C, class B>
		void extends();

		void registerMemberSymbols();

		/**
		 * Register C Member constructor
		 */
		template <typename C,typename F>
		void registerConstructor(
				F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register static property getter
		 */
		template <typename C,typename F>
		void registerStaticMemberPropertyGetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register property setter
		 */
		template <typename C,typename F>
		void registerMemberPropertySetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register property getter
		 */
		template <typename C,typename F>
		void registerMemberPropertyGetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register property neg
		 */
		template <typename C,typename F>
		void registerMemberPropertyNeg(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register property bwc
		 */
		template <typename C,typename F>
		void registerMemberPropertyBwc(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_inc
		 */
		template <typename C,typename F>
		void registerMemberPropertyPostInc(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_dec
		 */
		template <typename C,typename F>
		void registerMemberPropertyPostDec(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_inc
		 */
		template <typename C,typename F>
		void registerMemberPropertyPreInc(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_dec
		 */
		template <typename C,typename F>
		void registerMemberPropertyPreDec(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property add set operation
		template <typename C,typename F>
		void registerMemberPropertyAddSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property sub set operation
		template <typename C,typename F>
		void registerMemberPropertySubSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mul set operation
		template <typename C,typename F>
		void registerMemberPropertyMulSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register div set operation
		template <typename C,typename F>
		void registerMemberPropertyDivSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mod set operation
		template <typename C,typename F>
		void registerMemberPropertyModSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register and set operation
		template <typename C,typename F>
		void registerMemberPropertyAndSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register or set operation
		template <typename C,typename F>
		void registerMemberPropertyOrSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register xor set operation
		template <typename C,typename F>
		void registerMemberPropertyXorSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shl set operation
		template <typename C,typename F>
		void registerMemberPropertyShlSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shr set operation
		template <typename C,typename F>
		void registerMemberPropertyShrSetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/**
		 * Register C Member function static Class
		 */
		template <typename C, typename F>
		void registerStaticMemberFunction(
				const zs_string & _function_name
				,F _ptr_function
				, const char *_registered_file=""
				,short _registered_line=-1);



		/**
		 * Register C function as function member
		 */
		template <typename C,typename F>
		void	registerMemberFunction(
				const zs_string & name_script_function
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

		zs_vector<ScriptType *>			*	script_types;
		ZetScript 						*	zs;
		VirtualMachine					*	vm;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		ScriptType 						* 	main_object;
		ScriptFunction 					* 	main_function;

		int 							idx_clear_checkpoint;

		void							checkScriptTypeName(const zs_string & str_script_type);
		PrimitiveType *					getPrimitiveTypeFromStr(const zs_string & str);


		void 							registerPrimitiveTypes();

		short							getIdxTypeFromTypeNameInternal(const zs_string & _type_name);

		void 							internalPrintError(const char *s);

	};

}


