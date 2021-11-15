/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

typedef enum{
	SCRIPT_CLASS_PROPERTY_C_OBJECT_REF=0x1
}ScriptClassProperty;

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptClass{

	public:

		int 			idx_class; 	// registered class idx

		zs_string 		class_name;		// info symbol class
		Scope			*class_scope;
		const char 		*str_class_ptr_type; // type_id().name();
		uint16_t		properties;

		//zs_vector *symbol_member_variables; // symbol_member_variable:  It can be attribute members or script member vars
		//zs_vector *symbol_member_functions; // symbol_member_functions: It can be static or member functions
		zs_vector 		*allocated_member_attributes;
		ScriptFunction	*sf_field_initializer;
		MemberProperty			*setter_getter;
		int						idx_starting_this_member_functions;
		int						idx_starting_this_member_variables;


		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;

		zs_vector						   	*   idx_base_classes; // list of idx of classes base




		 ScriptClass(ZetScript *_zs
				 ,short _idx_class
				 , zs_string _name
				 , Scope *_scope
				 ,const char *  str_class_ptr_type=TYPE_SCRIPT_VARIABLE
				 ,uint16_t _properties=0
		);

		 bool isDerivedFrom(short idx_class);

		//---------------------------------------------------
		// SCRIPT VARIABLES
		Symbol				* 	registerMemberVariable(
			 const zs_string & _symbol_name
			,unsigned short _symbol_properties
			,const char * _file=""
			,short _line=-1

		);

		template < typename R>
		void registerNativeMemberVariableStaticConst(
			const zs_string & _var_name
			, const R _var_pointer
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		Symbol				* 	registerNativeMemberVariable(
			 const zs_string & _symbol_name
			,const zs_string & _str_native_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_properties
			,const char * _file=""
			,short _line=-1

		);

		//---------------------------------------------------
		//
		// ATTRIBUTES
		//
		Symbol 				*	registerMemberProperty(
				const zs_string & _property_name
				,const char * _file=""
				,short _line=-1

		);
		
		//---------------
		// SETTER
		/*
		 * register C setter
		 */
		template <typename F>
		void registerNativeMemberPropertySetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *_registered_file=""
				,short _registered_line=-1
		);

		Symbol				* 	registerNativeMemberPropertySetter(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char * file=""
			,short line=-1

		);


		//---------------
		// GETTER
		template <typename F>
		void registerNativeMemberPropertyGetter(
				const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		);

		Symbol				* 	registerNativeMemberPropertyGetter(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_getter_function_properties
			,const char * _file=""
			,short _line=-1
		);


		//---------------
		// POST OPERATIONS

		template <typename F>
		void registerNativeMemberPropertyPostIncrement(
				const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		);

		Symbol				* 	registerNativeMemberPropertyPostIncrement(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_post_inc_function_properties
			,const char * _file=""
			,short _line=-1
		);

		//---
		template <typename F>
		void registerNativeMemberPropertyPostDecrement(
				const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		);

		Symbol				* 	registerNativeMemberPropertyPostDecrement(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_post_dec_function_properties
			,const char * _file=""
			,short _line=-1
		);


		//---------------
		// PRE OPERATIONS
		template <typename F>
		void registerNativeMemberPropertyPreIncrement(
				const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file
			,short _registered_line
		);

		Symbol				* 	registerNativeMemberPropertyPreIncrement(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_pre_inc_function_properties
			,const char * _file=""
			,short _line=-1
		);

		//---

		template <typename F>
		void registerNativeMemberPropertyPreDecrement(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyPreDecrement(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_pre_dec_function_properties
			,const char * _file=""
			,short _line=-1

		);

		//--------------------
		// OPERATIONS SETS

		// ADD SET
		template <typename F>
		void registerNativeMemberPropertyAddSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyAddSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_add_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// SUB SET
		template <typename F>
		void registerNativeMemberPropertySubSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);

		Symbol				* 	registerNativeMemberPropertySubSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_sub_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// MUL SET
		template <typename F>
		void registerNativeMemberPropertyMulSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyMulSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_mul_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// DIV SET
		template <typename F>
		void registerNativeMemberPropertyDivSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyDivSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_div_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// MOD SET
		template <typename F>
		void registerNativeMemberPropertyModSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyModSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_mod_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// AND SET
		template <typename F>
		void registerNativeMemberPropertyAndSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyAndSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_and_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// OR SET
		template <typename F>
		void registerNativeMemberPropertyOrSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyOrSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_or_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// XOR SET
		template <typename F>
		void registerNativeMemberPropertyXorSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyXorSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_xor_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// SHL SET
		template <typename F>
		void registerNativeMemberPropertyShlSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyShlSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_shl_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		// SHR SET
		template <typename F>
		void registerNativeMemberPropertyShrSet(
				const zs_string & _property_name
			,F _ptr_function
			, const char *registered_file
			,short registered_line
		);


		Symbol				* 	registerNativeMemberPropertyShrSet(
			const zs_string & _property_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_shr_set_function_properties
			,const char * _file=""
			,short _line=-1

		);

		//---------------------------------------------------
		// FUNCTIONS
		unsigned 	getNumNativeFunctions(const zs_string & function_name);

		Symbol				* 	registerMemberFunction(
				 const zs_string & _function_name
				,ScriptFunctionParam **_params=NULL
				,size_t _params_len=0
				, unsigned short _properties=0
				,const char * _file = ""
				, short _line=-1
		);

		Symbol				* 	registerNativeMemberFunction(
				 const zs_string & _function_name
				,ScriptFunctionParam **_params=NULL
				,size_t _params_len=0
				,int _idx_return_type=ZS_IDX_UNDEFINED
				,zs_int _ref_ptr=0
				,unsigned short _properties=0
				,const char * _file=""
				,short _line=-1
		);

		template <typename F>
		void registerNativeMemberFunctionStatic(
				const zs_string & _function_name
				,F _function_ptr
				, const char *_registered_file=""
				,short _registered_line=-1
		);

		template <typename F>
		void registerNativeMemberFunction(
				const zs_string & _function_name
				,F _function_type
				, const char *_registered_file=""
				,short _registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

		const char *getClassName();

		bool isNativeClass();
		bool isNativeSingletonClass();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		Symbol				* 	registerInternalMemberVariable(
			const zs_string & symbol_name
			, unsigned short properties
			,const zs_string & str_native_type=""
			,zs_int ref_ptr=0 // it's the offset from pointer or a pointer directly
			,const char *file=""
			, short line=-1

		);

		Symbol				* 	registerInternalMemberFunction(
			 const zs_string & _function_name
			,ScriptFunctionParam **_params
			,size_t _params_len
			, unsigned short _function_properties=0
			, int _idx_return_type=ZS_IDX_UNDEFINED
			, zs_int _ref_ptr=0
			, const char * _file=""
			, short _line=-1

		);


		ScriptClass * 					getScriptClass(short idx_class);
		short							getIdxClassFromItsNativeType(const char  * s);
		ScriptClass * 					getScriptClassByNativeClassPtr(const zs_string & class_type);
	};
}

#include "ScriptClass.tcc"

